gcc -Wall -c -O2 -g src/lexer.c -o o/lexer.o
gcc -Wall -c -O2 -g src/parser.c -o o/parser.o
gcc -Wall -c -O2 -g src/ast.c -o o/ast.o
gcc -Wall -c -O2 -g src/symboltable.c -o o/symboltable.o
gcc -Wall -c -O2 -g src/variable.c -o o/variable.o
gcc -Wall -c -O2 -g src/interpreter.c -o o/interpreter.o
gcc -Wall -c -O2 -g src/hl.c -o o/hl.o
gcc -Wall -c -O2 -g src/debug.c -o o/debug.o

gcc -Wall -g -O2 o/lexer.o o/parser.o o/ast.o o/variable.o o/interpreter.o o/symboltable.o o/hl.o o/debug.o -o hl