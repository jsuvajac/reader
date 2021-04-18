# Reader
e-Reader experimentation with 5.65-7-color waveshare e-Paper Module on the Raspberry Pi

### TODO
* Integrate font
* Integrate control buttons


# Processing pipeline
```
pip3 install -r requirements.txt
```
* extract.py -> get text from pdf and generate all unique characters
* pick fonts
* fongbuilder -> generate font png and json
* gen_font.py -> create c file with picture data


# e-Reader
```
make
./reader
```
### Characters
```
 !"%&'()*+,-.0123456789:;=?ABCDEFGHIJKLMNOPQRSTUVWXYZ[]abcdefghijklmnopqrstuvwxyz|«éĆćČčĐđŠšŽžαιξρστχДНПРХабвгдезийклмнопрстуфцчшыьяјњ„•
 ```