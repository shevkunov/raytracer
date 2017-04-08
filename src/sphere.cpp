#include <include/sphere.h>

Sphere::Sphere(const Point3d &center, const Float &radius, const Color &color,
               const Material &material)
    : center(center), radius(radius),color(color), material(material) {
}

bool Sphere::intersect(const Point3d &vector_start, const Vector3d &vector,
               Point3d &intersection_point) const {
    const Float a = vector.module2();

    const Float b = 2 * Vector3d::dot(vector, (vector_start - center));;

    const Float c = center.module2()
                    + vector_start.module2()
                    - 2 * Vector3d::dot(vector_start, center)
                    - radius * radius;

    const Float D = b * b - 4 * a * c;

    if (D < 0.) {
        return false;
    }

    const Float sqrt_D = sqrt(D);
    const Float a_2 = 2 * a;

    const Float t1 = (-b + sqrt_D) / a_2;
    const Float t2 = (-b - sqrt_D) / a_2;

    const Float min_t = (t1 < t2) ? t1 : t2;
    const Float max_t = (t1 > t2) ? t1 : t2;

    const Float t = (min_t > EPSILON) ? min_t : max_t;

    if (t < EPSILON) {
        return false;
    }

    intersection_point = vector_start + vector.mul(t);

    return true;
}

Color Sphere::get_color(const Point3d &intersection_point) const {
    (void)intersection_point;
    return color;
}

Vector3d Sphere::get_normal_vector(const Point3d &intersection_point) const {
    Vector3d v(center, intersection_point);
    v.normalize();
    return v;
}

Material Sphere::get_material(const Point3d &intersection_point) const {
    (void)intersection_point;
    return material;
}

Point3d Sphere::get_min_boundary_point() const {
    return Point3d(center.x - radius - 1., center.y - radius - 1.,
                   center.z - radius - 1.);
}

Point3d Sphere::get_max_boundary_point() const {
    return Point3d(center.x + radius + 1., center.y + radius + 1.,
                   center.z + radius + 1.);
}

bool Sphere::reflects() const {
    return material.Kr != 0;
}

bool Sphere::secondary_light(const Point3d &point, const LightSource3d &ls,
                             LightSource3d &ls_secondary) const {
    (void) point;
    (void) ls;
    (void) ls_secondary;
    return false;
}
