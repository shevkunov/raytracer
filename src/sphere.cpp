#include <include/sphere.h>

Sphere::Sphere(const Point3d &center, const Float &radius, const Color &color,
               const Material &material)
    : center(center), radius(radius),color(color), material(material) {
    this->data = this;
}

bool Sphere::intersect(const Point3d &vector_start, const Vector3d &vector,
               Point3d * const intersection_point) const {
    const Float a = vector.module2();

    const Float b = 2 * (vector.x * (vector_start.x - center.x)
                         + vector.y * (vector_start.y - center.y)
                         + vector.z * (vector_start.z - center.z));

    const Float c = center.to_vector().module2()
                    + vector_start.to_vector().module2()
                    - 2 * Vector3d::dot(vector_start.to_vector(), center.to_vector())
                    - radius * radius;

    const Float D = b * b - 4 * a * c;

    if(D < 0)
        return false;

    const Float sqrt_D = sqrt(D);
    const Float a_2 = 2 * a;

    const Float t1 = (-b + sqrt_D) / a_2;
    const Float t2 = (-b - sqrt_D) / a_2;

    const Float min_t = (t1 < t2) ? t1 : t2;
    const Float max_t = (t1 > t2) ? t1 : t2;

    const Float t = (min_t > EPSILON) ? min_t : max_t;

    if(t < EPSILON)
        return false;

    *intersection_point = Point3d(vector_start.x + t * vector.x,
                                  vector_start.y + t * vector.y,
                                  vector_start.z + t * vector.z);

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

void
release_sphere_data(void * data) {
    Sphere * sphere = reinterpret_cast<Sphere*>(data);
	free(sphere);
}
