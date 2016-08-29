# lessdb

## Introduction
**lessdb** is my rewrite of leveldb in C++11.

## Platform
**lessdb** is not intended to cross platform, unlike [leveldb](https://github.com/google/leveldb), it only supports
linux.

## Dependencies

+ silly library: [https://github.com/IppClub/silly.git](https://github.com/IppClub/silly.git)
+ googletest(bundled with lessdb): [https://github.com/neverchanje/lessdb.git](https://github.com/neverchanje/lessdb.git)

Some of the dependencies not mentioned above are dependencies already required for silly, including:
+ boost: [http://www.boost.org/](http://www.boost.org/)
+ folly: [https://github.com/facebook/folly](https://github.com/facebook/folly)

## Build

```shell
> git clone --recursive https://github.com/neverchanje/lessdb.git
```

Build with CMake in root directory of lessdb.
```shell
> mkdir build && cd build
> cmake ..
```

## TODO
- [ ] Automatic deployment and testing on travis-ci.