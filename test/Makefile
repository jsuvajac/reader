DIR_Config   = ../lib/Config
DIR_EPD      = ../lib/e-Paper
DIR_FONTS    = ../lib/Fonts
DIR_GUI      = ../lib/GUI
DIR_Src      = .

DIR_BIN      = ./bin

OBJ_C = $(wildcard ${DIR_FONTS}/*.c ${DIR_Src}/*.c )
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

CC = gcc

MSG = -g -O0 -Wall
DEBUG = -D USE_DEBUG
STD = -std=gnu99

CFLAGS += $(MSG) $(DEBUG) $(STD)

LIB = -lm -lpthread

TARGET = test

$(shell mkdir -p $(DIR_BIN))

$(TARGET):${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB) 

${DIR_BIN}/%.o:$(DIR_FONTS)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ 
	
${DIR_BIN}/%.o:$(DIR_Src)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
	rm out.txt

run:
	make $(TARGET)
	@./$(TARGET) > out.txt