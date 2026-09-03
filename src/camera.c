#include "winrend.h"

camera2 camera = {0};

static void update_camera_vectors() {
    vec3s front;
    front.x = cosf(to_radian(camera.yaw)) * cosf(to_radian(camera.pitch));
    front.y = sinf(to_radian(camera.pitch));
    front.z = sinf(to_radian(camera.yaw)) * cosf(to_radian(camera.pitch));

    camera.front = glms_normalize(front);
    camera.right = glms_normalize(glms_vec3_cross(camera.front, camera.world_up));
    camera.up    = glms_normalize(glms_vec3_cross(camera.right, camera.front));
}

extern void update_frustum() {
    mat4s view_proj = glms_mat4_mul(camera.transforms.projection, camera.transforms.view);
    glms_frustum_planes(view_proj, camera.frustum.planes);
    glms_frustum_corners(glms_mat4_inv(view_proj), camera.frustum.corners);
}

extern void camera_init() {
    camera.position = (vec3s){ 0.0f, 18.0f, 3.0f };
    camera.world_up = (vec3s){ 0.0f, 1.0f, 0.0f };
    camera.yaw      = -90.0f;
    camera.pitch    = 0.0f;
    camera.fovy     = to_radian(50.0f);

    camera.mouse_x = 0.0f;
    camera.mouse_y = 0.0f;
    camera.last_x  = (float)WIDTH/2.0f;
    camera.last_y  = (float)HEIGHT/2.0f;

    update_camera_vectors();
}

extern void camera_move() {
    const bool *key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_W])
        camera.position = glms_vec3_add(camera.position, glms_vec3_scale(camera.front, camera.move_speed));
    if (key_states[SDL_SCANCODE_S])
        camera.position = glms_vec3_sub(camera.position, glms_vec3_scale(camera.front, camera.move_speed));
    if (key_states[SDL_SCANCODE_A])
        camera.position = glms_vec3_sub(camera.position, glms_vec3_scale(camera.right, camera.move_speed));
    if (key_states[SDL_SCANCODE_D])
        camera.position = glms_vec3_add(camera.position, glms_vec3_scale(camera.right, camera.move_speed));

    vec3s lift_factor = (vec3s){ 0.0f, camera.move_speed, 0.0f };
    if (key_states[SDL_SCANCODE_LCTRL]) 
        camera.position = glms_vec3_sub(camera.position, lift_factor); 
    else if (key_states[SDL_SCANCODE_SPACE])
        camera.position = glms_vec3_add(camera.position, lift_factor);
    
    update_camera_vectors();
}

extern void camera_point() {
    if (camera.firstmouse) {
        camera.last_x = camera.mouse_x;
        camera.last_y = camera.mouse_y;
        camera.firstmouse = false;
    }

    float xoffset = camera.mouse_x - camera.last_x;
    float yoffset = camera.last_y - camera.mouse_y;
    camera.last_x = camera.mouse_x;
    camera.last_y = camera.mouse_y;
    const float sens = 0.1f;
    xoffset *= sens;
    yoffset *= sens;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    update_camera_vectors();
}

