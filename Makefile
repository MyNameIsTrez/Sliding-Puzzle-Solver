SOURCES :=									\
	code/cpp/src/main.cpp					\
	code/cpp/src/sliding_puzzle_solver.cpp


####


NAME := puzzle

CC := C:/msys64/mingw64/bin/g++.exe # No vector debug info.
# CC := C:/msys64/mingw64/bin/gcc.exe # Errors.
# CC := C:/msys64/mingw64/bin/gdb.exe # No vector debug info. Errors in puzzle.exe?
# CC := lldb # TODO: Download this?

LDFLAGS :=
CFLAGS := -Wall -Wextra -Werror
# CFLAGS += -Og
CFLAGS += -g3
# CFLAGS += -O3
FCLEANED_FILES := puzzle.exe

SRC_DIR := code/cpp/src
OBJ_DIR := code/cpp/obj


####


OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SOURCES))


####


all: $(NAME)


$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^


clean:
	rm -rf $(OBJ_DIR)


fclean: clean
	rm -f $(FCLEANED_FILES)


re: fclean all


# run: $(NAME)
# 	./$(NAME).exe


.PHONY: all $(NAME) clean fclean re #run
