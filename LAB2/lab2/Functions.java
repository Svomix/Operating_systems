package lab2;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.nio.file.attribute.FileTime;
import java.nio.file.attribute.PosixFilePermission;
import java.nio.file.attribute.PosixFilePermissions;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Functions {
    static private String IntToParams(String oct) {
        String res = "";
        for (int i = 1; i < oct.length(); i++) {
            int num = Character.getNumericValue(oct.charAt(i));
            switch (num) {
                case 7: {
                    res += "rwx";
                    break;
                }
                case 6: {
                    res += "rw-";
                    break;
                }
                case 5: {
                    res += "r-x";
                    break;
                }
                case 4: {
                    res += "r--";
                    break;
                }
                case 3: {
                    res += "-wx";
                    break;
                }
                case 2: {
                    res += "-w-";
                    break;
                }
                case 1: {
                    res += "--x";
                    break;
                }
                default: {
                    System.out.println("Error in getting params " + num);
                    return "ERROR";
                }
            }
        }
        return res;
    }
    static private String GetFilename(String filename)
    {
        String res = "";
        for (int i = filename.length() - 1; i > 0  ; --i) {
            if(filename.charAt(i) == '/') {
                return filename.substring(i, filename.length());
            }
        }
        return "Error";
    }
    static boolean copy(String src, String dest) {
        Path source = Paths.get(src);
        if(src.equals(dest)) {
            System.out.println("Source and destination are the same");
            return false;
        }
        if (!Files.exists(source) || !Files.isRegularFile(source)) {
            System.out.println("Source file not found or is not a regular file");
            return false;
        }
        Path destination = Paths.get(dest);
        if (!Files.exists(destination) || !Files.isRegularFile(destination)) {
            System.out.println("Destination file not found or is not a regular file");
            return false;
        }
        try {
            Files.copy(source, destination, StandardCopyOption.REPLACE_EXISTING);
        } catch (IOException e) {
            System.out.println("Copying error\n" + e.getMessage());
            return false;
        }
        return true;
    }

    static boolean move(String src, String dest) {
        Path source = Paths.get(src);
        if (!Files.exists(source) || !Files.isRegularFile(source)) {
            System.out.println("Source file not found or is not a regular file");
            return false;
        }

        Path destinationFolder = Paths.get(dest);
        if (!Files.exists(destinationFolder) || !Files.isDirectory(destinationFolder)) {
            System.out.println("Destination file not found or is not a directory");
            return false;
        }
        Path destination = Paths.get(dest += GetFilename(src));
        try {
            Files.move(source, destination);
        } catch (IOException e) {
            System.out.println("Moving error \n" + e.getMessage());
            return false;
        }
        return true;
    }

    static boolean info(String src) {
        Path source = Paths.get(src);
        if (!Files.exists(source) || !Files.isRegularFile(source)) {
            System.out.println("Source file not found or is not a regular file");
            return false;
        }
        try {
            System.out.println("The size of the file is " + Files.size(source));
        } catch (IOException e) {
            System.out.println("Error getting size" + e.getMessage());
            return false;
        }
        try {
            FileTime ft = Files.getLastModifiedTime(source);
            LocalDateTime ldt = LocalDateTime.ofInstant(ft.toInstant(), ZoneId.systemDefault());
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy HH:mm:ss");
            String date = ldt.format(formatter);
            System.out.println("The change time is " + date);
        } catch (IOException e) {
            System.out.println("Error getting last modified time" + e.getMessage());
            return false;
        }
        try {
            System.out.println("The access right is " + Files.getPosixFilePermissions(source));
        } catch (IOException e) {
            System.out.println("Error getting permissions" + e.getMessage());
            return false;
        }
        return true;
    }

    static boolean chr(String src, String rights) {
        Path source = Paths.get(src);
        if (!Files.exists(source) || !Files.isRegularFile(source)) {
            System.out.println("Source file not found or is not a regular file");
            return false;
        }
        Pattern reg_octal = Pattern.compile("0([0-7])([0-7])([0-7])");
        Pattern reg_params = Pattern.compile("(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)");
        Matcher matcher_oct = reg_octal.matcher(rights);
        Matcher matcher_params = reg_params.matcher(rights);
        if (matcher_params.matches()) {
            Set<PosixFilePermission> permissions = PosixFilePermissions.fromString(rights);
            try {
                Files.setPosixFilePermissions(source, permissions);
            } catch (IOException e) {
                System.out.println("Error setting permissions" + e.getMessage());
                return false;
            }
            return true;
        }
        if (matcher_oct.matches()) {
            Set<PosixFilePermission> permissions = PosixFilePermissions.fromString(Functions.IntToParams(rights));
            try {
                Files.setPosixFilePermissions(source, permissions);
            } catch (IOException e) {
                System.out.println("Error setting permissions" + e.getMessage());
                return false;
            }
            return true;
        }
        return false;
    }
}