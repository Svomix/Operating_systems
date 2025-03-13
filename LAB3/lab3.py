import os
import sys
import re
import struct


def help():
    print("Программа вычисляет произведение двух матриц")
    print("Запуск с -f input_file output_file - читать матрицы из input_file и записать результат в output_file")
    print("Пример формата матрицы: [[1,2,3],[4,5,6],[7,8,9]]")


def goodbye():
    print("До свидания")


def parse(line):
    result = []
    temp = []
    num = ""
    for i in range(1, len(line) - 1):
        if line[i] == ']':
            temp.append(float(num))
            num = ""
            result.append(temp)
            temp = []
        elif line[i] != '[' and line[i] != ',':
            num += line[i]
        elif line[i] == ',' and num:
            temp.append(float(num))
            num = ""
    return result


def read_matrix(filename):
    pattern = re.compile(
        r"^\[\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\](\s*,\s*\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\])*\]$")
    matrix1, matrix2 = [], []
    try:
        with open(filename, 'r') as file:
            line = file.readline().strip()
            if not pattern.match(line):
                print(f"Строка в файле не соответствует формату матрицы:\n{line}")
                return matrix1, matrix2
            matrix1 = parse(line)
            if matrix1:
                cols = len(matrix1[0])
                if not all(len(row) == cols for row in matrix1):
                    print("Строки матрицы должны иметь одинаковое количество столбцов")
                    return [], []
            line = file.readline().strip()
            if not pattern.match(line):
                print(f"Строка в файле не соответствует формату матрицы:\n{line}")
                return matrix1, []
            matrix2 = parse(line)
            if matrix2:
                cols = len(matrix2[0])
                if not all(len(row) == cols for row in matrix2):
                    print("Строки матрицы должны иметь одинаковое количество столбцов")
                    return matrix1, []
    except IOError:
        print(f"Не удалось открыть файл {filename}")
    return matrix1, matrix2


def input_matrix(msg):
    pattern = re.compile(
        r"^\[\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\](\s*,\s*\[\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)*\])*\]$")
    while True:
        matrix = input(msg)
        if pattern.match(matrix):
            result = parse(matrix)
            if all(len(row) == len(result[0]) for row in result):
                return result
            else:
                print("Строки матрицы должны иметь одинаковое количество столбцов")
        else:
            print("Матрица введена неправильно")


def check_dim(matrix1, matrix2):
    return not matrix1 or not matrix2 or len(matrix1[0]) != len(matrix2)


def mult(matrix1, matrix2):
    rows1 = len(matrix1)
    cols1 = len(matrix1[0])
    cols2 = len(matrix2[0])

    result = [[0 for _ in range(cols2)] for _ in range(rows1)]

    for i in range(rows1):
        for j in range(cols2):
            for k in range(cols1):
                result[i][j] += matrix1[i][k] * matrix2[k][j]
    return result


def backend(pipe_in, pipe_out):
    row1 = struct.unpack('i', os.read(pipe_in[0], 4))[0]
    col1 = struct.unpack('i', os.read(pipe_in[0], 4))[0]
    row2 = struct.unpack('i', os.read(pipe_in[0], 4))[0]
    col2 = struct.unpack('i', os.read(pipe_in[0], 4))[0]

    matrix1 = []
    for _ in range(row1):
        row = []
        for _ in range(col1):
            value = struct.unpack('d', os.read(pipe_in[0], 8))[0]
            row.append(value)
        matrix1.append(row)

    matrix2 = []
    for _ in range(row2):
        row = []
        for _ in range(col2):
            value = struct.unpack('d', os.read(pipe_in[0], 8))[0]
            row.append(value)
        matrix2.append(row)

    result = mult(matrix1, matrix2)

    for row in result:
        for value in row:
            os.write(pipe_out[1], struct.pack('d', value))


def frontend(pipe_in, pipe_out, input_file="", output_file=""):
    if input_file:
        matrix1, matrix2 = read_matrix(input_file)
    else:
        matrix1 = input_matrix("Введите первую матрицу:\n")
        matrix2 = input_matrix("Введите вторую матрицу:\n")
    while check_dim(matrix1, matrix2):
        print("Недопустимый размер матриц")
        if input_file:
            return
        else:
            matrix1 = input_matrix("Введите первую матрицу:\n")
            matrix2 = input_matrix("Введите вторую матрицу:\n")

    os.write(pipe_in[1], struct.pack('i', len(matrix1)))
    os.write(pipe_in[1], struct.pack('i', len(matrix1[0])))
    os.write(pipe_in[1], struct.pack('i', len(matrix2)))
    os.write(pipe_in[1], struct.pack('i', len(matrix2[0])))

    for row in matrix1:
        for value in row:
            os.write(pipe_in[1], struct.pack('d', value))
    for row in matrix2:
        for value in row:
            os.write(pipe_in[1], struct.pack('d', value))

    result = []
    for _ in range(len(matrix1)):
        row = []
        for _ in range(len(matrix2[0])):
            value = struct.unpack('d', os.read(pipe_out[0], 8))[0]
            row.append(value)
        result.append(row)

    if output_file:
        try:
            with open(output_file, 'w') as out:
                for row in result:
                    out.write('[' + ','.join(map(str, row)) + ']\n')
        except IOError:
            print(f"Не удалось открыть файл для записи: {output_file}")
            sys.exit(1)
    else:
        for row in result:
            print('[' + ','.join(map(str, row)) + ']')
    goodbye()
    sys.exit(0)


if len(sys.argv) == 2 and sys.argv[1] == "--help":
    help()
    sys.exit(0)
elif len(sys.argv) == 4 and sys.argv[1] == "-f":
    input_file = sys.argv[2]
    output_file = sys.argv[3]
elif len(sys.argv) == 1:
    input_file = ""
    output_file = ""
else:
    print("Неверные аргументы. Используйте --help для справки.")
    sys.exit(1)
pipe_in = os.pipe()
pipe_out = os.pipe()
pid = os.fork()
if pid == 0:
    backend(pipe_in, pipe_out)
else:
    frontend(pipe_in, pipe_out, input_file, output_file)
