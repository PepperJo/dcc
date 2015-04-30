
all:
	$(CXX) -std=c++11 -O2 main.cpp -ljsoncpp -o dcc

clean:
	rm dcc
	rm *~
