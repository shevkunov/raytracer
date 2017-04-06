#include <include/quadrangle.h>
#include <include/triangle.h>
#include <include/utils.h>

Quadrangle3d::Quadrangle3d(const Point3d &p1, const Point3d &p2,
                           const Point3d &p3, const Point3d &p4,
                           const Color &color, const Material &material)
        : p1(p1), p2(p2), p3(p3), p4(p4),
          norm(Vector3d::cross(Vector3d(p1, p3), Vector3d(p3, p2))),
          color(color), material(material) {
    Vector3d norm2 = Vector3d::cross(Vector3d(p1, p4), Vector3d(p4, p3));
    if (Vector3d::cross(norm, norm2).module2() > EPSILON) {
        throw new std::runtime_error("bad quadrangle");
    }
    //d = (-(p1.x * norm.x + p1.y * norm.y + p1.z * norm.z));
}

bool Quadrangle3d::intersect(const Point3d &vector_start, const Vector3d &vector,
               Point3d &intersection_point) const {
    Triangle3d a(p1, p2, p3, color, material);
    Triangle3d b(p1, p3, p4, color, material);

    if (!a.intersect(vector_start, vector, intersection_point)) {
        return b.intersect(vector_start, vector, intersection_point);
    } else {
        return true;
    }
}

Color Quadrangle3d::get_color(const Point3d &intersection_point) const {
    (void)intersection_point;
    return color;
}

Vector3d Quadrangle3d::get_normal_vector(const Point3d &intersection_point) const {
    (void)intersection_point;
    return norm;
}

Material Quadrangle3d::get_material(const Point3d &intersection_point) const {
    (void)intersection_point;
    return material;
}

Point3d Quadrangle3d::get_min_boundary_point() const {
    Float x_min = std::min(fast_min(p1.x, p2.x, p3.x), p4.x);
    Float y_min = std::min(fast_min(p1.y, p2.y, p3.y), p4.y);
    Float z_min = std::min(fast_min(p1.z, p2.z, p3.z), p4.z);

    return Point3d(x_min - EPSILON, y_min - EPSILON, z_min - EPSILON);
}

Point3d Quadrangle3d::get_max_boundary_point() const {
    Float x_max = std::max(fast_max(p1.x, p2.x, p3.x), p4.x);
    Float y_max = std::max(fast_max(p1.y, p2.y, p3.y), p4.y);
    Float z_max = std::max(fast_max(p1.z, p2.z, p3.z), p4.z);

    return Point3d(x_max + EPSILON, y_max + EPSILON, z_max + EPSILON);
}
