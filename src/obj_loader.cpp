#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <iostream>

#include <include/camera.h>
#include <include/color.h>
#include <include/utils.h>
#include <include/obj_loader.h>
#include <include/triangle.h>


// TODO: use LinkedList instead of arrays

void SceneFaceHandler::load_obj(std::string filename) {
    std::ifstream in(filename);
    std::string line;
    
    while (in >> line) {
        //std::cout << "readed = " << line; TODO REMOVE
        if ((line != "v") && (line != "vn") && (line != "f")) {
            continue;
        }
        
        if (line == "v") {
            parse_vertex(in);
        }
        
        if (line == "vn") {
            parse_norm_vector(in);
        }
        
        if (line == "f") {
            std::getline(in, line);
            parse_face(line);
        }
    }

}

void SceneFaceHandler::parse_vertex(std::ifstream &in) {
    Point3d v;
    in >> v.y >> v.z >> v.x;
    //std::cout << "V" << v.x << v.y << v.z; TODO REMOVE
    vertexes.push_back(v);
}

void SceneFaceHandler::parse_norm_vector(std::ifstream &in) {
    Vector3d v;
    in >> v.y >> v.z >> v.x;
    //std::cout << "VN" << v.x << v.y << v.z; TODO REMOVE
    norm_vectors.push_back(v);
}

void SceneFaceHandler::parse_face(std::string &str) {
    Queue<std::string> tokens;
    Queue<Point3d> q_vertexes;
    Queue<Vector3d> q_norm_vectors;

    std::string token = "";
    std::string sep = " \n";
    for (size_t i = 0; i < str.size(); ++i) {
        if (sep.find(str[i]) != sep.npos) {
            if (token != "") {
                tokens.push(token);
                token = "";
            }
        } else {
            token += str[i];
        }
    }
    if (token != "") {
        tokens.push(token);
    }

    int vertex_index = 0;
    int texture_index = 0;
    int norm_index = 0;
    while(!tokens.empty()) {
        token = tokens.pop();
        char cstr[token.length()];
        strcpy(cstr, token.c_str());
        parse_face_str(cstr, &vertex_index, &texture_index, &norm_index);

        q_vertexes.push(vertexes[vertex_index - 1]);
        
        if(norm_index > 0) {
            q_norm_vectors.push(norm_vectors[norm_index - 1]);
        }
    }
    
    scene_face_handler(q_vertexes, q_norm_vectors);
}

void SceneFaceHandler::parse_face_str(char * str, int * v_index,
                                      int * vt_index, int * vn_index) {
    int str_len = strlen(str);

    int i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *v_index = atoi(str);
    
    i++;
    if(i >= str_len)
        return;
    
    str += i;
    str_len = strlen(str);
    i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *vt_index = atoi(str);
    
    i++;
    if(i >= str_len)
        return;
    
    str += i;
    str_len = strlen(str);
    i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *vn_index = atoi(str);
}

void SceneFaceHandler::scene_face_handler(Queue<Point3d> &vertexes,
                                          Queue<Vector3d> &norm_vectors) {
    Point3d p_p1 = vertexes.pop();
    Point3d p_p2 = vertexes.pop();
    Point3d p_p3;
    
    Vector3d p_v1;
    Vector3d p_v2;
    Vector3d p_v3;
    
    bool vLoaded = false;
    if (norm_vectors.size() >= 2) {
        p_v1 = norm_vectors.pop();
        p_v2 = norm_vectors.pop();
        vLoaded = true;
    }

    Point3d p1;
    Point3d p2;
    Point3d p3;
    
    Vector3d v1;
    Vector3d v2;
    Vector3d v3;
    
    while(!vertexes.empty()) {
        p_p3 = vertexes.pop();
        if (!norm_vectors.empty()) {
            p_v3 = norm_vectors.pop();
        } else {
            vLoaded = false;
        }
        p1 = p_p1.rotate_x(sin_al_x, cos_al_x)
                 .rotate_y(sin_al_y, cos_al_y)
                 .rotate_z(sin_al_z, cos_al_z);
        
        p2 = p_p2.rotate_x(sin_al_x, cos_al_x)
                 .rotate_x(sin_al_y, cos_al_y)
                 .rotate_z(sin_al_z, cos_al_z);
        
        p3 = p_p3.rotate_x(sin_al_x, cos_al_x)
                 .rotate_y(sin_al_y, cos_al_y)
                 .rotate_z(sin_al_z, cos_al_z);
        
        if(vLoaded) {
            
            v1 = p_v1.rotate_x(sin_al_x, cos_al_x)
                     .rotate_y(sin_al_y, cos_al_y)
                     .rotate_z(sin_al_z, cos_al_z);
            
            v2 = p_v2.rotate_x(sin_al_x, cos_al_x)
                     .rotate_y(sin_al_y, cos_al_y)
                     .rotate_z(sin_al_z, cos_al_z);
            
            v3 = p_v3.rotate_x(sin_al_x, cos_al_x)
                     .rotate_y(sin_al_y, cos_al_y)
                     .rotate_z(sin_al_z, cos_al_z);
            
            scene->add_object(new NormedTriangle3d(Point3d(p1.x * scale + dx, p1.y * scale + dy, p1.z * scale + dz),
                                                   Point3d(p2.x * scale + dx, p2.y * scale + dy, p2.z * scale + dz),
                                                   Point3d(p3.x * scale + dx, p3.y * scale + dy, p3.z * scale + dz),
                                                   v1,
                                                   v2,
                                                   v3,
                                                   default_color,
                                                   default_material));
        } else {
            scene->add_object(new Triangle3d(Point3d(p1.x * scale + dx, p1.y * scale + dy, p1.z * scale + dz),
                                             Point3d(p2.x * scale + dx, p2.y * scale + dy, p2.z * scale + dz),
                                             Point3d(p3.x * scale + dx, p3.y * scale + dy, p3.z * scale + dz),
                                             default_color,
                                             default_material));
        }
        
        p_p2 = p_p3;
        p_v2 = p_v3;
    }
}
