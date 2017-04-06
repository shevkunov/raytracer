#include <include/utils.h>

Vector3d::Vector3d(const Point3d &start, const Point3d &end) :
    Vector3d(end.x - start.x, end.y - start.y, end.z - start.z) {
}

Vector3d Vector3d::cross(const Vector3d &other) const {
    return Vector3d(z * other.y - y * other.z,
                    x * other.z - z * other.x,
                    y * other.x - x * other.y);
}

Float Vector3d::dot(const Vector3d &other) const {
    return x * other.x + y * other.y + z * other.z;
}

Float Vector3d::cos(const Vector3d &other) const {
    return dot(other) / sqrt(module2() * other.module2());
}

Vector3d Vector3d::cross(const Vector3d &a, const Vector3d &b) {
    return Vector3d(a.z * b.y - a.y * b.z,
                    a.x * b.z - a.z * b.x,
                    a.y * b.x - a.x * b.y);
}

Float Vector3d::dot(const Vector3d &v1, const Vector3d &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Float Vector3d::cos(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d::dot(v1, v2) / sqrt(v1.module2() * v2.module2());
}

bool Vector3d::check_same_clock_dir(const Vector3d &v1, const Vector3d &v2,
                                    const Vector3d &norm) {
    const Vector3d norm_v1_v2 = Vector3d::cross(v2, v1);
    return !(Vector3d::dot(norm_v1_v2, norm) < 0);
}

Float Vector3d::module2() const {
    return x * x + y * y + z * z;
}

Float Vector3d::module() const {
    return sqrt(module2());
}

void Vector3d::normalize() {
    const Float module = this->module();
    x /= module;
    y /= module;
    z /= module;
}

Vector3d Vector3d::reflect(const Vector3d &norm) const {
    const Float k = 2. * dot(norm) / norm.module2();
    return Vector3d(x - norm.x * k,
                    y - norm.y * k,
                    z - norm.z * k);
}

Vector3d Vector3d::mul(const Float& k) const {
    return Vector3d(x * k, y * k, z * k);
}

Vector3d Vector3d::rotate_x(const Float &sin_al, const Float &cos_al) const {
    return Vector3d(x,
                    y * cos_al - z * sin_al,
                    y * sin_al + z * cos_al);
}

Vector3d Vector3d::rotate_y(const Float &sin_al, const Float &cos_al) const {
    return Vector3d(x * cos_al - z * sin_al,
                    y,
                    x * sin_al + z * cos_al);
}

Vector3d Vector3d::rotate_z(const Float &sin_al, const Float &cos_al) const {
    return Vector3d(x * cos_al - y * sin_al,
                    x * sin_al + y * cos_al,
                    z);
}

Vector3d operator+(const Vector3d &a, const Vector3d &b) {
    return Vector3d(a.x + b.x, a.y + b.y, a.z + b.z);
}
