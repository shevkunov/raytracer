#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include <math.h>
#include <include/render.h>
#include <include/color.h>
#include <include/queue.h>

typedef
struct {
    Float scale;
    
    Float dx;
    Float dy;
    Float dz;
    
    Scene * scene;
    
    Float sin_al_x;
    Float cos_al_x;
    
    Float sin_al_y;
    Float cos_al_y;
    
    Float sin_al_z;
    Float cos_al_z;
    
    Color default_color;
    Material default_material;
}
SceneFaceHandlerParams;


void
load_obj(const char * filename,
         void (* face_handler)(Queue * vertexes,
                               Queue * norm_vectors,
                               void * args),
         void * args);

//----------------------------------------------------

void
scene_face_handler(Queue * vertexes,
                   Queue * norm_vectors,
                   void * arg);

static inline SceneFaceHandlerParams
new_scene_face_handler_params(Scene * scene,
                              Float scale,
                              Float dx,
                              Float dy,
                              Float dz,
                              Float al_x,
                              Float al_y,
                              Float al_z,
                              Color default_color,
                              Material default_material) {
    
    SceneFaceHandlerParams params;
    params.scene = scene;
    params.scale = scale;
    params.dx = dx;
    params.dy = dy;
    params.dz = dz;
    params.sin_al_x = sin(al_x);
    params.cos_al_x = cos(al_x);
    params.sin_al_y = sin(al_y);
    params.cos_al_y = cos(al_y);
    params.sin_al_z = sin(al_z);
    params.cos_al_z = cos(al_z);
    params.default_color = default_color;
    params.default_material = default_material;

    return  params;
}

#endif
