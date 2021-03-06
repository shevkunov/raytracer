#ifndef OBJECTS_H
#define OBJECTS_H

#include <include/utils.h>
#include <include/objects.h>
#include <include/color.h>

class LightSource3d {
public:
    LightSource3d() : LightSource3d(Point3d(), Color()) {
    }

    LightSource3d(const Point3d &location, const Color &color)
        : location(location), color(color) {
    }
    Point3d location;
    Color color;
};

class Material {
public:
    Material() : Material(1., 1., 0., 0., 0., 0.) {
    }

    Material(const Float &Ka, const Float &Kd, const Float &Ks,
             const Float &Kr, const Float &Kt, const Float &p) : IOR(1.), p(p) {
        Float sum = Ka + Kd + Ks + Kr + Kt;
        this->Ka = Ka / sum;
        this->Kd = Kd / sum;
        this->Ks = Ks / sum;
        this->Kr = Kr / sum;
        this->Kt = Kt / sum;
        this->IOR = 1.;
    }

    Material(const Float &Ka, const Float &Kd, const Float &Ks,
             const Float &Kr, const Float &Kt, const Float &p, const Float& IOR)
             : IOR(IOR), p(p) {
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

    Float IOR; // in_IOR/out_IOR

    Float p; // specular * lisht_source_color * ((cos(..))^p)
};

class Object3d {
public:
    virtual ~Object3d() {
    }

    virtual bool intersect(const Point3d &vector_start, const Vector3d &vector,
                           Point3d &intersection_point) const = 0;
    virtual Color get_color(const Point3d &intersection_point) const = 0;
    virtual Vector3d get_normal_vector(const Point3d &intersection_point) const = 0;
    virtual Material get_material(const Point3d &intersection_point) const = 0;
    virtual Point3d get_min_boundary_point() const = 0;
    virtual Point3d get_max_boundary_point() const = 0;
    virtual bool reflects() const = 0;
    virtual bool secondary_light(const Point3d &point, const LightSource3d &ls,
                                 LightSource3d &ls_secondary) const = 0;
};

#endif // OBJECTS_H

