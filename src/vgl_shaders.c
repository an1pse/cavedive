#include <assert.h>
#include "virgl.h"
#define VIXX_SHADERS
#include "vixx.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
    
extern void vgl_shader_program_create(vgl_shader_interface *shader_interface) {
    int success;
    char infoLog[1024];
    GLint vertex_shader, fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
        fprintf(stderr, "VERTEX SHADER COMPILATION ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &lantern, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
        fprintf(stderr, "FRAGMENT SHADER COMPILATION ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    shader_interface->shader = glCreateProgram();
    glAttachShader(shader_interface->shader, vertex_shader);
    glAttachShader(shader_interface->shader, fragment_shader);
    glLinkProgram(shader_interface->shader);
    glGetProgramiv(shader_interface->shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_interface->shader, 1024, NULL, infoLog);
        fprintf(stderr, "SHADER PROGRAM LINKING ERROR:\n%s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    glUseProgram(shader_interface->shader);
    shader_interface->uniforms.view = glGetUniformLocation(shader_interface->shader, "view");
    assert(shader_interface->uniforms.view > -1);
    shader_interface->uniforms.projection = glGetUniformLocation(shader_interface->shader, "projection");
    assert(shader_interface->uniforms.projection > -1);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

#if 0
extern void vgl_texture_load(const char *filename, GLint *texture, int alpha) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        if (alpha)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load image\n");
    }
    stbi_image_free(data);
}
#endif
