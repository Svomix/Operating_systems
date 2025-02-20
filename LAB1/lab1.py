def input_binary():
    while True:
        bin_num = input().strip()
        if all(c in '01.' for c in bin_num) and bin_num.count('.') < 2 and not bin_num.startswith(
                '.') and not bin_num.endswith('.'):
            return bin_num
        print("You have not entered a valid value")
        print("Enter a non-negative binary number")

def restart():
    while True:
        ans = input("Do you want to restart?\nEnter Y/n\n").strip().lower()
        if ans in ['y', 'n']:
            return ans
        print("Invalid answer")
        print("Enter Y/n")

def binary_to_oct_num(binary):
    num = (int(binary[0]) * 4) + (int(binary[1]) * 2) + int(binary[2])
    return str(num)

def binary_to_octal_int(binary_int):
    while len(binary_int) % 3 != 0:
        binary_int = '0' + binary_int
    octal = ''
    for i in range(0, len(binary_int), 3):
        group = binary_int[i:i + 3]
        octal += binary_to_oct_num(group)
    octal = octal.lstrip('0')
    return octal if octal else '0'

def binary_to_octal_frac(binary_fraction):
    while len(binary_fraction) % 3 != 0:
        binary_fraction += '0'
    octalf = ''
    for i in range(0, len(binary_fraction), 3):
        group = binary_fraction[i:i + 3]
        octalf += binary_to_oct_num(group)
    octalf = octalf.rstrip('0')
    return octalf if octalf else '0'

print("Hello, this program converts a number from binary to octal")
print("Enter a non-negative binary number")
ans = 'y'
while ans == 'y':
    n = input_binary()
    pos_of_dot = n.find('.')
    if pos_of_dot == -1:
        print("The result is", binary_to_octal_int(n))
    else:
        res = binary_to_octal_int(n[:pos_of_dot]) + '.' + binary_to_octal_frac(n[pos_of_dot + 1:])
        print("The result is", res)
    ans = restart()
    if ans != 'y':
        break
print("Thank you for using the program")
print("Goodbye!")
