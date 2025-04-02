package lab4.lab4client;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.charset.StandardCharsets;

public class SecondController {
    @FXML
    private TextArea chatLog;
    @FXML
    private TextArea inputArea;
    @FXML
    private Button sendButton;
    @FXML
    private Button fio;

    @FXML
    private Label label;

    private SocketChannel socket;
    private int clientId;
    private Thread serverListenerThread;
    private volatile boolean running = true;

    public void setSocket(SocketChannel socket) {
        this.socket = socket;
        startServerListener();
    }

    public void setClientId(int id) {
        this.clientId = id;
        Platform.runLater(() -> {
            chatLog.appendText("Вы подключены как Клиент #" + id + "\n");
        });
    }

    @FXML
    private void sendMessageToServer() {
        String message = inputArea.getText().trim();
        if (message.isEmpty() || socket == null) return;

        try {
            ByteBuffer buffer = ByteBuffer.wrap(message.getBytes(StandardCharsets.UTF_8));
            socket.write(buffer);
            chatLog.appendText("Клиент #" + clientId + ": " + message + "\n");
            inputArea.clear();
        } catch (IOException e) {
            chatLog.appendText("Ошибка отправки: " + e.getMessage() + "\n");
        }
    }

    private void startServerListener() {
        serverListenerThread = new Thread(() -> {
            ByteBuffer buffer = ByteBuffer.allocate(1024);
            try {
                while (running && socket != null && socket.isConnected()) {
                    int bytesRead = socket.read(buffer);
                    if (bytesRead > 0) {
                        buffer.flip();
                        String serverMessage = StandardCharsets.UTF_8.decode(buffer).toString().trim();
                        buffer.clear();
                        Platform.runLater(() -> {
                            chatLog.appendText("Сервер: " + serverMessage + "\n");
                        });
                    } else if (bytesRead == -1) {
                        Platform.runLater(() -> {
                            chatLog.appendText("Сервер отключился.\n");
                        });
                        break;
                    }
                }
            } catch (IOException e) {
                Platform.runLater(() -> {
                    chatLog.appendText("Ошибка соединения: " + e.getMessage() + "\n");
                });
            } finally {
                closeSocket();
            }
        });
        serverListenerThread.setDaemon(true);
        serverListenerThread.start();
    }
    @FXML
    private void FIO()
    {
        label.setText("Власов Игорь Александрович");
    }

    public void closeSocket() {
        running = false;
        if (socket != null) {
            try {
                socket.close();
            } catch (IOException e) {
                System.err.println("Ошибка при закрытии сокета: " + e.getMessage());
            }
        }
        if (serverListenerThread != null) {
            serverListenerThread.interrupt();
        }
    }
}