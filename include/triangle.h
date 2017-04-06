#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cmath>
#include <cstdlib>

#include <include/canvas.h>
#include <include/objects.h>
#include <include/utils.h>

class Triangle3d : public Object3d{
public:
    Triangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Color &color, const Material &material);

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d &intersection_point) const;

    ////////

    virtual Color get_color(const Point3d &intersection_point) const;
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const;
    virtual Material get_material(const Point3d &intersection_point) const;
    virtual Point3d get_min_boundary_point() const;
    virtual Point3d get_max_boundary_point() const;

    virtual void get_weights_of_vertexes(const Point3d &intersection_point,
                                         Float &w1, Float &w2, Float &w3) const;

protected:
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

};

class NormedTriangle3d : public Triangle3d {
public:
    NormedTriangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Vector3d &n1, const Vector3d &n2, const Vector3d &n3,
             const Color &color, const Material &material);
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const;

protected:
    // normals
    Vector3d n1;
    Vector3d n2;
    Vector3d n3;
};

class TexturedTriangle3d : public Triangle3d {
public:
    TexturedTriangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
             const Point2d &t1,const Point2d &t2, const Point2d &t3,
             Canvas *texture, const Color &color, const Material &material);
    virtual Color get_color(const Point3d &intersection_point) const;

protected:
    // texture
    Point2d t1;
    Point2d t2;
    Point2d t3;

    Canvas * texture;
};

#endif // TRIANGLE_H
