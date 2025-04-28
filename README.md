# Natrium Example
## Introduction
Simple example application of the [Natrium C++ framework](https://github.com/vicultraviolet/natrium-cpp/)

For more info, see: https://github.com/vicultraviolet/natrium-cpp/wiki

## Cloning
This repository uses git submodules, so it is important to either clone it recursively:

```
git clone --recursive https://github.com/vicultraviolet/natrium-cpp-example.git
```

Or get the submodules manually by running:

```
git submodule init
git submodule update
```

## Dependencies
https://github.com/vicultraviolet/natrium-cpp/wiki/Getting-started#dependencies

## Building
Natrium uses [premake5](https://premake.github.io/), a meta build system, which means it's used to generate other build system files like Visual Studio projects or Makefiles

Run the Generate.py script with the desired build system, e.g.:

```
python3 Generate.py msbuild
```
or
```
python3 Generate.py make
```

### Windows
To make it easier, you can just double click the GenerateVs.bat script and it will generate the Visual Studio projects, no need to open a terminal

### Linux
If you use Visual Studio Code, just opening the folder and hitting F5 will generate the project files + start debugging the program