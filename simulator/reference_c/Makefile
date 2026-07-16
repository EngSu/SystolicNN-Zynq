CC=gcc

CFLAGS=-Wall -O2


SRC=src/systolic_reference.c \
    src/systolic_array.c \
    src/pe.c \
    src/data_loader.c


TARGET=systolic_reference


all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm


clean:
	rm -f $(TARGET)