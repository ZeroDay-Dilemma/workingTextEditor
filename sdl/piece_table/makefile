CXX = g++
CXXFLAGS = -Wall -g
#SDLFLAGS = `sdl2-config --cflags --libs`
main: piece_table.o driver.cpp
					$(CXX) $(CXXFLAGS) piece_table.o driver.cpp -o main #$(SDLFLAGS)

piece_table.o: piece_table.cpp piece_table.h
				$(CXX) $(CXXFLAGS) -c piece_table.cpp

run:
					./main

val:
					valgrind ./main

gdb:
					gdb ./main

zz: piece_table.cpp
					$(CXX) $(CXXFLAGS) piece_table.cpp -o main
zzr:
					./main

clean:
					rm *~
					rm *.o
