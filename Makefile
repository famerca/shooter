CXX = g++ -std=c++14

LIBS = -lGL -lGLEW -lglfw

all: main

main: main.cpp
	$(CXX) $@.cpp -o shooter.out $(LIBS)

.PHONY:
clean:
	$(RM) *.out
