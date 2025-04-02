package server;

import java.util.Scanner;

public class Input {
    Scanner sc = new Scanner(System.in);
    int num_clients()
    {
        int res;
        System.out.println("Enter number(3-50) of clients:");
        while(!sc.hasNextInt() || (res = sc.nextInt()) < 3 || res > 51)
        {
            sc.nextLine();
            System.out.println("Wrong input. Try again.");
            System.out.println("Please enter a number between 3 and 50");
        }
        return res;
    }
}
