cls
gcc -Wall -c -O2 -pg src/lexer.c -o o/lexer.o
gcc -Wall -c -O2 -pg src/parser.c -o o/parser.o
gcc -Wall -c -O2 -pg src/ast.c -o o/ast.o
gcc -Wall -c -O2 -pg src/symboltable.c -o o/symboltable.o
gcc -Wall -c -O2 -pg src/variable.c -o o/variable.o
gcc -Wall -c -O2 -pg src/interpreter.c -o o/interpreter.o
gcc -Wall -c -O2 -pg src/hl.c -o o/hl.o
gcc -Wall -c -O2 -pg src/debug.c -o o/debug.o
gcc -Wall -c -O2 -pg src/capi.c -o o/capi.o

gcc -Wall -c -O2 -pg lib/standard.c -o o/standard.o
gcc -Wall -c -O2 -pg lib/list.c -o o/list.o
gcc -Wall -c -O2 -pg lib/stack.c -o o/stack.o

gcc -Wall -O2 -pg o/lexer.o o/parser.o o/ast.o o/variable.o o/interpreter.o o/symboltable.o o/hl.o o/debug.o o/capi.o o/standard.o o/list.o o/stack.o -o hl