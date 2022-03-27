SOURCES := cpp/src/main.cpp


####


NAME := puzzle
CC := C:/msys64/mingw64/bin/g++.exe
LDFLAGS :=
CFLAGS := -Wall -Wextra -Werror
CFLAGS += -std=c++11 # nlohmann::json asks this to be added, although it doesn't seem necessary
FCLEANED_FILES := puzzle.exe

SRC_DIR := cpp/src
OBJ_DIR := cpp/obj


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


run: $(NAME)
	./$(NAME).exe


.PHONY: all $(NAME) clean fclean re run
