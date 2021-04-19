import json
import sys
from pprint import pprint

import png

HEIGHT = 32
BYTES_PER_LINE = 4

def print_char(img, data, threshold=0x60):
    ''' print width * height at (x, x) from img 2d arr '''
    width, height, x, y = data['width'], data['height'], data['x'], data['y']
    x_advance, x_offset, y_offset = data['xadvance'], data['xoffset'], data['yoffset']

    for _ in range(y_offset):
        print()

    for i in range(y, y + height):
        for iii in range(x_advance):
            sys.stdout.write(" ")
        for ii in range(x, x + width):
            if img[i][ii] <= threshold:
                sys.stdout.write(" ")
            else:
                sys.stdout.write(f"@")
        print()

    for _ in range(32 - y_offset - height):
        print()


def gen_char_array(img, data, threshold=0x60):
    ''' return a byte 2d array of character '''
    width, height, x, y = data['width'], data['height'], data['x'], data['y']
    x_advance, x_offset, y_offset = data['xadvance'], data['xoffset'], data['yoffset']
    arr = []

    # empty lines above
    for _ in range(y_offset):
        line = []
        for _ in range(width):
            line.append(0)
        arr.append(line_to_bytes(line))

    # process each line
    for i in range(y, y + height):
        line = []
        for iii in range(x_advance):
            line.append(0)
        for ii in range(x, x + width):
            if img[i][ii] <= threshold:
                line.append(0)
            else:
                line.append(1)
        arr.append(line_to_bytes(line))

    # empty lines below
    for _ in range(32 - y_offset - height):
        line = []
        for _ in range(width):
            line.append(0)
        arr.append(line_to_bytes(line))
    # start
    if is_first_col_zero(arr, 0):
        for i in range(len(arr)):
            arr[i].remove(0)
            while len(arr[i]) < BYTES_PER_LINE:
                arr[i].append(0)

    # drop the last byte -- this is mostly empty
    for i in range(len(arr)):
        arr[i].pop()

    return arr

def line_to_bytes(line):
    ''' converts an bin array to a byte array '''
    byte_arr = []
    byte_str = ""
    for i in range(len(line)):
        byte_str += str(line[i])
        if i % 8 == 7:
            byte_arr.append(int(byte_str, 2))
            byte_str = ""
        elif i == len(line)-1:
            byte_arr.append(int(byte_str, 2))

    while len(byte_arr) > BYTES_PER_LINE:
        byte_arr.remove(0)

    while len(byte_arr) < BYTES_PER_LINE:
        byte_arr.append(0)

    return byte_arr
    
def is_first_col_zero(byte_arr, col):
    for row in byte_arr:
        if row[col] != 0:
            return False
    return True

def load_font(font_name):
    ''' read font png and metadata -> 2d pixel array, metatadta dict '''

    json_file = open(font_name+".json", "r").read()
    meta_data = json.loads(json_file)

    png_file = png.Reader(file=open(font_name+".PNG", "rb"))
    (total_width, total_height, rows, info) = png_file.asDirect()

    img = []
    for row in rows:
        line = []
        # extract only alpha data 
        for i in range(0, len(row)-3, 4):
            line.append(row[i+3])
        img.append(line)

    return (img, meta_data)

def generate_font_arr(img, meta_data, font_name, threshold):
    char_arr = []
    print('#include "fonts.h"\n')
    print(f"const uint8_t {font_name}_Table [] = {{")

    # print each character
    for char_data in meta_data['symbols']:
        # pprint(char_data)
        print(f"// {chr(char_data['id'])} ({char_data['id']})")
        # print_char(img, char_data, threshold=threshold)

        # print char array data
        char_arr = gen_char_array(img, char_data, threshold=threshold)
        for line in char_arr:
            sys.stdout.write("\t")
            for char in line:
                sys.stdout.write(f"0x{char:02x}, ")
            print()

    print("};\n")
    print(
        f"sFONT Font32 = {{\n"
        f"\t{font_name}_Table,\n"
        f"\t{3*8}, // width\n"
        f"\t{HEIGHT}, // height\n"
        "};"
    )

if __name__ == "__main__":
    font_name = "noto_serif_regular_24"
    threshold = 100

    (img, meta_data) = load_font(font_name)
    generate_font_arr(img, meta_data, font_name, threshold)
