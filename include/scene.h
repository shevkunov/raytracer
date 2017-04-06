#ifndef SCENE_H
#define SCENE_H

#include <include/objects.h>
#include <include/kdtree.h>
#include <include/color.h>
#include <include/camera.h>
#include <include/fog.h>
#include <include/canvas.h>


const bool ANTIALIASING = true;

class Scene {
public:
    Scene(const Color &background_color);
    ~Scene();

    void add_object(Object3d * const object);
    void prepare_scene();
    void set_exponential_fog(const Float &k);
    void set_no_fog();
    void add_light_source(LightSource3d * const light_source);
    void rebuild_kd_tree();
    void render(const Camera &camera, Canvas& canvas) const;

    // Tracer
    Color trace(const Camera &camera, const Vector3d &vector) const;
    size_t get_objects_count() const;

protected:
    std::vector<Object3d*> objects;
    std::vector<LightSource3d*> light_sources;
    Color background_color;
    KDTree *kd_tree;
    Fog *fog;

    Color trace_recursively(const Point3d &vector_start,
                            const Vector3d &vector, const Float &intensity,
                            const int recursion_level) const;

    bool is_viewable(const Point3d &target_point,
                     const Point3d &starting_point) const;

    Color get_lighting_color(const Point3d &point,
                             const Vector3d &norm_v) const;

    Color get_specular_color(const Point3d &point,
                             const Vector3d &reflected_ray, const Float &p) const;

    Color calculate_color(const Point3d &vector_start,
                          const Vector3d &vector, Object3d * const * obj_ptr,
                          const Point3d * const point_ptr, const Float * const dist_ptr,
                          const Float &intensity, const int recursion_level) const;

};


#endif // SCENE_H
