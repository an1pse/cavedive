#ifndef VIXX_H
#define VIXX_H

#include "nob.h"
#include "stb_perlin.h"

#define CHUNK_SIZE 64
#define WORLD_SIZE 8
#define GRID_AREA WORLD_SIZE*WORLD_SIZE

typedef enum {
    AIR,
    PROVISION,
    SOLID
} block_type;

#if 0
typedef struct {
    float *items;
    size_t count;
    size_t capacity;
} float_array;

typedef struct {
    bool *grid;
    vec3s grid_pos;
} vx_mesh_maker2;
#endif

typedef struct {
    vec3s min;
    vec3s max;
} vx_bounding_box;

typedef struct {
    vec4s sphere;
    size_t location;
} vx_sphere;

typedef struct {
    block_type *mesh_data; // to be used for later if we want to remove/add blocks
    struct {
        float *items;
        size_t count;
        size_t capacity;
    } mesh;
    struct {
        vx_sphere *items;
        size_t count;
        size_t capacity;
    } spheres;
    vx_bounding_box bounding_box;
    GLuint VAO, VBO;
    vec3s chunk_pos;
} vx_chunk;

typedef struct {
    vx_chunk *chunks;
} vx_world;

#ifdef VIXX_SHADERS

static const char *vertex_shader_source = // modified for voxels
"#version 410 core\n"
"layout (location = 0) in vec3 vertex_pos;\n"
"layout (location = 1) in vec3 voxel_normal;\n"
"layout (location = 2) in vec3 voxel_pos;\n"
"out vec3 frag_pos;\n"
"out vec3 normal;\n"
"out vec3 player_light;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform vec3 cam_pos;\n"

"mat4 translate(vec3 offset) {\n"
"   return mat4(\n"
"       vec4(1.0f, 0.0f, 0.0f, 0.0f),\n"
"       vec4(0.0f, 1.0f, 0.0f, 0.0f),\n"
"       vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
"       vec4(offset, 1.0f)\n"
"   );\n"
"}\n"

"void main()\n"
"{\n"
"   mat4 model = translate(voxel_pos);\n"
"   gl_Position = projection * view * model * vec4(vertex_pos, 1.0f);\n"
"   frag_pos = vec3(model * vec4(vertex_pos, 1.0f));\n"
"   normal = mat3(transpose(inverse(model))) * voxel_normal;\n"
"   player_light = cam_pos;\n"
//"   inv_view = inverse(view);\n"
"}\0";

static const char *lantern = 
"#version 410 core\n"
"out vec4 frag_color;\n"

"in vec3 frag_pos;\n"
"in vec3 normal;\n"
"in vec3 player_light;\n"

"void main() {\n"
"   vec3 gruvbox_orange = vec3(0.2f, 0.2f, 0.2f);\n"
"   vec3 light = vec3(1.0f, 1.0f, 1.0f);\n"
"   vec3 light_pos = player_light;\n"
// ambient
"   float ambient_strength = 0.5f;\n"
"   vec3 ambient = light * ambient_strength;\n"
// diffuse
"   vec3 norm = normalize(normal);\n"
"   vec3 light_dir = normalize(light_pos - frag_pos);\n"
"   float diff = max(dot(norm, light_dir), 0.0);\n"
"   vec3 diffuse = diff * light;\n"
// attenuation
"   float light_constant = 1.0f;\n"
"   float light_linear = 0.014f;\n"
"   float light_quadratic = 0.0007f;\n"

"   float distance = length(light_pos - frag_pos);\n"
"   float attenuation = 1.0f / (light_constant + light_linear * distance + light_quadratic * (distance * distance));\n"
"   ambient *= attenuation;\n"
"   diffuse *= attenuation;\n"
"   vec3 result = (ambient + diffuse) * gruvbox_orange;\n"
"   frag_color = vec4(result, 1.0f);\n"
"}\0";

static const char *spotlight =
"#version 410 core\n"
"in vec3 normal;\n"
"in vec3 frag_pos;\n"
"in vec3 player_light;\n"
"out vec4 frag_color;\n"

"uniform vec3 light_direction;\n"

"void main()\n"
"{\n"
"   vec3 gruvbox_orange = vec3(0.2f, 0.2f, 0.2f);\n"
"   vec3 light = vec3(1.0f, 1.0f, 1.0f);\n"
"   float light_constant = 1.0f;\n"
"   float light_linear = 0.014f;\n"
"   float light_quadratic = 0.0007f;\n"
"   float cut_off = cos(radians(32.5f));\n"
"   float out_cut_off = cos(radians(37.5f));\n"

// Ambient Lighting
"   float ambient_strength = 0.1f;\n"
"   vec3 ambient = light * ambient_strength;\n"

// Diffuse Lighting
"   vec3 norm = normalize(normal);\n"
"   vec3 light_dir = normalize(player_light - frag_pos);\n"
"   float diff = max(dot(norm, light_dir), 0.0f);\n"
"   vec3 diffuse = diff * light;\n"
#if 0
// Specular Lighting
"   float specular_strength = 0.5f;\n"
"   vec3 view_dir = normalize(player_light - frag_pos);\n"
"   vec3 reflect_dir = reflect(-light_dir, norm);\n"
"   float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);\n"
"   vec3 specular = specular_strength * spec * light;\n"
#endif

// Soft edges
"   float theta = dot(light_dir, normalize(-light_direction));\n"
"   float epsilon = (cut_off - out_cut_off);\n"
"   float intensity = clamp((theta - out_cut_off) / epsilon, 0.0, 1.0);\n"
"   diffuse *= intensity;\n"
//"   specular *= intensity;\n"

"   float distance = length(player_light - frag_pos);\n"
"   float attenuation = 1.0 / (light_constant + light_linear * distance + light_quadratic * (distance * distance));\n"

"   ambient *= attenuation;\n"
"   diffuse *= attenuation;\n"
//"   specular *= attenuation;\n"

//"   vec3 result = (ambient + diffuse + specular) * gruvbox_orange;\n"
"   vec3 result = (ambient + diffuse) * gruvbox_orange;\n"
"   frag_color = vec4(result, 1.0f);\n"
"}\0";

#endif // VIXX_SHADERS

extern const float vertices[];
extern vx_world wrld;
extern size_t current_chunk;

//extern void vx_world_grand_chunker();
extern void vx_world_clanker_chunker();

// Meshing Functions:
extern void vixx_mesh_buffer_create(vx_chunk *chunk);
extern void vixx_mesh_buffer_destroy();
extern void vixx_mesh_draw();

extern bool is_in_current_chunk(vx_bounding_box bb);
extern void shoot_ray(vx_chunk *chunk);
extern void edit_chunk(vx_chunk *chunk, SDL_MouseButtonEvent mv);
extern bool surrounded(block_type *grid, int x, int y, int z, int size);
extern void naive_mesh(vx_chunk *chunk, int sq_dim);

#endif // VIXX_H
