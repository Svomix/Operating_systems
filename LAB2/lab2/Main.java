package lab2;

public class Main {
    public static void main(String[] args) {
        Messages.Greeting();
        Input input = new Input();
        String choice;
        do {
            choice = input.choice("Enter your choice");
            switch (choice) {
                case "cp": {
                    String source, destination;
                    source = input.file("Enter the source filename");
                    destination = input.file("Enter the destination filename");
                    if (Functions.copy(source, destination))
                        Messages.Success();
                    break;
                }
                case "mv": {
                    String source, destination;
                    source = input.file("Enter the source filename");
                    destination = input.file("Enter the destination filename");
                    if (Functions.move(source, destination))
                        Messages.Success();
                    break;
                }
                case "info": {
                    String source;
                    source = input.file("Enter the filename");
                    if (Functions.info(source))
                        Messages.Success();
                    break;
                }
                case "chr": {
                    String source, rights;
                    source = input.file("Enter the filename");
                    rights = input.accessRights("Enter the mask");
                    if (Functions.chr(source, rights))
                        Messages.Success();
                    break;
                }
                case "--help":
                    Messages.Help();
                    break;
            }
        }while (!choice.equals("exit"));
        Messages.Goodbye();
    }
}