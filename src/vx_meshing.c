#define STB_PERLIN_IMPLEMENTATION
#include "winrend.h"
#include "vixx.h"

#define IDC_VALUES 0, 0, 0

vx_world wrld = {0};

#define VOXEL_SCALE 0.5
const float vertices[] = {
    -VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,
     VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,
     VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,
     VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,
    -VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,
    -VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  0.0f, -1.0f,

    -VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,
     VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,
     VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,
     VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,
    -VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,
    -VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  0.0f,  1.0f,

    -VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE, -1.0f,  0.0f,  0.0f,
    -VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE, -1.0f,  0.0f,  0.0f,
    -VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE, -1.0f,  0.0f,  0.0f,
    -VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE, -1.0f,  0.0f,  0.0f,
    -VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE, -1.0f,  0.0f,  0.0f,
    -VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE, -1.0f,  0.0f,  0.0f,

     VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  1.0f,  0.0f,  0.0f,
     VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  1.0f,  0.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  1.0f,  0.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  1.0f,  0.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  1.0f,  0.0f,  0.0f,
     VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  1.0f,  0.0f,  0.0f,

    -VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f, -1.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f, -1.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f, -1.0f,  0.0f,
     VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f, -1.0f,  0.0f,
    -VOXEL_SCALE, -VOXEL_SCALE,  VOXEL_SCALE,  0.0f, -1.0f,  0.0f,
    -VOXEL_SCALE, -VOXEL_SCALE, -VOXEL_SCALE,  0.0f, -1.0f,  0.0f,

    -VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  1.0f,  0.0f,
    -VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  1.0f,  0.0f,
     VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  1.0f,  0.0f,
     VOXEL_SCALE,  VOXEL_SCALE,  VOXEL_SCALE,  0.0f,  1.0f,  0.0f,
     VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  1.0f,  0.0f,
    -VOXEL_SCALE,  VOXEL_SCALE, -VOXEL_SCALE,  0.0f,  1.0f,  0.0f
};

static void vx_mesh_maker_bool_grid(vx_chunk *chunk, int sq_dim) {
    for (int k = 0; k < sq_dim; ++k) {
        for (int j = 0; j < sq_dim; ++j) {
            for (int i = 0; i < sq_dim; ++i) {
                float scale = 0.1f;
                float noise_3d = stb_perlin_fbm_noise3(
                        i * scale, j * scale, k * scale,
                        1.0f, 0.75f, 12);
                float unsigned_noise = (noise_3d + 1.0f) * 0.5f;
                if (unsigned_noise >= 0.55f)
                    chunk->mesh_data[i + sq_dim * (j + sq_dim * k)] = SOLID;
                else
                    chunk->mesh_data[i + sq_dim * (j + sq_dim * k)] = AIR;
            }
        }
    }
}

static void make_bounding_box(vx_chunk *chunk) {
    chunk->bounding_box.min = chunk->chunk_pos;
    chunk->bounding_box.max = glms_vec3_add(chunk->chunk_pos, (vec3s){ CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE });
}

static void make_voxel_bounding_spheres(vx_chunk *chunk, int sq_dim) {
    for (int k = 0; k < sq_dim; ++k) {
        for (int j = 0; j < sq_dim; ++j) {
            for (int i = 0; i < sq_dim; ++i) {
                if (chunk->mesh_data[i + sq_dim * (j + sq_dim * k)] == SOLID) {
                    vec4s sphere_pos = (vec4s){ i, j, k, VOXEL_SCALE/2.0f };
                    vec4s sphere_chunk_pos_rel = glms_vec4_add(sphere_pos, glms_vec4(chunk->chunk_pos, 0.0f));
                    vx_sphere sphere = (vx_sphere){
                        .sphere = sphere_chunk_pos_rel,
                        .location = (size_t)(i + sq_dim * (j + sq_dim * k)),
                    };
                    da_append(&chunk->spheres, sphere);
                }
            }
        }
    }
}

extern bool surrounded(block_type *grid, int x, int y, int z, int size) {
    const int offsets[6][3] = {
        {1, 0, 0}, {-1, 0, 0},
        {0, 1, 0}, {0, -1, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    int surrounded = 0;
    for (size_t i = 0; i < 6; ++i) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];
        int nz = z + offsets[i][2];

        if (nx < 0 || nx >= size ||
                ny < 0 || ny >= size ||
                nz < 0 || nz >= size)
            continue;

        size_t pos = nx + ny * size + nz * size * size;
        if (grid[pos] == SOLID) surrounded++;
    }

    if (surrounded == 6) return true;
    return false;
}

extern void naive_mesh(vx_chunk *chunk, int sq_dim) {
    for (int k = 0; k < sq_dim; ++k) {
        for (int j = 0; j < sq_dim; ++j) {
            for (int i = 0; i < sq_dim ; ++i) {
                if (chunk->mesh_data[i + sq_dim * (j + sq_dim * k)] == SOLID) {
                    if (surrounded(chunk->mesh_data, i, j, k, sq_dim)) continue; // skip voxels that are surrounded by other voxels
                    for (size_t l = 0; l < 36; ++l) {
                        vec3s voxel_pos = (vec3s){ i, j, k };
                        vec3s triangle  = (vec3s){ vertices[l*6], vertices[l*6+1], vertices[l*6+2] };
                        vec3s normal    = (vec3s){ vertices[l*6+3], vertices[l*6+4], vertices[l*6+5] };
                        triangle        = glms_vec3_add(triangle, voxel_pos);
                        da_append_many(&chunk->mesh, triangle.raw, 3);
                        da_append_many(&chunk->mesh, normal.raw, 3);
                        da_append_many(&chunk->mesh, chunk->chunk_pos.raw, 3);
                    }
                }
            }
        }
    }
}

extern void vx_world_clanker_chunker() {
    wrld.chunks = malloc(GRID_AREA * sizeof(vx_chunk));

    float world_x = 0.0f;
    float world_z = 0.0f;
    for (size_t i = 0; i < GRID_AREA; ++i) {
        memset(&wrld.chunks[i], 0, sizeof(vx_chunk));
        wrld.chunks[i].mesh_data = malloc(sizeof(block_type) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
        vec3s grid_pos = (vec3s){ world_x, 0.0f, world_z };

        vx_mesh_maker_bool_grid(&wrld.chunks[i], CHUNK_SIZE);
        vec3s grid_scaled = glms_vec3_scale(grid_pos, CHUNK_SIZE);
        memcpy(&wrld.chunks[i].chunk_pos, &grid_scaled, sizeof(vec3s));
        naive_mesh(&wrld.chunks[i], CHUNK_SIZE);
        make_bounding_box(&wrld.chunks[i]);
        make_voxel_bounding_spheres(&wrld.chunks[i], CHUNK_SIZE);

        if (world_x <= WORLD_SIZE ) {
            world_x += 1.0f;
        } else {
            world_z += 1.0f;
            world_x = 0.0f;
        }
    }
}

