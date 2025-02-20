#include <iostream>
#include <algorithm>

void hello() {
    std::cout << "Hello, this program converts a number from binary to octal\n";
    std::cout << "Enter a non-negative binary number\n";
}

std::string input() {
    std::string bin_num;
    while (!(std::cin >> bin_num) || bin_num.find_first_not_of("01.") != std::string::npos ||
           std::count(bin_num.begin(), bin_num.end(), '.') > 1 || bin_num[0] == '.' || bin_num[bin_num.length() - 1] ==
           '.') {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "You have not entered a valid value\n";
        std::cout << "Enter a non-negative binary number\n";
    }
    return bin_num;
}

void goodbye() {
    std::cout << "Thank for using the program\nGoodbye!\n";
}

char restart() {
    std::cout << "Do you want to restart?\nEnter Y/n\n";
    char ans;
    std::cin >> ans;
    ans = std::tolower(ans);
    while (ans != 'y' && ans != 'n' || std::cin.peek() != '\n') {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Invalid answer\n";
        std::cout << "Enter Y/n\n";
        std::cin >> ans;
    }
    return ans;
}

char binary_to_oct_num(const std::string& binary) {
    char num = 0;
    num += (binary[0] - '0') * 4 + (binary[1] - '0') * 2 + (binary[2] - '0');
    return num + '0';
}

std::string binary_to_octal_int(std::string binary_int) {
    while (binary_int.length() % 3 != 0) {
        binary_int = '0' + binary_int;
    }
    std::string octal;
    for (size_t i = 0; i < binary_int.length(); i += 3) {
        std::string group = binary_int.substr(i, 3);
        octal += binary_to_oct_num(group);
    }
    octal.erase(0, octal.find_first_not_of('0'));
    return octal;
}

std::string binary_to_octal_frac(std::string binar_fraction) {
    while (binar_fraction.length() % 3 != 0) {
        binar_fraction += '0';
    }
    std::string octalf;
    std::string group;
    for (size_t i = 0; i < binar_fraction.length(); i += 3) {
        group = binar_fraction.substr(i, 3);
        octalf += binary_to_oct_num(group);
    }
    octalf.erase(octalf.find_last_not_of('0') + 1, octalf.length());
    return octalf;
}

void name()
{
    std::cout << "Igor\n";
}

int main() {
    name();
    char ans;
    hello();
    do {
        std::string n = input();
        size_t pos_of_dot = n.find('.');
        if (pos_of_dot == std::string::npos) {
            std::cout << "The result is " << binary_to_octal_int(n) << std::endl;
        } else {
            std::string res = binary_to_octal_int(
                                  n.substr(0, pos_of_dot)) + "." + binary_to_octal_frac(n.substr(pos_of_dot + 1));
            std::cout << "The result is " << res << std::endl;
        }
        ans = restart();
    } while (ans == 'y');
    goodbye();
    return 0;
}
