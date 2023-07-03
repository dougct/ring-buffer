
test:
ifeq ($(version),single)
		clang++ -std=c++20 -Wall -Wextra -lgtest SingleThreadRingBufferTest.cpp
		./a.out
else
		clang++ -std=c++20 -Wall -Wextra -lgtest RingBufferTest.cpp
		./a.out
endif

clean:
		rm a.out
