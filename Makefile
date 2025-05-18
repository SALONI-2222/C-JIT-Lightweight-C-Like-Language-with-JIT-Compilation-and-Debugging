CC = gcc
CFLAGS = -Wall -Wextra
OBJS = main.c parser.c lexer.c ir.c optimizer.c

compiler: $(OBJS)
	$(CC) -o compiler $(OBJS) $(CFLAGS)