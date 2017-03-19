#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <math.h>
#include <stdlib.h>

#include <include/render.h>
#include <include/canvas.h>
#include <include/utils.h>

class Triangle3d : public Object3d{
public:
    Triangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Color &color, const Material &material);
    Triangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Vector3d &n1, const Vector3d &n2, const Vector3d &n3,
             const Color &color, const Material &material);
    Triangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Point2d &t1,const Point2d &t2, const Point2d &t3,
             Canvas *texture, const Color &color, const Material &material);

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d * const intersection_point) const;

    ////////

    virtual Color get_color(const Point3d &intersection_point) const {
        return get_color_(this, intersection_point);
    }
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const {
        return get_normal_vector_(this, intersection_point);
    }

    //virtual Color get_color(const Point3d &intersection_point) const;
    //virtual Vector3d get_normal_vector(const Point3d &intersection_point) const;
    virtual Material get_material(const Point3d &intersection_point) const;
    virtual Point3d get_min_boundary_point() const;
    virtual Point3d get_max_boundary_point() const;

    Color (*get_color_)(const void * data, // TODO
                       const Point3d intersection_point);

    Vector3d (*get_normal_vector_)(const void * data, // TODO
                                  const Point3d intersection_point);

    void (*release_data_)(void * data);

    // vertexes
    Point3d p1;
    Point3d p2;
    Point3d p3;

    Vector3d norm;

    // distance to the center of coordinates
    Float d;

    Vector3d v_p1_p2;
    Vector3d v_p2_p3;
    Vector3d v_p3_p1;

    // material
    Color color;
    Material material;

    // texture
    Point2d t1;
    Point2d t2;
    Point2d t3;

    Canvas * texture;

    // normals
    Vector3d n1;
    Vector3d n2;
    Vector3d n3;
};

inline static bool
intersect_triangle(const void * data,
                   const Point3d vector_start,
                   const Vector3d vector,
                   Point3d * const intersection_point);

Point3d
get_min_triangle_boundary_point(const void * data);

Point3d
get_max_triangle_boundary_point(const void * data);

static inline Color
get_triangle_color(const void * data,
                   const Point3d intersection_point);

static inline Color
get_texture_color(const void * data,
                  const Point3d intersection_point);

static inline Vector3d
get_triangle_normal_vector(const void * data,
                           const Point3d intersection_point);

static inline Vector3d
get_phong_normal_vector(const void * data,
                        const Point3d intersection_point);

static inline Material
get_triangle_material(const void * data,
                      const Point3d intersection_point);

static inline void
release_triangle_data(void * data);

static inline bool
check_same_clock_dir(const Vector3d v1,
                     const Vector3d v2,
                     const Vector3d norm);

static inline void
get_weights_of_vertexes(const Triangle3d * const tr,
                        const Point3d intersection_point,
                        Float * const w1,
                        Float * const w2,
                        Float * const w3);

#endif // TRIANGLE_H
