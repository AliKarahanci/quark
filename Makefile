# Quark 
#
# Maintainer
# Ali Karahancı <karahanciali@hotmail.com>
#
# 

CC = gcc
CFLAGS = -O2 -Wall
TARGET = quark
SRC = src/main.c
INSTALL_DIR = /usr/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)/$(TARGET)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)

clean:
	rm -f $(TARGET)
