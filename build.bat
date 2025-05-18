@echo off
gcc -o compiler main.c parser.c lexer.c ir.c optimizer.c -Wall -Wextra
echo Build complete. Run with: compiler input.txt