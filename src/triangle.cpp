#include <include/triangle.h>

Triangle3d::Triangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
                       const Color &color, const Material &material)
        : p1(p1), p2(p2), p3(p3),
          norm(Vector3d::cross(Vector3d(p1, p3), Vector3d(p3, p2))),
          color(color), material(material),
          v_p1_p2(Vector3d(p1, p2)), v_p2_p3(Vector3d(p2, p3)),
          v_p3_p1(Vector3d(p3, p1)) {

    d = (-(p1.x * norm.x + p1.y * norm.y + p1.z * norm.z));
}

bool Triangle3d::intersect(const Point3d &vector_start, const Vector3d &vector,
               Point3d &intersection_point) const {
    const Float scalar_product = Vector3d::dot(norm, vector);

    if (fabs(scalar_product) < EPSILON) {
        // Ray is perpendicular to triangles normal vector (A, B, C)
        // it means that ray is parellel to triangle
        // so there is no intersection
        return false;
    }

    const Float k = - (norm.x * vector_start.x
                 + norm.y * vector_start.y
                 + norm.z * vector_start.z
                 + d)
            / scalar_product;

    if (k < EPSILON) {
        // Avoid intersection in the opposite direction
        return false;
    }

    // Intersection point
    const Float x = vector_start.x + vector.x * k;
    const Float y = vector_start.y + vector.y * k;
    const Float z = vector_start.z + vector.z * k;
    const Point3d ipt = Point3d(x, y, z);

    if (Vector3d::check_same_clock_dir(v_p1_p2, Vector3d(p1, ipt), norm)
       && Vector3d::check_same_clock_dir(v_p2_p3, Vector3d(p2, ipt), norm)
       && Vector3d::check_same_clock_dir(v_p3_p1, Vector3d(p3, ipt), norm)) {

        intersection_point = ipt;
        return true;
    }

    // No intersection
    return false;
}

Color Triangle3d::get_color(const Point3d &intersection_point) const {
    (void)intersection_point;
    return color;
}

Vector3d Triangle3d::get_normal_vector(const Point3d &intersection_point) const {
    (void)intersection_point;
    return norm;
}

Material Triangle3d::get_material(const Point3d &intersection_point) const {
    (void)intersection_point;
    return material;
}

Point3d Triangle3d::get_min_boundary_point() const {
    Float x_min = fast_min(p1.x, p2.x, p3.x);
    Float y_min = fast_min(p1.y, p2.y, p3.y);
    Float z_min = fast_min(p1.z, p2.z, p3.z);

    return Point3d(x_min - EPSILON, y_min - EPSILON, z_min - EPSILON);
}

Point3d Triangle3d::get_max_boundary_point() const {
    Float x_max = fast_max(p1.x, p2.x, p3.x);
    Float y_max = fast_max(p1.y, p2.y, p3.y);
    Float z_max = fast_max(p1.z, p2.z, p3.z);

    return Point3d(x_max + EPSILON, y_max + EPSILON, z_max + EPSILON);
}

void Triangle3d::get_weights_of_vertexes(const Point3d &intersection_point,
                                         Float &w1, Float &w2, Float &w3) const {
    const Vector3d v_p1_p = Vector3d(p1, intersection_point);
    const Vector3d v_p2_p = Vector3d(p2, intersection_point);
    const Vector3d v_p3_p = Vector3d(p3, intersection_point);

    const Float s1 = Vector3d::cross(v_p2_p, v_p2_p3).module();
    const Float s2 = Vector3d::cross(v_p3_p, v_p3_p1).module();
    const Float s3 = Vector3d::cross(v_p1_p, v_p1_p2).module();

    const Float s_sum = s1 + s2 + s3;

    w1 = s1 / s_sum;
    w2 = s2 / s_sum;
    w3 = s3 / s_sum;
}

TexturedTriangle3d::TexturedTriangle3d(const Point3d &p1, const Point3d &p2,
                                       const Point3d &p3, const Point2d &t1,
                                       const Point2d &t2, const Point2d &t3,
                                       Canvas *texture, const Color &color,
                                       const Material &material)
        : Triangle3d(p1, p2, p3, color, material), t1(t1), t2(t2),
          t3(t3), texture(texture) {
}

NormedTriangle3d::NormedTriangle3d(const Point3d &p1, const Point3d &p2, const Point3d &p3,
                       const Vector3d &n1, const Vector3d &n2, const Vector3d &n3,
                       const Color &color, const Material &material)
        : Triangle3d(p1, p2, p3, color, material), n1(n1), n2(n2), n3(n3) {
}


Color TexturedTriangle3d::get_color(const Point3d &intersection_point) const {
    // TODO UNCHECKED
    Float w1, w2, w3;

    get_weights_of_vertexes(intersection_point, w1, w2, w3);
        
    Float xf = w1 * t1.x + w2 * t2.x + w3 * t3.x;
    Float yf = w1 * t1.y + w2 * t2.y + w3 * t3.y;
    
    // transform xf and yf to values from interval [0..1]
    xf = (xf < 0) ? (xf - (int)xf) + 1 : (xf - (int)xf);
    yf = (yf < 0) ? (yf - (int)yf) + 1 : (yf - (int)yf);

    int x = (int)(xf * texture->width());
    int y = (int)(yf * texture->height());
    
    return texture->get_pixel(x, y);
}


Vector3d NormedTriangle3d::get_normal_vector(const Point3d &intersection_point) const {
    Float w1, w2, w3;
    
    get_weights_of_vertexes(intersection_point, w1, w2, w3);

    return Vector3d(w1 * n1.x + w2 * n2.x + w3 * n3.x,
                     w1 * n1.y + w2 * n2.y + w3 * n3.y,
                     w1 * n1.z + w2 * n2.z + w3 * n3.z);
}



