# Custom C++ Containers

A collection of custom data containers for use in C++.

## Set

The Set class provides various methods to `Insert`, `Erase`, and check (`Contains`) for the presence of elements in the set, all while maintaining efficient storage and calculation using bitwise operations on a single integer. 

```cpp
enum class Options {
  kOptions1,
  kOptions2,
  kOptions3,
  kOptions4,
  kOptions5,
  kOptions6,
  kOptions7,
  kOptions8,
  kOptions9,
  kOptions10
};

typedef Set<Options, Options::kOptions1, Options::kOptions10> SetType;

SetType set;
set << Options::kOptions1 << Options::kOptions2 << Options::kOptions3;

if (set.Contains(Options::kOptions1)) {
  printf("The set contains kOptions1\n");
}
```

## License

MIT - see LICENSE file for details.
