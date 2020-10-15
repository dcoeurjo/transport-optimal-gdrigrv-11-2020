# Build the C++ project


## Linux / MacOS

Default setting (using cmake - makefile)

```
mkdir build
cd build
cmake ..
make
```

To build the code in release mode, consider using `cmake .. -DCMAKE_BUILD_TYPE=Release`.

(If you want to get a Codeblock / Eclipse or Xcode project, speficy the generator at the cmake step)

## Windows

Use the cmake GUI and generate the Visual Studio projet for instance.
