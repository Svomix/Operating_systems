package lab5;

import lab5.CRUD.CRUD;
import lab5.Entity.Car;
import lombok.RequiredArgsConstructor;
import org.hibernate.SessionFactory;
import org.hibernate.cfg.Configuration;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


@RequiredArgsConstructor
public class HandlerClient extends Thread {
    private final int i;
    private final Socket socket;
    private final List<Socket> connections;
    private final SessionFactory sessionFactory = new Configuration().configure("hibernate.cfg.xml").buildSessionFactory();
    private final Lock lock = new ReentrantLock();

    private String receiveMessage(Socket socket) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        return reader.readLine();
    }

    private void sendMessage(Socket socket, String message) throws IOException {
        PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
        writer.println(message);
    }

    private void processSelectOperation(String[] parts) throws IOException {
        if (parts.length < 4) {
            sendMessage(socket, "Invalid select command format");
            return;
        }

        String property = parts[1];
        String sign = parts[2];
        String value = parts[3];

        CRUD.Choice choice;
        try {
            choice = CRUD.Choice.valueOf(property.toUpperCase());
        } catch (IllegalArgumentException e) {
            sendMessage(socket, "Invalid property");
            return;
        }

        CRUD.Query query = CRUD.Query.builder().value(value).sign(sign).build();
        lock.lock();
        try {
            List<Car> cars = CRUD.read(sessionFactory, choice, query);
            sendMessage(socket, String.valueOf(cars.size()));

            String ack = receiveMessage(socket);
            System.out.println("Message from client " + (i + 1) + ": " + ack);

            for (Car car : cars) {
                String carProps = car.getName() + " " + car.getMaxSpeed() + " " +
                        car.getTankCapacity() + " " + car.getEnginePower() + " " +
                        car.getPrice();
                sendMessage(socket, carProps);

                ack = receiveMessage(socket);
                System.out.println("Message from client " + (i + 1) + ": " + ack);
            }
        } finally {
            lock.unlock();
        }
    }

    private void processAddOperation(String[] parts) throws IOException {
        String name = "";
        int maxSpeed = 0;
        int tankCapacity = 0;
        int enginePower = 0;
        double price = 0;

        try {
            name = parts[3];
            maxSpeed = Integer.parseInt(parts[6]);
            tankCapacity = Integer.parseInt(parts[9]);
            price = Double.parseDouble(parts[12]);
            enginePower = Integer.parseInt(parts[15]);
            lock.lock();
            try {
                boolean success = CRUD.create(sessionFactory, maxSpeed, tankCapacity, enginePower, (int) price, name);
                sendMessage(socket, success ? "Saving has been done" : "Saving hasn't been done");
                for (Socket connection : connections) {
                    if (connection != socket)
                        sendMessage(connection, "New car has been created");
                }
            } finally {
                lock.unlock();
            }
        } catch (NumberFormatException e) {
            sendMessage(socket, "Invalid number format");
        }
    }

    private void processChangeOperation(String[] parts) throws IOException {
        if (parts.length < 3) {
            sendMessage(socket, "Invalid change command format");
            return;
        }

        String name = parts[1];
        String property = parts[2];
        String value = parts[4];

        CRUD.Choice fieldToUpdate;
        try {
            fieldToUpdate = CRUD.Choice.valueOf(property.toUpperCase());
        } catch (IllegalArgumentException e) {
            sendMessage(socket, "Invalid property");
            return;
        }

        lock.lock();
        try {
            CRUD.Query query = CRUD.Query.builder().sign("=").value(value).build();
            int success = CRUD.update(sessionFactory, fieldToUpdate, query, name);
            sendMessage(socket, success > 0 ? String.format("%d changing", success) : "Changing hasn't been done");
            for (Socket connection : connections) {
                if (connection != socket)
                    sendMessage(connection, "The car has been changed");
            }
        } finally {
            lock.unlock();
        }

    }

    private void processDeleteOperation(String[] parts) throws IOException {
        if (parts.length < 2) {
            sendMessage(socket, "Invalid delete command format");
            return;
        }

        String propName = parts[1];
        String sign = parts[2];
        String value = parts[3];
        CRUD.Choice fieldToDelete = CRUD.Choice.valueOf(propName.toUpperCase());
        lock.lock();
        try {
            CRUD.Query query = CRUD.Query.builder().sign(sign).value(value).build();
            int success = CRUD.delete(sessionFactory, fieldToDelete, query);
            sendMessage(socket, success > 0 ? String.format("%d deleting", success) : "Deleting hasn't been done");
            for (Socket connection : connections) {
                if (connection != socket)
                    sendMessage(connection, "The car has been deleted");
            }
        } finally {
            lock.unlock();
        }
    }

    @Override
    public void run() {
        try {
            while (true) {
                String message = receiveMessage(socket);
                if (message == null || message.isEmpty()) {
                    System.out.println("Client " + (i + 1) + " disconnected");
                    lock.lock();
                    try {
                        connections.remove(socket);
                    } finally {
                        lock.unlock();
                    }
                    break;
                }

                System.out.println("Message from client " + (i + 1) + ": " + message);
                String[] parts = message.split(" ");
                String operation = parts[0].toLowerCase();

                switch (operation) {
                    case "select":
                        processSelectOperation(parts);
                        break;
                    case "add":
                        processAddOperation(parts);
                        break;
                    case "change":
                        processChangeOperation(parts);
                        break;
                    case "del":
                        processDeleteOperation(parts);
                        break;
                    default:
                        sendMessage(socket, "Unknown operation");
                        break;
                }
            }
        } catch (IOException e) {
            System.out.println("Error with client " + (i + 1) + ": " + e.getMessage());
            lock.lock();
            try {
                connections.remove(socket);
            } finally {
                lock.unlock();
            }
        } finally {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
