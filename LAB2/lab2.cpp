#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <sys/stat.h>
#include <regex>

#define BUFFER_SIZE 1024

void greeting() {
    std::cout << "Hello, the program copies, moves, gets information about the file and changes file access rights\n";
    std::cout << "Enter '--help' for getting instructions\n";
}

void help() {
    std::cout << "HELP\n";
    std::cout << "For copying files enter cp, the copied file and new file\n";
    std::cout << "For moving files enter cp, the moved file and new file\n";
    std::cout << "For getting information about the file enter info and the name of the file\n";
    std::cout <<
            "For changing rights of the file enter chr and the mask. It must be octal number or type of mask like 'rwxr-xr-x' which consits of 3 sequence of 3 characters each.\n";
    std::cout <<
            "The first param means read rights, the second write rights and the third execute rights. The absence of a right is indicated by '-'\n";
    std::cout << "For printing first line of the file enter l\n";
}

std::string input_choice() {
    std::string choice;
    while (std::cin >> choice && choice != "cp" && choice != "mv" && choice != "info" && choice != "chr" && choice !=
           "--help" &&
           choice != "exit" && choice != "l") {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cerr << "You've entered an invalid choice\n";
        std::cout << "Enter '--help' for getting instructions\n";
    }
    return choice;
}

void read_mask(std::string &mask) {
    std::cout <<
            "Enter the rights mask. It must be octal number or type of mask like 'rwxr-xr-x' which consits of 3 sequence of 3 characters each.\n";
    std::cout << "The first param means read rights, the second write rights and the third execute rights\n";
    std::cin >> mask;
}

void error_filename_cp() {
    std::cerr << "The name of the copied file and the name of the new file must be different\n";
    std::cout << "Please change the file name\n";
}

void error_file_path() {
    std::cerr << "There is no file in the specified path\n";
}

void error_open_file() {
    std::cerr << "The file does not exist\n";
}

void success() {
    std::cout << "The operation has been done successfully\n";
}

bool read_filename(std::string &file_name_new, std::string &file_name_cp) {
    std::cout << "Enter the name of the first path\n";
    std::cin >> file_name_cp;
    if (std::cin.peek() != '\n') {
        error_open_file();
        while (std::cin.get() != '\n');
        return false;
    }
    std::cout << "Enter the name of the second path\n";
    std::cin >> file_name_new;
    if (std::cin.peek() != '\n') {
        error_open_file();
        while (std::cin.get() != '\n');
        return false;
    }
    return true;
}

void goodbye() {
    std::cout << "Goodbye\n";
}

bool read_filename(std::string &filename) {
    std::cout << "Enter the name of the file\n";
    std::cin >> filename;
    if (std::cin.peek() != '\n') {
        error_open_file();
        while (std::cin.get() != '\n');
        return false;
    }
    return true;
}

void error_mask() {
    std::cerr << "You've entered an incorrect mask\n";
}

bool cp(const std::string &file_name_cp, const std::string &file_name_new) {
    if (file_name_cp == file_name_new) {
        error_filename_cp();
        return false;
    }
    struct stat info;
    if (stat(file_name_cp.c_str(), &info) != 0) {
        std::cerr << "Source file does not exist.\n";
        return false;
    }
    if (S_ISDIR(info.st_mode)) {
        std::cerr << "Source path is a directory, not a file.\n";
        return false;
    }
    std::ifstream fin(file_name_cp, std::ios::binary);
    if (!fin.is_open()) {
        std::cerr << "Could not open source file.\n";
        return false;
    }
    std::ofstream fout(file_name_new, std::ios::binary);
    if (!fout.is_open()) {
        std::cerr << "Could not create or open destination file.\n";
        fin.close();
        return false;
    }
    const size_t buf_size = BUFFER_SIZE;
    char *buffer = new char[buf_size];
    while (!fin.eof()) {
        fin.read(buffer, buf_size);
        if (fin.gcount())
            fout.write(buffer, fin.gcount());
    }
    delete[] buffer;
    fin.close();
    fout.close();
    return true;
}

bool mv(const std::string &file_path_old, std::string &file_path_new) {
    std::ifstream fin(file_path_old, std::ios::binary);
    if (!fin.is_open()) {
        error_file_path();
        return false;
    }
    fin.close();
    std::string filename;
    for (size_t i = file_path_old.length() - 1; file_path_old[i] != '/'; --i) {
        filename += file_path_old[i];
    }
    std::reverse(filename.begin(), filename.end());
    file_path_new += filename;
    std::filesystem::rename(file_path_old.c_str(), file_path_new.c_str());
    return true;
}

std::string decimal_to_octal(int decimalNumber) {
    std::string octal;
    while (decimalNumber > 0) {
        int remainder = decimalNumber % 8;
        octal.push_back('0' + remainder);
        decimalNumber /= 8;
    }
    std::reverse(octal.begin(), octal.end());
    return octal;
}

std::string octal_to_string(const int octal) {
    int num;
    int dec = stoi(decimal_to_octal(octal));
    auto dec_str = std::to_string(dec);
    std::string res;
    for (int i = 0; i < dec_str.length(); ++i) {
        num = dec_str[i] - '0';
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
            default:
                std::cerr << "Error\n";
                return "ERROR";
        }
    }
    return res;
}

void print_first(const std::string &filename) {
    std::ifstream fin(filename, std::ios::binary);
    if (!fin.is_open()) {
        std::cerr << "The file doesn't exist\n";
    }
    std::string result;
    std::getline(fin, result);
    std::cout << result << std::endl;
}

bool info(const std::string &filename) {
    std::ifstream fin(filename, std::ios::binary);
    struct stat file_info{};
    if (!fin.is_open()) {
        error_open_file();
        return false;
    }
    if (!stat(filename.c_str(), &file_info)) {
        std::cout << "Access rights of " << filename << " " << octal_to_string(file_info.st_mode & 0777) << "\n";
        std::cout << "Size of " << filename << " is " << file_info.st_size << " bytes\n";
        std::cout << "Modification time of " << filename << " is " << std::asctime(std::localtime(&file_info.st_mtime))
                << "\n";
    }
    fin.close();
    return true;
}

std::string mask_to_octal(const std::string &mask) {
    std::string res = "0";
    int temp = 0;
    for (int i = 0; i < mask.length(); ++i) {
        if (mask[i] == 'r')
            temp += 4;
        else if (mask[i] == 'w')
            temp += 2;
        else if (mask[i] == 'x')
            temp += 1;
        if ((i + 1) % 3 == 0) {
            res += std::to_string(temp);
            temp = 0;
        }
    }
    return res;
}

bool chr(const std::string &filename, const std::string &mask) {
    std::ifstream fin(filename, std::ios::binary);
    const std::regex reg_octal("0([0-7])([0-7])([0-7])");
    const std::regex reg_params("(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)");
    if (!fin.is_open()) {
        error_open_file();
        return false;
    }
    fin.close();
    if (regex_match(mask, reg_octal)) {
        const int mode = std::stoi(mask, nullptr, 8);
        chmod(filename.c_str(), mode);
    } else if (std::regex_match(mask, reg_params)) {
        const int mode = std::stoi(mask_to_octal(mask), nullptr, 8);
        chmod(filename.c_str(), mode);
    } else {
        error_mask();
        return false;
    }
    return true;
}

int main() {
    greeting();
    std::string choice;
    do {
        choice = input_choice();
        if (choice == "--help") {
            help();
        } else if (choice == "cp") {
            std::string file_name_cp;
            std::string file_name_new;
            if (read_filename(file_name_new, file_name_cp) && cp(file_name_cp, file_name_new)) {
                success();
            }
        } else if (choice == "mv") {
            std::string file_path_old;
            std::string file_path_new;
            if (read_filename(file_path_new, file_path_old) && mv(file_path_old, file_path_new)) {
                success();
            }
        } else if (choice == "info") {
            std::string filename;
            if (read_filename(filename) && info(filename)) {
                success();
            }
        } else if (choice == "chr") {
            std::string filename;
            std::string mask;
            read_mask(mask);
            if (read_filename(filename) && chr(filename, mask)) {
                success();
            } else if (choice == "l") {
                std::string filename;
                if (read_filename(filename)) {
                    print_first(filename);
                }
            }
        }
    } while (choice != "exit");
    goodbye();
}
