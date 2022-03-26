SOURCES := cpp/main.cpp


####


NAME := puzzle
CC := C:/msys64/mingw64/bin/g++.exe
LDFLAGS :=
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := cpp
OBJ_DIR := obj


####


OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SOURCES))


####


all: run


$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^


run: $(NAME)
	./$(NAME).exe


.PHONY: $(NAME) run