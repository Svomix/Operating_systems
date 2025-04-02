package lab4.lab4client;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

public class Client extends Application {
    private static SocketChannel socket;
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(Client.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 800, 800);
        socket.write(ByteBuffer.wrap("Client has connected".getBytes()));
        ClientController controller = fxmlLoader.getController();
        controller.setSocket(socket);
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) throws IOException {
        socket = SocketChannel.open();
        socket.connect(new InetSocketAddress("127.0.0.1", 8080));
        launch();
    }
}