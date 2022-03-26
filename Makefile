SOURCES := src/main.c


####


NAME := puzzle
CC := C:/msys64/mingw64/bin/gcc.exe
LDFLAGS :=
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj


####


OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(SOURCES))


####


all: run


$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^


run: $(NAME)
	./$(NAME).exe


.PHONY: $(NAME) run