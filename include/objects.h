#ifndef OBJECTS_H
#define OBJECTS_H

class LightSource3d {
public:
    LightSource3d() : LightSource3d(Point3d(), Color()) {
    }

    LightSource3d(const Point3d &location, const Color &color)
        : location_world(location), location(location), color(color) {
    }

    Point3d location_world; // Absolute position
    Point3d location; // Projection position
    Color color;
};

class Material {
public:
    Material() : Material(1., 1., 0., 0., 0., 0.) {
    }

    Material(const Float &Ka, const Float &Kd, const Float &Ks,
             const Float &Kr, const Float &Kt, const Float &p) : p(p) {
        Float sum = Ka + Kd + Ks + Kr + Kt;
        this->Ka = Ka / sum;
        this->Kd = Kd / sum;
        this->Ks = Ks / sum;
        this->Kr = Kr / sum;
        this->Kt = Kt / sum;
    }

    Float Ka; // ambient
    Float Kd; // diffuse
    Float Ks; // specular
    Float Kr; // reflection
    Float Kt; // transparency

    Float p; // specular * lisht_source_color * ((cos(..))^p)
};

class Object3d {
public:
    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                   Point3d * const intersection_point) const {
        return intersect_(data, vector_start, vector, intersection_point);
    }
    virtual Color get_color(const Point3d &intersection_point) const {
        return get_color_(data, intersection_point);
    }
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const {
        return get_normal_vector_(data, intersection_point);
    }
    virtual Material get_material(const Point3d &intersection_point) const {
        return get_material_(data, intersection_point);
    }
    virtual Point3d get_min_boundary_point() const {
        return get_min_boundary_point_(data);
    }
    virtual Point3d get_max_boundary_point() const {
        return get_max_boundary_point_(data);
    }

    void* data;

//protected:
    bool (*intersect_)(const void * data,
                         const Point3d vector_start,
                         const Vector3d vector,
                         Point3d * const intersection_point);

    Color (*get_color_)(const void * data,
                       const Point3d intersection_point);

    Vector3d (*get_normal_vector_)(const void * data,
                                  const Point3d intersection_point);

    Material (*get_material_)(const void * data,
                             const Point3d intersection_point);

    Point3d (*get_min_boundary_point_)(const void * data);

    Point3d (*get_max_boundary_point_)(const void * data);

    void (*release_data_)(void * data);
};

#endif // OBJECTS_H
/*
*/
