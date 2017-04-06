#ifndef SPHERE_H
#define SPHERE_H

#include <math.h>
#include <stdlib.h>

#include <include/utils.h>
#include <include/objects.h>

class Sphere : public Object3d {
public:
    Sphere(const Point3d &center, const Float &radius,
           const Color &color, const Material &material);

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d * const intersection_point) const;
    virtual Color get_color(const Point3d &intersection_point) const;
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const;
    virtual Material get_material(const Point3d &intersection_point) const;
    virtual Point3d get_min_boundary_point() const;
    virtual Point3d get_max_boundary_point() const;

    Point3d center;
    Float radius;
    Color color;
    Material material;
};


#endif // SPHERE_H
