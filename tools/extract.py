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

def get_num_pages(file_name):
    pdf = pycpdf.PDF(open(file_name, 'rb').read())
    print(len(pdf.pages))

def extract_page(file_name, page):
    all_text = ""
    pdf = pycpdf.PDF(open(file_name, 'rb').read())
    text = pdf.pages[page-1].text.translate(pycpdf.unicode_translations)
    page_num = str(page)

    print("\n")
    # page number at end
    try:
        int(text[-len(page_num):])
        print(text[:-len(page_num)])
    except ValueError:
        pass

    # page number at start
    try:
        int(text[:len(page_num)])
        print(text[len(page_num):])
    except ValueError:
        pass

def print_help(argv):
    help_text = {
        "text": "(Print the entire text from pdf)",
        "alphabet": "(Print all unique characters)",
        "info": "(Print PDF info)",
        "page": "[page] or [start page] [pages to print from start] (Print specific page(s) from a start page.)",
        "page-num": "(Prints total number of pages)"
    }

    action_str = "\n    ".join([str(key).ljust(len("alphabet ")) + str(help_text[key]) for key in actions.keys()])
    sys.stderr.write(f"\n{sys.argv[0]} file.pdf \n    {action_str}\n\n")


if __name__ == "__main__":
    import sys

    file_name = ""
    actions = {
        "text": extract_text,
        "alphabet": generate_alphabet,
        "page": extract_page,
        "info": extract_info,
        "page-num": get_num_pages
    }

    # print(sys.argv)

    # help
    if len(sys.argv) < 2 or len(sys.argv) > 5:
        print_help(sys.argv)
        exit(1)

    # text | alphabet | info
    if len(sys.argv) == 3 and sys.argv[2] in ["text", "alphabet", "info", "page-num"]:
        actions[sys.argv[2]](sys.argv[1])
    elif len(sys.argv) == 3:
        print_help(sys.argv)
        exit(1)

    # specific page 
    elif len(sys.argv) == 4 and sys.argv[2] == "page":
        actions[sys.argv[2]](sys.argv[1], int(sys.argv[3]))

    # n pages from start page 
    elif len(sys.argv) == 5 and sys.argv[2] == "page":
        for i in range(int(sys.argv[4])):
            actions[sys.argv[2]](sys.argv[1], int(sys.argv[3]) + i)

    # help
    else:
        print_help(sys.argv)
        exit(1)
