Compile it with:
```
clang++ -O3 -std=c++11 -I./firrtl-sig -I./cc top.cc ./cc/mm.cc -o top
```
Run it with:
```
./top towers.riscv.hex
```
