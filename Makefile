CC      = gcc
DIR_SRC = solvers
DIR_INC = include
CFLAGS  = -Wall -O2 -I$(DIR_INC)
LDFLAGS = -lm
SRC     = $(wildcard $(DIR_SRC)/*.c)
INC     = $(wildcard $(DIR_INC)/*.h)
TARGET  = Sudoku

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(INC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
