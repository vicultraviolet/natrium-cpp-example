# Natrium Example
<img width="1312" height="890" alt="image" src="https://github.com/user-attachments/assets/458a2950-7017-45a3-9d1c-bd0d3179352a" />

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

Run GenerateVs.bat for Visual Studio project files or GenerateMk.sh for Makefiles then build normally!
