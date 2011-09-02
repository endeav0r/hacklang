cls
gcc -Wall -c -O2 src/lexer.c -o o/lexer.o
gcc -Wall -c -O2 src/parser.c -o o/parser.o
gcc -Wall -c -O2 src/ast.c -o o/ast.o
gcc -Wall -c -O2 src/symboltable.c -o o/symboltable.o
gcc -Wall -c -O2 src/variable.c -o o/variable.o
gcc -Wall -c -O2 src/interpreter.c -o o/interpreter.o
gcc -Wall -c -O2 src/hl.c -o o/hl.o
gcc -Wall -c -O2 src/debug.c -o o/debug.o
gcc -Wall -c -O2 src/capi.c -o o/capi.o

gcc -Wall -c -O2 lib/standard.c -o o/standard.o
gcc -Wall -c -O2 lib/list.c -o o/list.o
gcc -Wall -c -O2 lib/stack.c -o o/stack.o

gcc -Wall -O2 o/lexer.o o/parser.o o/ast.o o/variable.o o/interpreter.o o/symboltable.o o/hl.o o/debug.o o/capi.o o/standard.o o/list.o o/stack.o -o hl