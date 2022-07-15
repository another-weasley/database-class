all: example

example: example.o database.o 
	g++ -o example example.o database.o -lpq

database.o: database.hpp database.cpp
	g++ -c database.cpp 

example.o: example.cpp
	g++ -c example.cpp

clean:
	rm example.o database.o example