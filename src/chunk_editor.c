#include "virgl.h"
#include "vixx.h"

size_t current_chunk = 0;
static size_t current_voxel = 0;
static bool intersected = false;

extern bool is_in_current_chunk(vx_bounding_box bb) {
    bool min_x = (bb.min.x <= camera.position.x) ? true : false;
    bool max_x = (bb.max.x >= camera.position.x) ? true : false;
    bool min_y = (bb.min.y <= camera.position.y) ? true : false;
    bool max_y = (bb.max.y >= camera.position.y) ? true : false;
    bool min_z = (bb.min.z <= camera.position.z) ? true : false;
    bool max_z = (bb.max.z >= camera.position.z) ? true : false;
    if (min_x && max_x && min_y && max_y && min_z && max_z)
        return true;
    return false;
}

extern void shoot_ray(vx_chunk *chunk) {
    float tmin = 1.0f;
    float tmax = 10.0f;
    da_foreach(vx_sphere, sphere, &chunk->spheres) {
        float t1, t2; // values to possibly limit the range of the ray
        bool ray_intersect = glms_ray_sphere(camera.position, camera.front, sphere->sphere, &t1, &t2);
        bool limit_min = tmin < t1;
        bool limit_max = t2 < tmax;
        if (ray_intersect && (limit_min && limit_max) && !intersected) {
            current_voxel = sphere->location;
            intersected = true;
            fprintf(stderr, "Current Voxel: %zu, t1: %.2f, t2: %.2f\n", current_voxel, t1, t2);
        } else {
            intersected = false;
        }
    }
}

extern void edit_chunk(vx_chunk *chunk, SDL_MouseButtonEvent mv) {
    switch (mv.button) {
        case SDL_BUTTON_LEFT:
        case SDL_BUTTON_RIGHT:
            chunk->mesh_data[current_voxel] = false;
            break;
        default:
            break;
    }

    da_free(chunk->mesh);
    memset(&chunk->mesh, 0, sizeof(chunk->mesh));
    glDeleteVertexArrays(1, &chunk->VAO);
    glDeleteBuffers(1, &chunk->VBO);

    naive_mesh(chunk, CHUNK_SIZE);
    vixx_mesh_buffer_create(chunk);
}
