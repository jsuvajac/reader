# Reader
e-Reader experimentation with 7.8 in  waveshare e-Paper Module on the Raspberry Pi


# Build
```
sudo make -j4 && sudo ./reader
```

# Processing pipeline
- extracting unique characters out of a pdf
- 
```
pip3 install -r requirements.txt
```
* extract.py -> get text from pdf and generate all unique characters
* pick fonts
* fongbuilder -> generate font png and json
* gen_font.py -> create c file with picture data

### Characters
```
 !"%&'()*+,-.0123456789:;=?ABCDEFGHIJKLMNOPQRSTUVWXYZ[]abcdefghijklmnopqrstuvwxyz|«éĆćČčĐđŠšŽžαιξρστχДНПРХабвгдезийклмнопрстуфцчшыьяјњ„•
 ```