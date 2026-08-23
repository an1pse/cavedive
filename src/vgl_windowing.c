#include "virgl.h"
#include "vixx.h"

static SDL_Window *window;
static SDL_GLContext context;
static int window_exit = 0;

extern int vgl_window_setup() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    window = SDL_CreateWindow("Vixx", WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_Log("Couldn't create context: %s\n", SDL_GetError());
        return 1;
    }

    if (!SDL_SetWindowRelativeMouseMode(window, true)) {
        SDL_Log("Couldn't capture mouse: %s\n", SDL_GetError());
        return 1;
    }

    int version = gladLoadGL(SDL_GL_GetProcAddress);
    if (!version) {
        fprintf(stderr, "Couldn't load OpenGL\n");
        return 1;
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    return 0;
}

extern void vgl_event_poll() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                window_exit = 1;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                camera.mouse_x += event.motion.xrel;
                camera.mouse_y += event.motion.yrel;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                edit_chunk(&wrld.chunks[current_chunk], event.button);
                break;
            default:
                break;
        }
    }
}

extern int vgl_window_should_close() {
    return window_exit;
}

extern void vgl_timer_update(vgl_timer *timer) {
    timer->current_frame = GET_SECONDS;
    timer->delta_time = timer->current_frame - timer->last_frame;
    timer->last_frame = timer->current_frame;
}

extern void vgl_window_swap() {
    SDL_GL_SwapWindow(window);
}

extern void vgl_close_window() {
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
