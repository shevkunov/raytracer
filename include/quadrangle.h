#ifndef QUADRANGLE_H
#define QUADRANGLE_H

#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include <include/canvas.h>
#include <include/objects.h>
#include <include/utils.h>
#include <include/triangle.h>

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
    virtual bool reflects() const;
    virtual bool secondary_light(const Point3d &point, const LightSource3d &ls,
                                 LightSource3d & ls_secondary) const;

protected:
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

class TexturedQuadrangle3d : public Quadrangle3d{
public:
    TexturedQuadrangle3d(const Point3d &p1, const Point3d &p2,
                        const Point3d &p3, const Point3d &p4,
                        const Point2d &t1, const Point2d &t2,
                        const Point2d &t3, const Point2d &t4,
                        Canvas *texture, const Color &color,
                        const Material &material);

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d &intersection_point) const;

    virtual Color get_color(const Point3d &intersection_point) const;


protected:
    TexturedTriangle3d a, b;
};

#endif // QUADRANGLE_H
