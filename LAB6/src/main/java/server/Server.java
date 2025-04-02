package server;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.Set;

public class Server {
    public static void Run() throws IOException {
        ProcessBuilder pb = new ProcessBuilder(
                "java",
                "-cp", "E:\\Projects\\lab4Client\\target\\classes",
                "--module-path", "E:\\Projects\\lab4_serv\\javafx-sdk-21.0.6\\lib",
                "--add-modules", "javafx.controls,javafx.fxml",
                "lab4.lab4client.Client"
        );
        pb.inheritIO();
        pb.start();
    }

    private static boolean processClientData(SocketChannel socket, int index) throws IOException {
        ByteBuffer buffer = ByteBuffer.allocate(1024);
        try {
            int bytesRead = socket.read(buffer);
            if (bytesRead == -1) {
                socket.close();
                return false;
            }
            if (bytesRead > 0) {
                buffer.flip();
                byte[] data = new byte[buffer.remaining()];
                buffer.get(data);
                String message = new String(data);
                System.out.println("Messages from client " + (index + 1) + ':');
                System.out.println(message);
            }
            return true;
        } catch (SocketException e) {
            return false;
        }
    }

    private static boolean allSocketsClosed(ArrayList<SocketChannel> connections) {
        for (SocketChannel socket : connections) {
            if (socket.isOpen()) {
                return false;
            }
        }
        System.out.println("All sockets closed");
        return true;
    }

    public static void MonitorClient(ArrayList<SocketChannel> connections) throws IOException {
        Selector selector = Selector.open();
        for (int i = 0; i < connections.size(); ++i) {
            SocketChannel socketChannel = connections.get(i);
            if (!processClientData(socketChannel, i)) {
                socketChannel.close();
                System.out.println("Client " + (i + 1) + " has disconnected");
                continue;
            }
            socketChannel.configureBlocking(false);
            socketChannel.write(ByteBuffer.wrap("Are you in connect?".getBytes()));
            socketChannel.register(selector, SelectionKey.OP_READ);
        }

        while (!allSocketsClosed(connections)) {
            int readyChannels = selector.select(10);
            if (readyChannels == 0) continue;
            Set<SelectionKey> selectedKeys = selector.selectedKeys();
            for (SelectionKey key : selectedKeys) {
                if (key.isReadable()) {
                    SocketChannel channel = (SocketChannel) key.channel();
                    int indexClient = connections.indexOf(channel);
                    if (!processClientData(channel, indexClient)) {
                        channel.close();
                        System.out.println("Client " + (indexClient + 1) + " has disconnected");
                    } else {
                        SocketChannel socket = (SocketChannel) key.channel();
                        socket.write(ByteBuffer.wrap("Are yot in connect?".getBytes()));
                    }
                }
            }
            selector.selectedKeys().clear();
        }
        selector.close();
    }

    public static void main(String[] args) throws IOException {
        ArrayList<SocketChannel> connections = new ArrayList<>();
        ServerSocketChannel serverSocket = ServerSocketChannel.open();
        serverSocket.socket().bind(new InetSocketAddress("127.0.0.1", 8080));
        int numClients = new Input().num_clients();
        for (int i = 0; i < numClients; ++i) {
            Run();
            SocketChannel clientSocket = serverSocket.accept();
            connections.add(clientSocket);
            System.out.println("Client " + (i + 1) + " connected");
            processClientData(clientSocket, i);
            clientSocket.write(ByteBuffer.wrap(String.valueOf(i + 1).getBytes()));
        }
        MonitorClient(connections);
        serverSocket.close();
    }
}