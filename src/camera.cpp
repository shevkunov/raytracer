#include <stdlib.h>
#include <string.h>

#include <include/scene.h>

Camera::Camera(const Point3d &position, const Float &al_x,
       const Float &al_y, const Float &al_z, const Float &proj_plane_dist)
        : position(position),
          al_x(al_x), sin_al_x(sin(al_x)), cos_al_x(cos(al_x)),
          al_y(al_y), sin_al_y(sin(al_y)), cos_al_y(cos(al_y)),
          al_z(al_z), sin_al_z(sin(al_z)), cos_al_z(cos(al_z)),
          proj_plane_dist(proj_plane_dist) {
}

void Camera::rotate(const Float &al_x, const Float &al_y, const Float &al_z) {
    if(fabs(al_x) > EPSILON) {
        this->al_x += al_x;
        sin_al_x = sin(al_x);
        cos_al_x = cos(al_x);
    }

    if(fabs(al_y) > EPSILON) {
        this->al_y += al_y;
        sin_al_y = sin(al_y);
        cos_al_y = cos(al_y);
    }

    if(fabs(al_z) > EPSILON) {
        this->al_z += al_z;
        sin_al_z = sin(al_z);
        cos_al_z = cos(al_z);
    }
}

void Camera::move_camera(const Vector3d &vector) {
    Vector3d r_vector = vector.rotate_x(sin_al_x, cos_al_x);
    r_vector = r_vector.rotate_z(sin_al_z, cos_al_z);
    r_vector = r_vector.rotate_y(sin_al_y, cos_al_y);

    Point3d curr_pos = position;

    position = Point3d(curr_pos.x + r_vector.x,
                       curr_pos.y + r_vector.y,
                       curr_pos.z + r_vector.z);
}
