#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <include/utils.h>


class Camera {
public:
    Camera(const Point3d &camera_position, const Float &al_x,
           const Float &al_y, const Float &al_z, const Float &proj_plane_dist);

    void rotate(const Float &al_x, const Float &al_y, const Float &al_z);
    void move_camera(const Vector3d &vector);

    Point3d position;

    Float al_x;
    Float sin_al_x;
    Float cos_al_x;

    Float al_y;
    Float sin_al_y;
    Float cos_al_y;

    Float al_z;
    Float sin_al_z;
    Float cos_al_z;

    Float proj_plane_dist;
};

#endif // CAMERA_H
