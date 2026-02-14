#!/bin/bash


emcc src/main.c src/shader.c src/stb_image.c src/tinyobj_loader_c.c\
    -o index.js \
    -I./include \
    -I./shaders \
    -s USE_GLFW=3 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s WASM=1 \
    --preload-file shaders \
    --preload-file textures \
    --preload-file objs \
    -O3 
