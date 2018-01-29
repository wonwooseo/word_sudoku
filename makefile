EXENAME = wsudoku

CXX = clang++
LD = clang++

CXXFLAGS = -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -Werror -pedantic
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi

all: $(EXENAME)

wsudoku: testgrid.o
	$(LD) testgrid.o $(LDFLAGS) -lpthread -o wsudoku

testgrid.o: testgrid.cpp grid.h
	$(CXX) $(CXXFLAGS) testgrid.cpp
	
clean: 
	-rm -f *.o $(EXENAME)
