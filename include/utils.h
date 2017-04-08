#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>

typedef double Float;
const Float EPSILON = 1e-5;
#define FLOAT_MAX DBL_MAX

template <typename T>
inline T fast_max(const T &a, const T &b, const T &c) {
    T ret = a;
    if (ret < b) {
        ret = b;
    }
    if (ret < c) {
        ret = c;
    }
    return ret;
}

template <typename T>
inline T fast_min(const T &a, const T &b, const T &c) {
    T ret = a;
    if (ret > b) {
        ret = b;
    }
    if (ret > c) {
        ret = c;
    }
    return ret;
}

template <typename Type>
inline Type max(Type a) {
    static Type mx = a;
    if (mx < a) {
        mx = a;
    }
    return mx;
}

template <typename T, typename ... Type>
inline T max(T a, Type ... args) {
    T temp = max(args...);
    return (temp > a ? temp : a);
}

template <typename Type>
inline Type min(Type a) {
    static Type mn = a;
    if (mn > a) {
        mn = a;
    }
    return mn;
}

template <typename T, typename ... Type>
inline T min(T a, Type ... args) {
    T temp = min(args...);
    return (temp < a ? temp : a);
}


class Point2d {
public:
    Point2d() : Point2d(0., 0.0) {
    }

    Point2d(const Float &x, const Float &y) : x(x), y(y) {
    }

    Float x;
    Float y;
};

//class Point3d;

class Vector3d {
public:
    Vector3d() : Vector3d(0., 0., 0.) {
    }

    Vector3d(const Float &x, const Float &y, const Float &z) :
        x(x), y(y), z(z) {
    }

    Vector3d(const Vector3d &start, const Vector3d &end);

    Vector3d cross(const Vector3d &other) const;
    Float dot(const Vector3d &other) const;
    Float cos(const Vector3d &other) const;

    static Vector3d cross(const Vector3d &a, const Vector3d &b);
    static Float dot(const Vector3d &v1, const Vector3d &v2);
    static Float cos(const Vector3d &v1, const Vector3d &v2);

    static bool check_same_clock_dir(const Vector3d &v1, const Vector3d &v2,
                                     const Vector3d &norm);

    Vector3d reflect(const Vector3d &norm) const;
    Vector3d mul(const Float& k) const;


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

Vector3d operator+(const Vector3d &a, const Vector3d &b);
Vector3d operator-(const Vector3d &a, const Vector3d &b);

typedef Vector3d Point3d;


/*
class Point3d : public Vector3d {
public:
    Point3d() : Vector3d(0., 0., 0.) {
    }

    Point3d(const Vector3d &v) : Vector3d(v) {
    }

    Point3d(Vector3d &&v) : Vector3d(v) {
    }

    Point3d(const Float &x, const Float &y, const Float &z) :
        Vector3d(x, y, z) {
    }
};
*/

#endif //__UTILS_H__
