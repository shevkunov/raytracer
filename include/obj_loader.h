#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include <cmath>
#include <include/color.h>
#include <include/queue.h>
#include <include/scene.h>

#include <fstream>


class SceneFaceHandler {
public:
    SceneFaceHandler(Scene * scene, Float scale,
                     Float dx, Float dy, Float dz,
                     Float al_x, Float al_y, Float al_z,
                     Color default_color, Material default_material)
        :scene(scene), scale(scale),
         dx(dx), dy(dy), dz(dz),
         sin_al_x(sin(al_x)), cos_al_x(cos(al_x)),
         sin_al_y(sin(al_y)), cos_al_y(cos(al_y)),
         sin_al_z(sin(al_z)), cos_al_z(cos(al_z)),
         default_color(default_color), default_material(default_material) {
    }

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

    std::vector<Point3d> vertexes;
    std::vector<Vector3d> norm_vectors;

    void load_obj(std::string filename);
    void scene_face_handler(Queue<Point3d> &vertexes, Queue<Vector3d> &norm_vectors);

    void parse_face(std::string &str);
    void parse_vertex(std::ifstream &in);
    void parse_norm_vector(std::ifstream &in);
    void parse_face_str(char *str, int * v_index, int * vt_index, int * vn_index);
};


//----------------------------------------------------



#endif
