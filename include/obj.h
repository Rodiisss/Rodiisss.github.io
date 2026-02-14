#ifndef OBJ_H
#define OBJ_H
#include "../include/tinyobj_loader_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    float pos[3];
    float normal[3];
    float uv[2];
} Vertex;

void tinyobj_file_reader(void *ctx, const char *file_name, int is_mtl, const char *obj_filename, char **buf, size_t *len); 
void loadObj (const char* filePath, Vertex** vertices, uint32_t** indices, size_t* vertex_count, size_t* index_count); 


#endif
