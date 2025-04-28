# Natrium Example
## Introduction
Simple example application of the Natrium C++ framework

For more info, see: https://github.com/vicultraviolet/natrium-cpp/wiki

## Cloning
This repository uses git submodules, so it is important to either clone it recursively:

```
git clone --recursive https://github.com/vicultraviolet/natrium-cpp-example.git
```

Or init the submodules manually by running:

```
git submodule init
git submodule update
```

## Dependencies
https://github.com/vicultraviolet/natrium-cpp/wiki/Getting-started#dependencies

## Building
Natrium uses premake5, a meta build system, which means it's used to generate other build system files like Visual Studio project files or Makefiles

Run the Generate.py script with the build system you want, e.g.:

```
python3 Generate.py msbuild
```
or
```
python3 Generate.py make
```
