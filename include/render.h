#ifndef __RENDER_H__
#define __RENDER_H__

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <include/color.h>
#include <include/canvas.h>
#include <include/utils.h>
#include <include/objects.h>

#ifdef RAY_INTERSECTIONS_STAT
long intersections_per_ray;
#endif // RAY_INTERSECTIONS_STAT

// KD Tree
enum Plane {XY, XZ, YZ, NONE};

typedef
union {
    Float x;
    Float y;
    Float z;
}
Coord;

typedef
struct {
    Float x_min;
    Float y_min;
    Float z_min;
    
    Float x_max;
    Float y_max;
    Float z_max;
}
Voxel;

typedef
struct KDNode {
    enum Plane plane;
    Coord coord;
    
    Object3d ** objects;
    int objects_count;
    
    struct KDNode * l;
    struct KDNode * r;
}
KDNode;

typedef
struct {
    KDNode * root;
    Voxel bounding_box;
}
KDTree;
//

typedef
struct {    
    // Array of pointers to 3d objects of scene
    Object3d ** objects;
    int objects_count;
    int last_object_index;
    
    KDTree * kd_tree;
    
    // Array of pointers to light sources
    LightSource3d ** light_sources;
    int light_sources_count;
    int last_light_source_index;
    
    Color background_color;
    
    // Required to return value from interval [0..1]
    Float (*fog_density)(const Float distance,
                         const void * fog_parameters);    
    void * fog_parameters;
}
Scene;

typedef
struct {
    Point3d camera_position;
    
    Float al_x;
    Float sin_al_x;
    Float cos_al_x;
    
    Float al_y;
    Float sin_al_y;
    Float cos_al_y;
    
    Float al_z;
    Float sin_al_z;
    Float cos_al_z;
    
    Float proj_plane_dist;
}
Camera;

/***************************************************
 *                     Render                      *
 ***************************************************/

void
render_scene(const Scene * const scene,
             const Camera * const camera,
             Canvas& canvas);

/***************************************************
 *                     Scene                       *
 ***************************************************/

Scene *
new_scene(const int objects_count,
          const int light_sources_count,
          const Color background_color);

void
release_scene(Scene * scene);

void
add_object(Scene * const scene,
           Object3d * const object);

void
prepare_scene(Scene * const scene);

void
set_exponential_fog(Scene * const scene,
                    const Float k);

void
set_no_fog(Scene * const scene);

Color
trace(const Scene * const scene,
      const Camera * const camera,
      Vector3d vector);

void
add_light_source(Scene * const scene,
                 LightSource3d * const light_source);

/***************************************************
 *                    3D objects                   *
 ***************************************************/

Object3d *
new_triangle(const Point3d p1,
             const Point3d p2,
             const Point3d p3,
             const Color color,
             const Material material);

Object3d *
new_triangle_with_norms(const Point3d p1,
                        const Point3d p2,
                        const Point3d p3,
                        const Vector3d n1,
                        const Vector3d n2,
                        const Vector3d n3,
                        const Color color,
                        const Material material);

Object3d *
new_triangle_with_texture(const Point3d p1,
                          const Point3d p2,
                          const Point3d p3,
                          const Point2d t1,
                          const Point2d t2,
                          const Point2d t3,
                          Canvas * texture,
                          const Color color,
                          const Material material);

void
release_object3d(Object3d * obj);


/***************************************************
 *                     Camera                      *
 ***************************************************/

Camera *
new_camera(const Point3d camera_position,
           const Float al_x,
           const Float al_y,
           const Float al_z,
           const Float proj_plane_dist);

void
release_camera(Camera * const cam);

void
rotate_camera(Camera * const cam,
              const Float al_x,
              const Float al_y,
              const Float al_z);

void
move_camera(Camera * const camera,
            const Vector3d vector);

#endif //__RENDER_H__
