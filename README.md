# VN-Raylib

This is a study project to learn Raylib and C programming language. The goal is to create a simple game using Raylib and C.

## How to build and run

1. Install Raylib and cJSON

```bash
sudo apt-get install libraylib-dev
sudo apt-get install libcjson-dev
```

2. Build the project

```bash
gcc ./src/main.c -o vn `pkg-config --cflags --libs raylib libcjson`
```
