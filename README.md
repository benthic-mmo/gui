# What is lxgui? 
lxgui is a platform independent C++ gui library for making applications with a familiar xml/lua API. The main project can be found [![here!](https://github.com/cschreib/lxgui)](https://github.com/cschreib/lxgui) 

# Getting started
Ensure your c++ compiler is up to date. Lxgui requires a compiler that is c++17 compliant (GCC >= 8, clang >= 7, Apple-Clang >= 11, or VisualStudio >= 2017).

Install the required dependencies for your operating system.

- dnf based distros (Fedora):
```
sudo dnf install -y SFML-devel lua-devel spdlog
``` 

- apt based distros (Debian, Ubuntu):
``` 
sudo apt install libsfml-dev liblua5.2-dev spdlog
``` 
- OSX:
``` 
 brew install sfml lua spdlog
``` 
- Windows: 
``` 
 vcpkg install sfml lua spdlog
```

Finally, initialize the submodules.
```
cd lxgui
git submodule update --init
```

# OpenGL setup 
- dnf based distros (Fedora):
```
sudo dnf install -y freetype-devel SFML-devel lua-bit32 mesa-libGLU-devel lua-devel
```

- apt based distros (Debian, Ubuntu):
```
sudo apt install libz-dev libpng-dev libfreetype6-dev libglew-dev libglu1-mesa-dev
``` 

- OSX:
```
brew install zlib libpng freetype glew
```

- Windows: 
```
vcpkg install zlib libpng freetype glew
```
