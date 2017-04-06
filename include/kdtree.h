#ifndef __KD_TREEE_H__
#define __KD_TREEE_H__

#include <include/objects.h>


// KD Tree
enum Plane {XY, XZ, YZ, NONE};

class Coord {
public:
    Coord() : x(0), y(0), z(0){
    }

    Float x;
    Float y;
    Float z;
};

class Voxel {
public:
    Voxel(): x_min(0.), y_min(0.), z_min(0.),x_max(0.), y_max(0.), z_max(0.) {
    }

    Voxel(const Float &x_min, const Float &y_min, const Float &z_min,
          const Float &x_max, const Float &y_max, const Float &z_max)
    : x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max) {
    }

    Voxel(const std::vector<Object3d*> &objects);

    bool is_in_voxel(Object3d * const obj) const;
    bool is_in_voxel(const Point3d p) const;

    int objects_in_voxel(const std::vector<Object3d*> &objects) const;

    void find_plane(const std::vector<Object3d*> &objects,
                    const int tree_depth,
                    enum Plane &p,
                    Coord &c) const;

    void split_voxel(const enum Plane p, const Coord c,
                     Voxel &vl, Voxel &vr) const;

    std::vector<Object3d*> filter_overlapped_objects(const std::vector<Object3d*> &objects) const;


    Float x_min;
    Float y_min;
    Float z_min;

    Float x_max;
    Float y_max;
    Float z_max;
};

class KDNode {
public:
    KDNode(): l(NULL), r(NULL) {
    }

    KDNode(enum Plane plane, const Coord &coord,
           const std::vector<Object3d*> &objects, KDNode *l, KDNode *r)
        : plane(plane), coord(coord), objects(objects), l(l), r(r) {
    }

    KDNode(const std::vector<Object3d*> &objects);

    ~KDNode();
    enum Plane plane;
    Coord coord;
    std::vector<Object3d*> objects;
    KDNode *l;
    KDNode *r;
};

class KDTree {
public:
    KDTree(std::vector<Object3d *> &objects);
    bool find_intersection_tree(const Point3d vector_start,
                                const Vector3d vector,
                                Object3d ** const nearest_obj_ptr,
                                Point3d * const nearest_intersection_point_ptr,
                                Float * const nearest_intersection_point_dist_ptr);

    static KDNode* rec_build(std::vector<Object3d*> &objects, Voxel v, int iter);
    ~KDTree();
    KDNode * root;
    Voxel bounding_box;
};




#endif
