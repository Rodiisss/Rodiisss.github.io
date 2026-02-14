#!/bin/bash

gcc -ggdb -O3 -lglfw -lGL -lm src/main.c src/gl.c src/shader.c src/stb_image.c  src/tinyobj_loader_c.c -o bin/LearnOpenGL 
