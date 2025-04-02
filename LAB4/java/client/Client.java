package client;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.Scanner;

public class Client {
    private static boolean processServerData(SocketChannel socket) throws IOException {
        ByteBuffer buffer = ByteBuffer.allocate(1024);
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
            System.out.println(message);
        }
        return true;
    }

    public static void main(String[] args) throws InterruptedException {
        try {
            SocketChannel socket = SocketChannel.open();
            socket.connect(new InetSocketAddress("127.0.0.1", 8080));
            socket.write(ByteBuffer.wrap("Client has connected".getBytes()));
            processServerData(socket);
            Scanner sc = new Scanner(System.in);
            while (processServerData(socket)) {
                String message = sc.nextLine();
                socket.write(ByteBuffer.wrap(message.getBytes()));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println("Server stopped");
        Thread.sleep(10000);
    }
}