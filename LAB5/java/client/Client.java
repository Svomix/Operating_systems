package lab5;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.regex.Pattern;

public class Client {
    private static final int BUFSIZE = 4096;
    private static final int LEN_MSG = 21;
    private static volatile boolean running = true;

    public static void main(String[] args) {
        try {
            Socket socket = new Socket("127.0.0.1", 8080);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            infoMessage();

            ExecutorService executor = Executors.newSingleThreadExecutor();
            executor.submit(() -> messageReceiver(socket));

            Scanner scanner = new Scanner(System.in);
            String choice = operationChoice(scanner);

            while (!choice.equals("quit") && running) {
                out.println(choice);

                String[] parts = choice.split(" ");
                String oper = parts[0];

                if (oper.equals("select")) {
                    String response = in.readLine();
                    int nCars = Integer.parseInt(response);
                    out.println("Data has been accept");

                    if (nCars == 0) {
                        System.out.println("There aren't any car");
                    }

                    for (int i = 0; i < nCars; i++) {
                        response = in.readLine();
                        out.println("Data has been accept");
                        System.out.println("\n" + response);
                    }
                    System.out.println();
                } else if (oper.equals("add") || oper.equals("change") || oper.equals("del")) {
                    String response = in.readLine();
                    System.out.println(response);
                }
                choice = operationChoice(scanner);
            }

            running = false;
            executor.shutdown();
            try {
                executor.awaitTermination(1, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            socket.close();
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    private static void infoMessage() {
        System.out.println("You are allowed to choose cars by its properties and change its properties");
        System.out.println("Here are the properties of cars");
        System.out.println("name\nmax_speed\ntank_capacity\nprice\nengine_power");
        System.out.println("Every token must be divided by space");
        System.out.println("---------------------------------------------------------------------");
        System.out.println("To get information about the car you should enter 'select' and the name of the car");
        System.out.println("If you need to get some cars by it property you should enter the name of the property, " +
                "the sign(<,<=,=,>,>=) and the value of the property");
        System.out.println("For the name is allowed only =");
        System.out.println("---------------------------------------------------------------------");
        System.out.println("To change information about the car you should enter 'change', the name of the car, " +
                "changeable property, '=' and the new value of the property");
        System.out.println("--------------------------------------------------------------------");
        System.out.println("To add new car you should enter 'add' and all properties of the car");
        System.out.println("--------------------------------------------------------------------");
        System.out.println("To delete car you should enter 'del' 'name' and the name of the car");
    }

    private static String operationChoice(Scanner scanner) throws IOException {
        Pattern patternSelect = Pattern.compile(
                "^select ((name = \\w+)|(max_speed|tank_capacity|engine_power|price) (<|<=|=|>|>=) \\d+$)");
        Pattern patternAdd = Pattern.compile(
                "^add name = \\w+ max_speed = \\d+ tank_capacity = \\d+ price = \\d+ engine_power = \\d+$");
        Pattern patternChange = Pattern.compile(
                "^change \\w+ ((name = \\w+)|(max_speed = \\d+)|(tank_capacity = \\d+)|(engine_power = \\d+)|(price = \\d+))$");
        Pattern patternDelete = Pattern.compile("^del ((name = \\w+)|(max_speed (<|<=|=|>|>=) \\d+)|(tank_capacity (<|<=|=|>|>=) \\d+)|(engine_power (<|<=|=|>|>=) \\d+)|(price (<|<=|=|>|>=) \\d+))$");

        while (running) {
            System.out.println("Enter the operation");
            String choice = scanner.nextLine();
            if (choice.equals("quit")) {
                running = false;
                return choice;
            }

            if (patternSelect.matcher(choice).matches() ||
                    patternAdd.matcher(choice).matches() ||
                    patternChange.matcher(choice).matches() ||
                    patternDelete.matcher(choice).matches()) {
                return choice;
            }
            System.out.println("Please enter a valid choice");

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return "quit";
            }
        }
        return "quit";
    }

    private static void messageReceiver(Socket socket) {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            while (running) {
                if (socket.getInputStream().available() > 0) {
                    String message = in.readLine();
                    if (message == null) {
                        running = false;
                        break;
                    }
                    System.out.println(message);
                }

                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        } catch (IOException e) {
            System.err.println("Error in message receiver: " + e.getMessage());
            running = false;
        }
    }
}