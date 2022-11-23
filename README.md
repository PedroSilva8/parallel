# parallel
parallel is a lightweight library that simplifies the job of multithreading the processing of arrays by doing most of the heavy lifting and giving up to 4x faster processing time then using a normal for loop

__Pros__

- the more cores the computer has the faster it can process
- easy to use and understand

__Cons__

- doesn't guarantee processing order (obviously)
- when breaking doesn't guarantee that others workers do it instantly

# example
for a full example you can go [here](test/test.cpp)

normal for loop
```c++
    for (auto i = 0; i < values.size(); i++)
        values[i] = calculation(i);
```

parallel for loop
```c++
    pl::_for(0, values.size(), [&](int i) {
        values[i] = calculation(i);
        return true;
    });
```

normal "foreach" loop
```c++
    for (auto v : values)
        auto value = calculation(v);
 ```
 
 parallel "foreach" loop
 ```c++
    pl::_foreach<int>(values.data(), values.size(), [&](int v) {
        values[v] = calculation(v);
        return true;
    });
```

# building
## Linux
CMake and CPP base-kit development tools should be enough
## Windows
this library uses MinGW to compile for windows, cmake and base-devel for MinGW should be enough
