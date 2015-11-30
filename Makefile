# Name: Tzuo Shuin Yew, Jeremy Tan
# CSC444 Assignment 2 Makefile
# This makefile utility contains commands to compile all the files
# into an executable called toygit (or toygittest)

make toygit: toygit.o main.o unittest.o
	g++ -o toygit toygit.o main.o
	g++ -o unittest toygit.o unittest.o 

unittest.o:
	g++ -g -c unittest.cpp

main.o: main.cpp
	g++ -g -c main.cpp

toygit.o: toygit.cpp
	g++ -g -c toygit.cpp

clean:
	rm toygit unittest main.o toygit.o unittest.o
