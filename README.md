## Compile & run using CMake
```
mkdir build
cd build
cmake ../
cmake --build .
./SGtest
```
## Compile & run using QMake
```
mkdir build
cd build
qmake ../
make
./SGtest
```
## Getting started

There are 6 test examples which can interchanged in CMakeLists.txt (CMake) or SGtest.pro (QMake / QTCreator).
```
test05-filescanner.cpp
test04-numericpad.cpp
test03-library.cpp
test02-registerform.cpp
test01-packager.cpp
test00-helloworld.cpp
```
In addition is included an empty test example.
```
main.cpp
```
Uncomment and comment appropiately, one at a time.
