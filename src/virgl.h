#ifndef VIRGL_H
#define VIRGL_H

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_timer.h>
#include "glad/gl.h"
#include "cglm/struct.h"
#include <stdbool.h>

#define WIDTH 1366
#define HEIGHT 768
#define ASPECT_RATIO (float)WIDTH/HEIGHT

#define PI 3.141592f
#define to_radian(angle) (angle * PI/180.0f)
#define GET_SECONDS ((float)SDL_GetTicks()/1000.0f)

#define GRUVBOX_BG 40.0f/255.0f, 40.0f/255.0f, 40.0f/255.0f, 1.0f
#define GRUVBOX_BLUE 65.0f/255.0f, 133.0f/255.0f, 136.0f/255.0f, 1.0f
#define WHITE 255.0f, 255.0f, 255.0f, 1.0f
#define BLACK 0.0f, 0.0f, 0.0f, 1.0f

typedef struct {
    float current_frame;
    float delta_time;
    float last_frame;
} vgl_timer;

typedef struct {
    vec4s planes[6];
    vec4s corners[8];
} vgl_frustum;

typedef struct {
    bool firstmouse;
    vec3s position;
    vec3s front;
    vec3s up;
    vec3s right;
    vec3s world_up;
    float fovy;
    float yaw;
    float pitch;
    float move_speed;
    float mouse_sensitivity;

    float mouse_x;
    float mouse_y;
    float last_x;
    float last_y;

    struct {
        mat4s projection;
        mat4s view;
    } transforms;

    vgl_frustum frustum;
} vgl_camera2;

typedef struct {
    GLuint shader;
    GLuint VAO;
    struct {
        GLint projection;
        GLint view;
        GLint model; 
    } uniforms;
} vgl_shader_interface;

#ifdef CROSSHAIR_IMPLEMENTATION
const char *xh_vertex_shader =
"#version 410 core\n"
"layout (location = 0) in vec2 line;\n"

"void main() {\n"
"   gl_Position = vec4(line, 0.0f, 1.0f);\n"
"}\0";

const char *xh_fragment_shader =
"#version 410 core\n"
"out vec4 frag_color;"

"void main() {\n"
"   frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";

static const float crosshair[] = {
    0.03f, 0.0f,
   -0.03,  0.0f,
    0.0f,  0.03f,
    0.0f, -0.03f
};

static GLuint xh_vao;
static GLuint xh_vbo;
static GLuint xh_shader;

static void xh_shader_compile() {
    int success;
    char infoLog[1024];
    GLint vertex_shader, fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &xh_vertex_shader, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
        fprintf(stderr, "VERTEX SHADER COMPILATION ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &xh_fragment_shader, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
        fprintf(stderr, "FRAGMENT SHADER COMPILATION ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    xh_shader = glCreateProgram();
    glAttachShader(xh_shader, vertex_shader);
    glAttachShader(xh_shader, fragment_shader);
    glLinkProgram(xh_shader);
    glGetProgramiv(xh_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(xh_shader, 1024, NULL, infoLog);
        fprintf(stderr, "SHADER PROGRAM LINKING ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }
}

static void xh_buffer_create() {
    glGenVertexArrays(1, &xh_vao);
    glGenBuffers(1, &xh_vbo);

    glBindVertexArray(xh_vao);
    glBindBuffer(GL_ARRAY_BUFFER, xh_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair), crosshair, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

extern void xh_prep_crosshair() {
    xh_shader_compile();
    xh_buffer_create();
}

extern void xh_render() {
    glUseProgram(xh_shader);
    glBindVertexArray(xh_vao);
    glDrawArrays(GL_LINES, 0, 8);
}
#endif // CROSSHAIR_IMPLEMENTATION

extern vgl_camera2 camera;

#define vgl_camera_move_speed_update(cam, tmr) cam.move_speed = 10.0f * tmr.delta_time;
#define vgl_camera_lift_speed_update(cam, tmr) cam.lift_speed = 5.0f * tmr.delta_time;

extern void xh_prep_crosshair();
extern void xh_render();
extern void vgl_update_frustum();
extern void vgl_camera_init();
extern void vgl_camera_move();
extern void vgl_camera_point();
extern void vgl_timer_update(vgl_timer *timer);

extern int vgl_window_setup();
extern int vgl_window_should_close();
extern void vgl_event_poll();
extern void vgl_window_swap();
extern void vgl_close_window();

extern void vgl_shader_program_create(vgl_shader_interface *shader_interface);
extern void vgl_vao_create(vgl_shader_interface *shader_interface);
extern void vgl_texture_load(const char *filename, GLint *texture, int alpha);

// Overridable Functions
extern void vgl_draw(vgl_shader_interface *shader_interface);

#endif // VIRGL_H
