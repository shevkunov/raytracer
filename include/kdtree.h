#ifndef __KD_TREEE_H__
#define __KD_TREEE_H__

#include <include/objects.h>


class KDTree {
public:
    KDTree(std::vector<Object3d *> &objects);
    bool find_intersection_tree(const Point3d vector_start,
                                const Vector3d vector,
                                Object3d *&nearest_obj_ptr,
                                Point3d &nearest_intersection_point_ptr,
                                Float &nearest_intersection_point_dist_ptr);
    ~KDTree();

private:
    enum Plane {XY, XZ, YZ, NONE};

    class Voxel {
    public:
        Voxel(): x_min(0.), y_min(0.), z_min(0.),x_max(0.), y_max(0.), z_max(0.) {
        }

        Voxel(const Float &x_min, const Float &y_min, const Float &z_min,
              const Float &x_max, const Float &y_max, const Float &z_max)
        : x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max) {
        }

        Voxel(const std::vector<Object3d*> &objects);

        bool is_in(Object3d * const obj) const;
        bool is_in(const Point3d p) const;

        int count_objects_in(const std::vector<Object3d*> &objects) const;

        void find_plane(const std::vector<Object3d*> &objects,
                        const int tree_depth,
                        enum Plane &p,
                        Point3d &c) const;

        void split(const enum Plane p, const Point3d c,
                         Voxel &vl, Voxel &vr) const;

        inline bool x_in_voxel(const Float x) const;
        inline bool y_in_voxel(const Float y) const;
        inline bool z_in_voxel(const Float z) const;
        bool intersection(const Vector3d vector,
                                const Point3d vector_start) const;


        static bool vector_plane_intersection(const Vector3d vector,
                                              const Point3d vector_start,
                                              const enum Plane plane,
                                              const Point3d coord,
                                              Point3d &result);

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

        KDNode(enum Plane plane, const Point3d &coord,
               const std::vector<Object3d*> &objects, KDNode *l, KDNode *r)
            : plane(plane), coord(coord), objects(objects), l(l), r(r) {
        }

        KDNode(const std::vector<Object3d*> &objects);

        ~KDNode();
        enum Plane plane;
        Point3d coord;
        std::vector<Object3d*> objects;
        KDNode *l;
        KDNode *r;
    };

    KDNode * root;
    Voxel bounding_box;

    bool find_intersection_node(KDNode * const node,
                                const Voxel v,
                                const Point3d vector_start,
                                const Vector3d vector,
                                Object3d *&nearest_obj_ptr,
                                Point3d &nearest_intersection_point_ptr,
                                Float &nearest_intersection_point_dist_ptr);

    static KDNode* rec_build(std::vector<Object3d*> &objects, Voxel v, int iter);
};




#endif
