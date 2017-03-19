#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include <include/render.h>
#include <include/utils.h>
#include <include/color.h>
#include <include/kdtree.h>

// Declarations
// --------------------------------------------------------------

static inline void
rebuild_kd_tree(Scene * scene);

// Code
// --------------------------------------------------------------

Scene *
new_scene(const int objects_count,
          const int light_sources_count,
          const Color background_color) {
    
    Scene * s = reinterpret_cast<Scene*>(malloc(sizeof(Scene)));
    s->objects_count=objects_count;
    s->objects = reinterpret_cast<Object3d**>(calloc(objects_count, sizeof(Object3d *)));
    if(light_sources_count) {
        s->light_sources = reinterpret_cast<LightSource3d**>(calloc(light_sources_count, sizeof(LightSource3d *)));
    }
    s->light_sources_count = light_sources_count;
    s->background_color = background_color;
    s->last_object_index = -1;
    s->last_light_source_index = -1;
    s->fog_parameters = NULL;
    s->fog_density = NULL;
    
    s->kd_tree = NULL;
    return s;
}

void
release_scene(Scene * scene) {
    int i;
    
    for(i = 0; i < scene->objects_count; i++) {
        if(scene->objects[i]) {
            release_object3d(scene->objects[i]);
        }
    }
    
    for(i = 0; i < scene->light_sources_count; i++) {
        if(scene->light_sources[i]) {
            free(scene->light_sources[i]);
        }
    }
    
    free(scene->objects);
    free(scene->light_sources);
    
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    
    if(scene->kd_tree)
        release_kd_tree(scene->kd_tree);
    free(scene);
}

void
add_object(Scene * const scene,
           Object3d * const object) {
    
    scene->objects[++scene->last_object_index] = object;
}

void
prepare_scene(Scene * const scene) {
    rebuild_kd_tree(scene);    
}

void
add_light_source(Scene * const scene,
                 LightSource3d * const light_source) {
    
    scene->light_sources[++scene->last_light_source_index] = light_source;
}

static inline void
rebuild_kd_tree(Scene * scene) {
    if(scene->kd_tree)
        release_kd_tree(scene->kd_tree);
    scene->kd_tree = build_kd_tree(scene->objects, scene->last_object_index + 1);
}

void
release_object3d(Object3d * obj) {
    free(obj);
}


Camera *
new_camera(const Point3d camera_position,
           const Float al_x,
           const Float al_y,
           const Float al_z,
           const Float proj_plane_dist) {
    
    
    Camera * cam = reinterpret_cast<Camera*>(malloc(sizeof(Camera)));
    
    cam->camera_position = camera_position;
    
    cam->al_x = al_x;
    cam->sin_al_x = sin(al_x);
    cam->cos_al_x = cos(al_x);
    
    cam->al_y = al_y;
    cam->sin_al_y = sin(al_y);
    cam->cos_al_y = cos(al_y);
    
    cam->al_z = al_z;
    cam->sin_al_z = sin(al_z);
    cam->cos_al_z = cos(al_z);
    
    cam->proj_plane_dist = proj_plane_dist;
    
    return cam;
}

void
release_camera(Camera * const cam) {
    free(cam);
}

void
rotate_camera(Camera * const cam,
              const Float al_x,
              const Float al_y,
              const Float al_z) {
    
    if(fabs(al_x) > EPSILON) {
        cam->al_x += al_x;
        cam->sin_al_x = sin(cam->al_x);
        cam->cos_al_x = cos(cam->al_x);
    }
    
    if(fabs(al_y) > EPSILON) {
        cam->al_y += al_y;
        cam->sin_al_y = sin(cam->al_y);
        cam->cos_al_y = cos(cam->al_y);
    }
    
    if(fabs(al_z) > EPSILON) {
        cam->al_z += al_z;
        cam->sin_al_z = sin(cam->al_z);
        cam->cos_al_z = cos(cam->al_z);
    }
}

void
move_camera(Camera * const camera,
            const Vector3d vector) {
    
    Vector3d r_vector = vector.rotate_x(camera->sin_al_x, camera->cos_al_x);
    r_vector = r_vector.rotate_z(camera->sin_al_z, camera->cos_al_z);
    r_vector = r_vector.rotate_y(camera->sin_al_y, camera->cos_al_y);
    
    Point3d curr_pos = camera->camera_position;
    
    camera->camera_position = Point3d(curr_pos.x + r_vector.x,
                                      curr_pos.y + r_vector.y,
                                      curr_pos.z + r_vector.z);    
}
