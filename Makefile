CXX = g++
CXXFLAGS = -std=c++11 -Iinclude -O3

all: main benchmark

main: src/main.cpp include/*.hpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o main

benchmark: tests/benchmark.cpp include/*.hpp
	$(CXX) $(CXXFLAGS) tests/benchmark.cpp -o benchmark

clean:
	rm -f main benchmark
