#define CROSSHAIR_IMPLEMENTATION
#include "virgl.h"
#include "vixx.h"

extern void vixx_mesh_buffer_create(vx_chunk *chunk) {
    glGenVertexArrays(1, &chunk->VAO);
    glGenBuffers(1, &chunk->VBO);

    glBindVertexArray(chunk->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
    glBufferData(GL_ARRAY_BUFFER, chunk->mesh.count * sizeof(float), 
            chunk->mesh.items, GL_STATIC_DRAW);

    // Voxel Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

    // Voxel Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    // Voxel Postion
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
}

static bool planes_bb_check(vec4s *planes, vx_bounding_box bb) {
    for (size_t i = 0; i < 6; ++i) {
        vec3s plane = (vec3s){ planes[i].x, planes[i].y, planes[i].z };
        float d = planes[i].w;
        vec3s p_vertex = bb.min;

        if (plane.x >= 0.0f) p_vertex.x = bb.max.x; 
        if (plane.y >= 0.0f) p_vertex.y = bb.max.y; 
        if (plane.z >= 0.0f) p_vertex.z = bb.max.z;

        float out = glms_vec3_dot(plane, p_vertex) + d;
        if (out < 0.0f) return false;
    }
    return true;
}

// Override for vgl_draw
extern void vgl_draw(vgl_shader_interface *shader_interface) {
    const float z_near = 1.0f;
    const float z_far  = 100.0f;

    camera.transforms.projection = glms_perspective(camera.fovy, ASPECT_RATIO, z_near, z_far);
    vec3s center                 = glms_vec3_add(camera.position, camera.front);
    camera.transforms.view       = glms_lookat(camera.position, center, camera.up);

    vgl_update_frustum();

    glUniformMatrix4fv(shader_interface->uniforms.projection, 1, GL_FALSE, 
            (const GLfloat *)&camera.transforms.projection);
    glUniformMatrix4fv(shader_interface->uniforms.view, 1, GL_FALSE, 
            (const GLfloat *)&camera.transforms.view);

    GLint player_light = glGetUniformLocation(shader_interface->shader, "cam_pos");
    assert(player_light > -1);
    glUniform3fv(player_light, 1, (const GLfloat *)&camera.position);
#if 0
    GLint light_direction = glGetUniformLocation(shader_interface->shader, "light_direction");
    assert(light_direction > -1);
    glUniform3fv(light_direction, 1, (const GLfloat *)&camera.front);
#endif
    glClearColor(BLACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    xh_render();
    
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shader_interface->shader);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    for (size_t i = 0; i < GRID_AREA; ++i) {
        vx_bounding_box bb = wrld.chunks[i].bounding_box;
        bool planes_check  = planes_bb_check(camera.frustum.planes, bb);
        if (is_in_current_chunk(bb)) {
            current_chunk = i;
            shoot_ray(&wrld.chunks[i]);
        }

        if (planes_check) {
            glBindVertexArray(wrld.chunks[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, wrld.chunks[i].mesh.count/9);
        }
    } 
}
