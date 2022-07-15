#include "../lib/Fonts/fonts.h"

#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
 
#include <stdint.h>

void print_character(sFONT font, wchar_t letter) {
    uint16_t character = (uint16_t)letter;

    int byte_width = font.Width / 8;
    int offset = get_start_index(character) * byte_width;

    printf("%lc (%d) -> %d\n", letter, character, offset);
    for (int i = offset; i < offset + font.Height * byte_width; i += byte_width) {
        for (int ii = 0; ii < byte_width; ++ii) {
            for (int index = 0; index < 8; ++index){
                printf("%s", (Font32.table[i+ii] & (0x01 << (7-index))) == 0 ? "  " : "$$");
            }
        }
        printf("\n");
    }
}


int main(void) {
    setlocale(LC_ALL, "");
    wchar_t test_str[] = L" !q\"%&'()*+,-.0123456789:;=?ABCDEFGHIJKLMNOPQRSTUVWXYZ[]abcdefghijklmnopqrstuvwxyz|«éĆćČčĐđŠšŽžαιξρστχДНПРХабвгдезийклмнопрстуфцчшыьяјњ„•";
    size_t str_len = sizeof(test_str)/sizeof(wchar_t) - 1;

    printf("Test character map\n");
    printf("%ls\n\n", test_str);

    for (int i = 0; i < str_len; ++i) {
        if (i % 8 == 0 && i != 0) printf("\n");

        wint_t c = test_str[i];
        printf("%lc -> %4d ", c, (uint16_t)c);

        assert(get_start_index((uint16_t)c) != -1);
    }
    printf("\n\n");

    printf("Test character visually\n");
    for (int i = 0; i < str_len; ++i) {
        print_character(Font32, test_str[i]);
    }
    printf("\n\n");
 
    return 0;
}