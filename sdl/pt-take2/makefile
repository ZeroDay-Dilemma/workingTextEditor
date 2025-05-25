CXX = g++
CXXFLAGS = -Wall -g
SDLFLAGS = `sdl2-config --cflags --libs`
main: pt-one.o driver.cpp
					$(CXX) $(CXXFLAGS) pt-one.o driver.cpp -o main $(SDLFLAGS)

pt-one.o: pt-one.cpp #piece_table.h
				$(CXX) $(CXXFLAGS) -c pt-one.cpp

run:
					./main

val:
					valgrind ./main

gdb:
					gdb ./main

zz: pt-one.cpp
					$(CXX) $(CXXFLAGS) pt-one.cpp -o main
zzr:
					./main

clean:
					rm *~
					rm *.o
