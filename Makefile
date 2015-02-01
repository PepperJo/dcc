
all:
	$(CXX) -std=c++11 -O3 -march=native main.cpp -o dcc

clean:
	rm dcc
	rm *~
