#ifndef QUADRANGLE_H
#define QUADRANGLE_H

#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include <include/canvas.h>
#include <include/objects.h>
#include <include/utils.h>

class Quadrangle3d : public Object3d{
public:
    Quadrangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3, const Point3d &p4,
             const Color &color, const Material &material);

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d &intersection_point) const;

    virtual Color get_color(const Point3d &intersection_point) const;
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const;
    virtual Material get_material(const Point3d &intersection_point) const;
    virtual Point3d get_min_boundary_point() const;
    virtual Point3d get_max_boundary_point() const;

private:
    // vertexes
    Point3d p1;
    Point3d p2;
    Point3d p3;
    Point3d p4;

    Vector3d norm;

    // material
    Color color;
    Material material;
};

#endif // QUADRANGLE_H
