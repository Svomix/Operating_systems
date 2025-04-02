package server;

import java.util.Scanner;

public class Input {
    Scanner sc = new Scanner(System.in);
    int num_clients()
    {
        int res;
        System.out.println("Enter number(1-50) of clients:");
        while(!sc.hasNextInt() || (res = sc.nextInt()) < 1 || res > 51)
        {
            sc.nextLine();
            System.out.println("Wrong input. Try again.");
            System.out.println("Please enter a number between 0 and 50");
        }
        return res;
    }
}
