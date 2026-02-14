#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../include/obj.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void tinyobj_file_reader(void *ctx, const char *file_name, int is_mtl, const char *obj_filename, char **buf, size_t *len) {
    (void)ctx;
    (void)is_mtl;
    (void)obj_filename;

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = (char *)malloc(size + 1);
    fread(data, 1, size, fp);
    fclose(fp);

    data[size] = '\0';
    *buf = data;
    *len = size;
}


 void loadObj (const char* filePath, Vertex** out_vertices, uint32_t** out_indices, size_t* vertex_count, size_t* index_count) {

    tinyobj_attrib_t attrib;
    tinyobj_attrib_init(&attrib); 
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;


    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE; // Triangulate all faces
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes,
                                &materials, &num_materials,
                                filePath, tinyobj_file_reader, NULL, flags);

    if (ret != TINYOBJ_SUCCESS) {
        printf("Failed to load OBJ\n");
        exit(1);
    }

    // Count total indices across all shapes
    size_t total_vertices = 0;
    for (size_t f = 0; f < attrib.num_face_num_verts; f++) {
        total_vertices += attrib.face_num_verts[f];
    }

    // Allocate arrays
    Vertex* vertices = (Vertex*)malloc(total_vertices * sizeof(Vertex));
    uint32_t* indices = (uint32_t*)malloc(total_vertices * sizeof(uint32_t));

    if (!vertices || !indices) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    size_t vertex_offset = 0;
    size_t index_offset = 0;

    // Process each shape
    for (size_t s = 0; s < num_shapes; s++) {
        size_t face_offset = shapes[s].face_offset;
        // Process each face
        for (size_t f = 0; f < shapes[s].length; f++) {
            unsigned char fv = attrib.face_num_verts[face_offset + f];
            
            // Process each vertex in the face
            for (size_t v = 0; v < fv; v++) {
                tinyobj_vertex_index_t idx = attrib.faces[index_offset];
                
                Vertex vertex = {0};
                memset(&vertex, 0, sizeof(Vertex));
                
                // Position
                vertex.pos[0] = attrib.vertices[3 * idx.v_idx + 0];
                vertex.pos[1] = attrib.vertices[3 * idx.v_idx + 1];
                vertex.pos[2] = attrib.vertices[3 * idx.v_idx + 2];
                
                // Normal (if available)
                if (idx.vn_idx >= 0) {
                    vertex.normal[0] = attrib.normals[3 * idx.vn_idx + 0];
                    vertex.normal[1] = attrib.normals[3 * idx.vn_idx + 1];
                    vertex.normal[2] = attrib.normals[3 * idx.vn_idx + 2];
                } else {
                    vertex.normal[0] = 0.0f;
                    vertex.normal[1] = 0.0f;
                    vertex.normal[2] = 0.0f;
                }
                
                // Texture coordinates (if available)
                if (idx.vt_idx >= 0) {
                    vertex.uv[0] = attrib.texcoords[2 * idx.vt_idx + 0];
                    vertex.uv[1] = attrib.texcoords[2 * idx.vt_idx + 1];
                } else {
                    vertex.uv[0] = 0.0f;
                    vertex.uv[1] = 0.0f;
                }
                
                vertices[vertex_offset] = vertex;
                indices[vertex_offset] = (uint32_t)vertex_offset;
                vertex_offset++;
                index_offset++;
            }
            
            // face_offset++
            // index_offset += fv;
        }
    }

    *out_vertices = vertices;
    *out_indices = indices;
    *vertex_count = vertex_offset;
    *index_count = vertex_offset;

    printf("Loaded OBJ: %zu vertices, %zu indices\n", vertex_offset, vertex_offset);


    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
}

