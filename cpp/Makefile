# NAME := main

# CC := g++

# OBJFILES := main.o

# LDFLAGS :=
# CFLAGS := -Wall -Wextra -Werror


# $(NAME): $(OBJFILES)
# 	$(CC) $(LDFLAGS) -o $@ $^


# %.o: %.c:
# 	$(CC) -c $(CFLAGS) $@ $^


NAME := main
OBJFILES := main.cpp
LDFLAGS ?=
CFLAGS ?= -Wall -Wextra -Werror
COMPILER := g++


$(NAME): $(OBJFILES)
	echo foo
	$(COMPILER) $(LDFLAGS) -o $@.out $^

%.o: %.c
	$(COMPILER) -c $(CFLAGS) -o $@ $^