package lab5;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;


class Server {
    private static int input() {
        int res;
        Scanner sc = new Scanner(System.in);
        System.out.println("Enter number(1-50) of clients:");
        while (!sc.hasNextInt() || (res = sc.nextInt()) < 1 || res > 51) {
            sc.nextLine();
            System.out.println("Wrong input. Try again.");
            System.out.println("Please enter a number between 0 and 50");
        }
        return res;
    }

    public static void Run() throws IOException {
        String classpath = "E:\\labs\\lab5_client\\client\\target\\classes";

        Process process = new ProcessBuilder(
                "cmd", "/c", "start",
                "java",
                "-cp",
                classpath,
                "lab5.Client"
        ).start();
    }

    static void backend(int numConnections, List<Socket> clients) {
        List<HandlerClient> clientThreads = new ArrayList<HandlerClient>();
        for (int i = 0; i < numConnections; ++i) {
            HandlerClient h = new HandlerClient(i, clients.get(i), clients);
            clientThreads.add(h);
            h.start();
        }
        try {
            for (HandlerClient h : clientThreads) {
                h.join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("All have been disconnected.");
    }

    public static void main(String[] args) throws IOException {
        List<Socket> connections = new ArrayList<Socket>();
        try (ServerSocket serverSocket = new ServerSocket(8080)) {
            System.out.println("Listening on port 8080");
            int num_con = input();
            for (int i = 0; i < num_con; ++i) {
                Run();
                Socket sockClient = serverSocket.accept();
                connections.add(sockClient);
                System.out.println("Client " + (i + 1) + " connected");
            }
            backend(num_con, connections);
            for (Socket sockClient : connections)
                sockClient.close();
        }
    }
}