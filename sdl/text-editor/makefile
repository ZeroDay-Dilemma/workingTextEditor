CXX = g++
CXXFLAGS = -Wall -g

main: main.c
	$(CXX) $(CXXFLAGS) main.c -o main

run:
	./main

val:
	valgrind ./main

gdb:
	gdb ./main

##Use this to clean out your directory
clean:
	rm *~
	rm *.o
