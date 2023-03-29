# Parallel
Parallel is a group of headers that help [processing arrays](include/pl/parallel.hpp), creating [thread pools](include/pl/thread_pool.hpp), and making [values thread safe](include/pl/safe.hpp)

__Pros__

- the more cores the computer has the faster it can process
- easy to use and understand

__Cons__

- doesn't guarantee processing order (obviously)
- when breaking doesn't guarantee that others workers do it instantly

# Example
For a full example you can go [here](test/test.cpp)