# Ring buffer implementation in C++

This repository contains two implementations of ring buffers in C++:

- A single-thread ring buffer to help us understand the challenges/tradeoffs of a real-world ring buffer.
- A multi-thread implementation which is pretty similar to state-of-the-art implementations such as [the one](https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h) in [folly](https://github.com/facebook/folly).

[Here's a blog post](https://dougct.github.io/blog/ring-buffer/) with a detailed description of each implementation.

## How to build and run

To run the test for the single-thread ring buffer, do:

```bash
make test version=single
```

And just do `make test` to run the tests for the multi-thread ring buffer.

To run a simple benchmark, just do `make bench`.
