gcc -Wall -c -O2 src/lexer.c -o src/lexer.o
gcc -Wall -c -O2 src/parser.c -o src/parser.o
gcc -Wall -c -O2 src/ast.c -o src/ast.o
gcc -Wall -c -O2 src/symboltable.c -o src/symboltable.o
gcc -Wall -c -O2 src/variable.c -o src/variable.o
gcc -Wall -c -O2 src/interpreter.c -o src/interpreter.o
gcc -Wall -c -O2 src/hl.c -o src/hl.o

gcc -Wall -O2 src/lexer.o src/parser.o src/ast.o src/variable.o src/interpreter.o src/symboltable.o src/hl.o -o hl