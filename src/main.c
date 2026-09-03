#include "winrend.h"
#include "vixx.h"

const uint32_t max_fps = 60;
const uint32_t frame_delay = 1000 / max_fps;

int main(void) {
    if (window_setup()) 
        return 1;

    shader_interface shader_interface = {0};
    camera_init();
    shader_program_create(&shader_interface);
    xh_prep_crosshair();
    
    printf("Building Chunks...\n");
    vx_world_clanker_chunker();
    printf("Building Buffers for Chunks...\n");
    for (size_t i = 0; i < GRID_AREA; ++i)
        vixx_mesh_buffer_create(&wrld.chunks[i]);
    printf("Chunks Built! Firing up world.\n");

    timer timer = {0};
    while (!window_should_close()) { 
        Uint32 start_frame = SDL_GetTicks();

        event_poll();
        timer_update(&timer);

        camera_move_speed_update(camera, timer);
        camera_move();
        camera_point();

        draw(&shader_interface);
        window_swap();

        Uint32 end_frame = SDL_GetTicks() - start_frame;
        if (frame_delay > end_frame)
            SDL_Delay(frame_delay - end_frame);
    }

    close_window();
    return 0;
}
