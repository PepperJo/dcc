
all:
	$(CXX) -std=c++11 -O3 -march=native main.cpp -I./jsoncpp/include -ljsoncpp -o dcc

clean:
	rm dcc
	rm *~
