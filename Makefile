
test:
ifeq ($(version),single)
		clang++ -std=c++20 -Wall -Wextra -lgtest SingleThreadedRingBufferTest.cpp
		./a.out
else
		clang++ -std=c++20 -Wall -Wextra -lgtest RingBufferTest.cpp
		./a.out
endif

bench:
		clang++ -std=c++20 -Wall -Wextra -O3 Benchmark.cpp
		./a.out

clean:
		rm a.out
