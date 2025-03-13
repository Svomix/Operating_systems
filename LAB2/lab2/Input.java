package lab2;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Input {
    Scanner sc = new Scanner(System.in);

    String choice(String msg) {
        System.out.println(msg);
        String res = sc.nextLine();
        while (res.compareTo("cp") != 0 && res.compareTo("mv") != 0 && res.compareTo("info") != 0 && res.compareTo("chr") != 0 && res.compareTo("exit") != 0 && res.compareTo("--help") != 0) {
            System.out.println("The wrong choice");
            System.out.println("Enter the correct choice");
            res = sc.nextLine();
        }
        return res;
    }

    String file(String msg) {
        System.out.println(msg);
        String res = sc.nextLine();
        return res;
    }

    String accessRights(String msg) {
        System.out.println(msg);
        String res = sc.nextLine();
        Pattern reg_octal = Pattern.compile("0([0-7])([0-7])([0-7])");
        Pattern reg_params = Pattern.compile("(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)");
        Matcher m1 = reg_octal.matcher(res);
        Matcher m2 = reg_params.matcher(res);
        while (!m1.matches() && !m2.matches()) {
            System.out.println("Wrong mask");
            System.out.println("Enter the correct mask");
            res = sc.nextLine();
            m1 = reg_octal.matcher(res);
            m2 = reg_params.matcher(res);
        }
        return res;
    }
}
