package lab4.lab4client;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.stage.Stage;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.charset.StandardCharsets;

public class ClientController {
    @FXML
    private Label Enter;
    private SocketChannel socket;
    private int id;

    public void setSocket(SocketChannel socket) {
        this.socket = socket;
    }

    @FXML
    protected void onHelloButtonClick() throws IOException {
        Enter.setText("Добро пожаловать в программу");
        try {
            if (socket != null && socket.isConnected()) {
                ByteBuffer inputBuffer = ByteBuffer.allocate(1024);
                int bytesRead = socket.read(inputBuffer);
                if (bytesRead > 0) {
                    inputBuffer.flip();
                    String idStr = StandardCharsets.UTF_8.decode(inputBuffer).toString().trim();
                    try {
                        id = Integer.parseInt(idStr);
                        System.out.println("Client ID: " + id);
                        Enter.setText("Ваш ID: " + id);
                        socket.write(ByteBuffer.wrap("ID accepted".getBytes()));
                        try {
                            FXMLLoader loader = new FXMLLoader(getClass().getResource("second-view.fxml"));
                            Parent root = loader.load();
                            SecondController secondController = loader.getController();
                            secondController.setClientId(id);
                            secondController.setSocket(socket);
                            Stage stage = (Stage) Enter.getScene().getWindow();
                            stage.setScene(new Scene(root, 800, 600));
                            stage.setTitle(String.format("Client %d", id));
                        } catch (IOException e) {
                            e.printStackTrace();
                            Enter.setText("Ошибка загрузки страницы");
                        }
                    } catch (NumberFormatException e) {
                        System.err.println("Неверный формат ID: " + idStr);
                        Enter.setText("Ошибка: неверный формат ID");
                    }
                } else if (bytesRead == -1) {
                    System.out.println("Соединение закрыто сервером");
                    Enter.setText("Соединение закрыто");
                }
            }

        } catch (
                IOException e) {
            e.printStackTrace();
            Enter.setText("Ошибка соединения");
        }
    }
}