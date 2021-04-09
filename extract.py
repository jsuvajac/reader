import pycpdf
from pprint import pprint
import collections

def generate_alphabet(file_name):
    all_text = ""
    alphabet = ""
    alphabet_dict = {}
    pdf = pycpdf.PDF(open(file_name, 'rb').read())

    # if pdf.info and pdf.info.get('Title'):
        # print('Title:', pdf.info['Title'])

    # get all text
    for pageno, page in enumerate(pdf.pages):
        # print('Page', pageno + 1)
        # print(page.text.translate(pycpdf.unicode_translations))
        all_text += page.text.translate(pycpdf.unicode_translations)

    for char in all_text:
        ascii_num = ord(char)
        if ascii_num not in alphabet_dict:
            alphabet_dict[ascii_num] = char

    # sort alphabet
    sorted_chars = collections.OrderedDict(sorted(alphabet_dict.items()))
    pprint(sorted_chars)
    
    # create alphabet string
    for key, val in enumerate(sorted_chars):
        alphabet += chr(val)
    print(alphabet)
    
def extract_text(file_name):
    all_text = ""
    pdf = pycpdf.PDF(open(file_name, 'rb').read())

    # if pdf.info and pdf.info.get('Title'):
        # print('Title:', pdf.info['Title'])

    # get all text
    for pageno, page in enumerate(pdf.pages):
        # print('Page', pageno + 1)
        print(page.text.translate(pycpdf.unicode_translations))
        all_text += page.text.translate(pycpdf.unicode_translations)

def extract_info(file_name):
    pdf = pycpdf.PDF(open(file_name, 'rb').read())
    if pdf.info:
        pprint(pdf.info)


def extract_page(file_name, page):
    all_text = ""
    pdf = pycpdf.PDF(open(file_name, 'rb').read())
    print(pdf.pages[page-1].text.translate(pycpdf.unicode_translations))


if __name__ == "__main__":
    import sys

    file_name = ""
    actions = {
        "text": extract_text,
        "alphabet": generate_alphabet,
        "page": extract_page,
        "info": extract_info
    }

    if len(sys.argv) < 3 or len(sys.argv) > 4:
        action_str = " | ".join([str(key) for key in actions.keys()])
        sys.stderr.write(f"{sys.argv[0]} file.pdf [ {action_str} ]\n")
        exit(1)
    elif len(sys.argv) == 4 and sys.argv[2] == "page":
        actions[sys.argv[2]](sys.argv[1], int(sys.argv[3]))
    else:
        actions[sys.argv[2]](sys.argv[1])

