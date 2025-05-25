CXX = g++
CXXFLAGS = -Wall -g
SDLFLAGS = `sdl2-config --cflags --libs`

main: sdl-six.cpp
	        $(CXX) $(CXXFLAGS) sdl-six.cpp -o main $(SDLFLAGS)

run:
	        ./main

val:
	        valgrind ./main

gdb:
	        gdb ./main

clean:
	        rm *~
		        rm *.o
