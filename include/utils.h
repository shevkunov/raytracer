#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>

typedef double Float;
const Float EPSILON = 1e-5;
#define FLOAT_MAX DBL_MAX

class Point2d {
public:
    Point2d() : Point2d(0., 0.0) {
    }

    Point2d(const Float &x, const Float &y) : x(x), y(y) {
    }

    Float x;
    Float y;
};

class Vector3d;

class Point3d {
public:
    Point3d() : Point3d(0., 0., 0.) {
    }

    Point3d(const Float &x, const Float &y, const Float &z) :
        x(x), y(y), z(z) {
    }

    Point3d rotate_x(const Float sin_al, const Float cos_al) const;
    Point3d rotate_y(const Float sin_al, const Float cos_al) const;
    Point3d rotate_z(const Float sin_al, const Float cos_al) const;

    Vector3d to_vector() const;

    Float x;
    Float y;
    Float z;
};

class Vector3d {
public:
    Vector3d() : Vector3d(0., 0., 0.) {
    }

    Vector3d(const Float &x, const Float &y, const Float &z) :
        x(x), y(y), z(z) {
    }

    Vector3d(const Point3d &start, const Point3d &end);

    Vector3d cross(const Vector3d &other) const;
    Float dot(const Vector3d &other) const;
    Float cos(const Vector3d &other) const;

    static Vector3d cross(const Vector3d &a, const Vector3d &b);
    static Float dot(const Vector3d &v1, const Vector3d &v2);
    static Float cos(const Vector3d &v1, const Vector3d &v2);

    Vector3d reflect(const Vector3d &norm) const;

    Vector3d rotate_x(const Float &sin_al, const Float &cos_al) const;
    Vector3d rotate_y(const Float &sin_al, const Float &cos_al) const;
    Vector3d rotate_z(const Float &sin_al, const Float &cos_al) const;

    Float module2() const;
    Float module() const;

    void normalize();

    Float x;
    Float y;
    Float z;
};

#endif //__UTILS_H__
