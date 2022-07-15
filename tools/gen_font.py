import json
import sys
from pprint import pprint

import png

HEIGHT = 32


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

    return arr

def line_to_bytes(line):
    ''' converts an bin array to a byte array '''

    # enforce 24 bit length
    while len(line) > 24:
        line.remove(0)

    while len(line) < 24:
        line.append(0)

    # group by byte sized bit vectors
    line = [line[i: i + 8] for i in range(0, len(line), 8)]

    # squash bit vectors into bytes
    line = [int("".join(str(x) for x in byte), 2) for byte in line]

    return line
    
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
    print('#include "fonts.h"\n')
    print(f"const uint8_t {font_name}_Table [] = {{")

    # print each character
    for index, char_data in enumerate(meta_data['symbols']):
        # pprint(char_data)
        print(f"\t// {chr(char_data['id'])} ({char_data['id']}) {index}")

        # print char array data
        char_arr = gen_char_array(img, char_data, threshold=threshold)
        for line in char_arr:
            sys.stdout.write("\t")
            for char in line:
                sys.stdout.write(f"0x{char:02x}, ")
            print()

    print("};\n")

    # lookup function
    print("int get_start_index(uint16_t char_code) {")
    print("\tswitch(char_code) {")

    for index, char_data in enumerate(meta_data['symbols']):
        print(f"\t\tcase({char_data['id']}): return {index * HEIGHT}; // {chr(char_data['id'])}")

    print("\t}")
    print("\treturn -1;")
    print("}")

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
