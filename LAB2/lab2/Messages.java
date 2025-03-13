package lab2;

public class Messages {
    static void Greeting() {
        System.out.println("Hello, the program copies, moves, gets information about the file and changes file access rights");
        System.out.println("Enter '--help' for getting instructions");
    }
    static void Goodbye()
    {
        System.out.println("Goodbye");
    }
    static void Help() {
        System.out.println("HELP");
        System.out.println("Enter '--help' for getting instructions");
        System.out.println("For copying files enter cp, the copied file and new file");
        System.out.println("For moving files enter cp, the moved file and new file");
        System.out.println("For getting information about the file enter info and the name of the file");
        System.out.println("For changing rights of the file enter chr and the mask. It must be octal number or type of mask like 'rwxr-xr-x' which consits of 3 sequence of 3 characters each");
        System.out.println("The first param means read rights, the second write rights and the third execute rights. The absence of a right is indicated by '-'");
    }
    static void Success()
    {
        System.out.println("The operation has been done successfully");
    }
}
