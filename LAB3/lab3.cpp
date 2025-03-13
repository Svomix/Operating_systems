#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <cstring>
#include <unistd.h>

int pipe_in[2];
int pipe_out[2];
pid_t pid;

void help() {
    std::cout << "Программа вычисляет произведение двух матриц\n";
    std::cout
            << "Запуск с -f input_file output_file - читать матрицы из input_file и записать результат в output_file\n";
    std::cout << "Пример формата матрицы: [[1,2,3],[4,5,6],[7,8,9]]\n";
}

void goodbye() {
    std::cout << "До свидания\n";
}

std::vector<std::vector<double>> parse(const std::string &line) {
    std::vector<std::vector<double>> result;
    std::vector<double> temp;
    std::string num;
    for (int i = 1; i < line.size() - 1; ++i) {
        if (line[i] == ']') {
            temp.push_back(std::stod(num));
            num.clear();
            result.emplace_back(temp);
            temp.clear();
        } else if (line[i] != '[' && line[i] != ',') {
            num += line[i];
        } else if (line[i] == ',' && !num.empty()) {
            temp.push_back(std::stod(num));
            num.clear();
        }
    }
    return result;
}

void read_matrix(const std::string &filename, std::vector<std::vector<double>> &matrix1,
                 std::vector<std::vector<double>> &matrix2) {
    const std::regex pattern(
            R"(^\[\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\](\s*,\s*\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\])*\]$)");

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл " << filename << std::endl;
        return;
    }
    std::string line;
    std::getline(file, line);
    if (!std::regex_match(line, pattern)) {
        std::cerr << "Строка в файле не соответствует формату матрицы:\n" << line << std::endl;
        return;
    }
    matrix1 = parse(line);
    if (!matrix1.empty()) {
        size_t cols = matrix1[0].size();
        if (!std::all_of(matrix1.begin(), matrix1.end(), [&](const std::vector<double> &v) {
            return v.size() == cols;
        })) {
            std::cerr << "Строки матрицы должны иметь одинаковое количество столбцов\n";
            return;
        }
    }
    std::getline(file, line);
    if (!std::regex_match(line, pattern)) {
        std::cerr << "Строка в файле не соответствует формату матрицы:\n" << line << std::endl;
        return;
    }
    matrix2 = parse(line);
    if (!matrix2.empty()) {
        size_t cols = matrix2[0].size();
        if (!std::all_of(matrix2.begin(), matrix2.end(), [&](const std::vector<double> &v) {
            return v.size() == cols;
        })) {
            std::cerr << "Строки матрицы должны иметь одинаковое количество столбцов\n";
            return;
        }
    }
    file.close();
}

std::vector<std::vector<double>> read_matrix(const char *msg) {
    const std::regex pattern(
            R"(^\[\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\](\s*,\s*\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\])*\]$)");
    std::cout << msg;
    std::string matrix;
    std::string num;
    std::vector<double> temp;
    std::vector<std::vector<double>> result;
    while (!(std::cin >> matrix) || !std::regex_match(matrix, pattern)) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cerr << "Матрица введена неправильно\n";
    }
    for (int i = 1; i < matrix.size() - 1; ++i) {
        if (matrix[i] == ']') {
            temp.push_back(std::stod(num));
            num.clear();
            result.emplace_back(temp);
            temp.clear();
        } else if (matrix[i] != '[' && matrix[i] != ',') {
            num += matrix[i];
        } else if (matrix[i] == ',' && !num.empty()) {
            temp.push_back(std::stod(num));
            num.clear();
        }
    }
    if (!(std::all_of(result.begin(), result.end(), [&](const std::vector<double> &v) {
        return v.size() == result[0].size();
    }))) {
        std::cerr << "Cтроки матрицы должны иметь одинаковое количество столбцов\n";
        return {};
    }
    return result;
}

bool check_dim(const std::vector<std::vector<double>> &matrix1, const std::vector<std::vector<double>> &matrix2) {
    return matrix1.empty() || matrix2.empty() || matrix1[0].size() != matrix2.size();
}

std::vector<std::vector<double>> mult(const std::vector<std::vector<double>> &matrix1,
                                      const std::vector<std::vector<double>> &matrix2) {
    std::vector<std::vector<double>> result(matrix1.size(), std::vector<double>(matrix2[0].size(), 0));
    std::vector<double> temp;
    for (size_t i = 0; i < matrix1.size(); ++i) {
        for (size_t j = 0; j < matrix2[0].size(); ++j) {
            for (size_t k = 0; k < matrix1[0].size(); ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return result;
}

void fronted(const std::string &input_file = "", const std::string &output_file = "") {
    std::vector<std::vector<double>> matrix1, matrix2;
    if (!input_file.empty()) {
        read_matrix(input_file,matrix1,matrix2);
    } else {
        matrix1 = read_matrix("Введите первую матрицу:\n");
        matrix2 = read_matrix("Введите вторую матрицу:\n");
    }
    while (check_dim(matrix1, matrix2)) {
        std::cerr << "Недопустимый размер матриц\n";
        matrix1.clear();
        matrix2.clear();
        if (!input_file.empty()) {
            return;
        } else {
            while (matrix1.empty())
                matrix1 = read_matrix("Введите первую матрицу:\n");
            while (matrix2.empty())
                matrix2 = read_matrix("Введите вторую матрицу:\n");
        }
    }
    size_t row1 = matrix1.size();
    size_t col1 = matrix1[0].size();
    size_t row2 = matrix2.size();
    size_t col2 = matrix2[0].size();
    write(pipe_in[1], &row1, sizeof(row1));
    write(pipe_in[1], &col1, sizeof(col1));
    write(pipe_in[1], &row2, sizeof(row2));
    write(pipe_in[1], &col2, sizeof(col2));
    for (const auto &r: matrix1)
        write(pipe_in[1], r.data(), sizeof(r[0]) * r.size());
    for (const auto &r: matrix2)
        write(pipe_in[1], r.data(), sizeof(r[0]) * r.size());

    std::vector<std::vector<double>> result(row1, std::vector<double>(col2));
    for (auto &r: result)
        read(pipe_out[0], r.data(), r.size() * sizeof(double));

    if (!output_file.empty()) {
        std::ofstream out(output_file);
        if (!out.is_open()) {
            std::cerr << "Не удалось открыть файл для записи: " << output_file << std::endl;
            exit(1);
        }
        for (const auto &r: result) {
            out << '[';
            for (size_t i = 0; i < r.size(); ++i) {
                out << r[i];
                if (i < r.size() - 1) out << ',';
            }
            out << "]\n";
        }
        out.close();
    } else {
        for (const auto &r: result) {
            std::cout << '[';
            for (size_t i = 0; i < r.size(); ++i) {
                std::cout << r[i];
                if (i < r.size() - 1) std::cout << ',';
            }
            std::cout << "]\n";
        }
    }

    goodbye();
    exit(0);
}

void backend() {
    size_t row1;
    size_t col1;
    size_t row2;
    size_t col2;
    read(pipe_in[0], &row1, sizeof(row1));
    read(pipe_in[0], &col1, sizeof(col1));
    read(pipe_in[0], &row2, sizeof(row2));
    read(pipe_in[0], &col2, sizeof(col2));
    std::vector<std::vector<double>> matrix1(row1, std::vector<double>(col1));
    std::vector<std::vector<double>> matrix2(row2, std::vector<double>(col2));
    for (auto &r: matrix1)
        read(pipe_in[0], r.data(), r.size() * sizeof(double));
    for (auto &r: matrix2)
        read(pipe_in[0], r.data(), r.size() * sizeof(double));
    std::vector<std::vector<double>> result = mult(matrix1, matrix2);
    for (const auto &r: result)
        write(pipe_out[1], r.data(), r.size() * sizeof(double));
}

int main(int argc, char const *argv[]) {
    setlocale(LC_ALL, "RUS");

    if (argc == 2 && !strcmp(argv[1], "--help")) {
        help();
        exit(0);
    }

    if (argc == 4 && !strcmp(argv[1], "-f")) {
        std::string input_file = argv[2];
        std::string output_file = argv[3];
        pipe(pipe_out);
        pipe(pipe_in);
        pid = fork();
        if (pid < 0) {
            std::cerr << "Ошибка, не удалось создать процесс\n";
            exit(1);
        } else if (pid > 0) {
            fronted(input_file, output_file);
        } else {
            backend();
        }
        for (int i = 0; i < 2; ++i) {
            close(pipe_in[i]);
            close(pipe_out[i]);
        }
    } else if (argc == 1) {
        pipe(pipe_out);
        pipe(pipe_in);
        pid = fork();
        if (pid < 0) {
            std::cerr << "Ошибка, не удалось создать процесс\n";
            exit(1);
        } else if (pid > 0) {
            fronted();
        } else {
            backend();
        }
        for (int i = 0; i < 2; ++i) {
            close(pipe_in[i]);
            close(pipe_out[i]);
        }
    } else {
        std::cerr << "Неверные аргументы. Используйте --help для справки.\n";
        exit(1);
    }
    return 0;
}