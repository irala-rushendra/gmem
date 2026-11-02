CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

allocator: memory_region.cpp main.cpp memory_region.hpp
	$(CXX) $(CXXFLAGS) memory_region.cpp main.cpp -o allocator

leakcheck:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./allocator

clean:
	rm -f allocator
