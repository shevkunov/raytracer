#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <stdexcept>

#include <include/camera.h>
#include <include/kdtree.h>
#include <include/utils.h>


const int MAX_TREE_DEPTH = 20;
const int OBJECTS_IN_LEAF = 1;
const int MAX_SPLITS_OF_VOXEL = 5;
const int SPLIT_COST = 5;


inline bool
vector_plane_intersection(const Vector3d vector,
                          const Point3d vector_start,
                          const enum Plane plane,
                          const Coord coord,
                          Point3d * const result);

inline bool
voxel_intersection(const Vector3d vector,
                   const Point3d vector_start,
                   const Voxel v);

inline bool
find_intersection_node(KDNode * const node,
                       const Voxel v,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr);

inline bool
is_intersect_anything_node(KDNode * const node,
                           const Voxel v,
                           const Point3d vector_start,
                           const Vector3d vector);



// Code
// --------------------------------------------------------------

bool Voxel::is_in_voxel(const Point3d p) const {
    return ((p.x > x_min) && (p.x < x_max) &&
            (p.y > y_min) && (p.y < y_max) &&
            (p.z > z_min) && (p.z < z_max));
}

KDTree::KDTree(std::vector<Object3d*> &objects) {
    bounding_box = Voxel(objects);
    root = rec_build(objects, bounding_box, 0);
}

KDTree::~KDTree() {
    delete root;
}

KDNode::~KDNode() {
    if (l) {
        delete l;
    }
    if (r) {
        delete r;
    }
}

KDNode* KDTree::rec_build(std::vector<Object3d*> &objects, Voxel v, int iter) {
    enum Plane p;
    Coord c;
    v.find_plane(objects, iter, p, c);

    if(p == NONE) {
        return new KDNode(objects);
    }

    Voxel vl, vr;
    v.split_voxel(p, c, vl, vr);

    std::vector<Object3d*> l_obj = vl.filter_overlapped_objects(objects);
    KDNode * l = rec_build(l_obj, vl, iter + 1);

    std::vector<Object3d*> r_obj = vr.filter_overlapped_objects(objects);
    KDNode * r = rec_build(r_obj, vr, iter + 1);

    KDNode * node = new KDNode(p, c, std::vector<Object3d*>(), l, r);
    return node;
}

std::vector<Object3d*>
Voxel::filter_overlapped_objects(const std::vector<Object3d *> &objects) const {
    std::vector<Object3d*> result;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (is_in_voxel(objects[i])) {
            result.push_back(objects[i]);
        }
    }
    return result;
}

void Voxel::split_voxel(const enum Plane p, const Coord c,
                        Voxel& vl, Voxel& vr) const {
    vl = vr = (*this);
    switch (p) {
    case XY:
        vl.z_max = vr.z_min = c.z;
        break;
    case XZ:
        vl.y_max = vr.y_min = c.y;
        break;
    case YZ:
        vl.x_max = vr.x_min = c.x;
        break;
    case NONE:
    default:
        // Unreachable case
        throw std::runtime_error("[split_voxel] Plane is NONE. Error");
        break;
    }
}

/*
 * Using Surface Area Heuristic (SAH) for finding best split pane
 *
 * SAH = 0.5 * voxel_surface_area * number_of_objects_in_voxel
 *
 * splitted_SAH = split_cost
 *                + 0.5 * left_voxel_surface_area * number_of_objects_in_left_voxel
 *                + 0.5 * right_voxel_surface_area * number_of_objects_in_right_voxel
 *
 * Finding coordinate of split plane (XY, XZ or YZ) which minimizing SAH
 *
 * If can't find optimal split plane - returns NONE
 *
 * see: http://stackoverflow.com/a/4633332/653511
 */

void Voxel::find_plane(const std::vector<Object3d*> &objects, const int tree_depth,
                       enum Plane &p, Coord &c) const {
    if ((tree_depth >= MAX_TREE_DEPTH) || (objects.size() <= OBJECTS_IN_LEAF)) {
        p = NONE;
        return;
    }

    const Float hx = x_max - x_min;
    const Float hy = y_max - y_min;
    const Float hz = z_max - z_min;

    // Calculating square of each side of initial voxel
    Float Sxy = hx * hy;
    Float Sxz = hx * hz;
    Float Syz = hy * hz;

    const Float Ssum = Sxy + Sxz + Syz;

    // Let's normalize square of each side of initial voxel
    // to satisfy the following relationship:
    // Sxy + Sxz + Syz = 1
    Sxy /= Ssum;
    Sxz /= Ssum;
    Syz /= Ssum;

    // Assume that at the beginning best SAH has initial voxel
    // SAH = 0.5 * square * objects_count
    // square of initial voxel is Sxy + Sxz + Syz = 1
    Float bestSAH = objects.size();
    // initial voxel doesn't have split pane
    p = NONE;

    Float S_split;
    Float S_non_split;

    // Let's find split surface, which have the least SAH

    // TODO: maybe do some refactoring (because of following 3 loops are very similar)

    // trying to minimize SAH by splitting across XY plane
    S_split = Sxy;
    S_non_split = Sxz + Syz;
    for (int i = 1; i < MAX_SPLITS_OF_VOXEL; i++) {
        Voxel vl, vr;
        Float l, r, currSAH;
        Coord curr_split_coord;

        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.z = z_min + l * hz;

        split_voxel(XY, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.objects_in_voxel(objects)
                + (S_split + r * S_non_split) * vr.objects_in_voxel(objects)
                + SPLIT_COST;

        if(currSAH < bestSAH) {
            bestSAH = currSAH;
            p = XY;
            c = curr_split_coord;
        }
    }

    // trying to minimize SAH by splitting across XZ plane
    S_split = Sxz;
    S_non_split = Sxy + Syz;
    for (int i = 1; i < MAX_SPLITS_OF_VOXEL; i++) {
        Voxel vl, vr;
        Float l, r, currSAH;
        Coord curr_split_coord;
        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.y = y_min + l * hy;

        split_voxel(XZ, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.objects_in_voxel(objects)
                + (S_split + r * S_non_split) * vr.objects_in_voxel(objects)
                + SPLIT_COST;

        if (currSAH < bestSAH) {
            bestSAH = currSAH;
            p = XZ;
            c = curr_split_coord;
        }
    }

    // trying to minimize SAH by splitting across YZ plane
    S_split = Syz;
    S_non_split = Sxy + Sxz;
    for (int i = 1; i < MAX_SPLITS_OF_VOXEL; i++) {
        Voxel vl, vr;
        Float l, r, currSAH;
        Coord curr_split_coord;
        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.x = x_min + l * hx;

        split_voxel(YZ, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.objects_in_voxel(objects)
                + (S_split + r * S_non_split) * vr.objects_in_voxel(objects)
                + SPLIT_COST;

        if (currSAH < bestSAH) {
            bestSAH = currSAH;
            p = YZ;
            c = curr_split_coord;
        }
    }
}

int Voxel::objects_in_voxel(const std::vector<Object3d*> &objects) const {
    int count = 0;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (is_in_voxel(objects[i])) {
            ++count;
        }
    }

    return count;
}

Voxel::Voxel(const std::vector<Object3d*> &objects) {

    if(objects.empty()) {
        (*this) = {-1, -1, -1, 1, 1, 1};
        return;
    }

    Point3d min_p = objects[0]->get_min_boundary_point();
    Point3d max_p = objects[0]->get_max_boundary_point();

    Float x_min = min_p.x;
    Float y_min = min_p.y;
    Float z_min = min_p.z;

    Float x_max = max_p.x;
    Float y_max = max_p.y;
    Float z_max = max_p.z;

    for(size_t i = 0; i < objects.size(); i++) {
        Object3d * obj = objects[i];
        min_p = obj->get_min_boundary_point();
        max_p = obj->get_max_boundary_point();

        x_min = (x_min < min_p.x) ? x_min : min_p.x;
        y_min = (y_min < min_p.y) ? y_min : min_p.y;
        z_min = (z_min < min_p.z) ? z_min : min_p.z;

        x_max = (x_max > max_p.x) ? x_max : max_p.x;
        y_max = (y_max > max_p.y) ? y_max : max_p.y;
        z_max = (z_max > max_p.z) ? z_max : max_p.z;
    }

    (*this) = {x_min - 1, y_min - 1, z_min - 1, x_max + 1, y_max + 1, z_max + 1};
}

bool Voxel::is_in_voxel(Object3d * const obj) const {
    Point3d min_p = obj->get_min_boundary_point();
    Point3d max_p = obj->get_max_boundary_point();

    return !((max_p.x < x_min) || (max_p.y < y_min) || (max_p.z < z_min)
          || (min_p.x > x_max) || (min_p.y > y_max) || (min_p.z > z_max) );
}

KDNode::KDNode(const std::vector<Object3d*> &objects) {
    plane = NONE;

    for (size_t i = 0; i < objects.size(); ++i) {
        this->objects.push_back(objects[i]); // TODO REWRITE
    }

    l = NULL;
    r = NULL;
}

inline bool
vector_plane_intersection(const Vector3d vector,
                          const Point3d vector_start,
                          const enum Plane plane,
                          const Coord coord,
                          Point3d * const result) {

    Float k;
    switch(plane) {
        case XY:
            if(((coord.z < vector_start.z) && (vector.z > 0))
                || ((coord.z > vector_start.z) && (vector.z < 0)))
                return false;

            k = (coord.z - vector_start.z) / vector.z;
            *result = Point3d(vector_start.x + vector.x * k,
                              vector_start.y + vector.y * k,
                              coord.z);
            break;

        case XZ:
            if(((coord.y < vector_start.y) && (vector.y > 0))
               || ((coord.y > vector_start.y) && (vector.y < 0)))
                return false;

            k = (coord.y - vector_start.y) / vector.y;
            *result = Point3d(vector_start.x + vector.x * k,
                              coord.y,
                              vector_start.z + vector.z * k);
            break;

        case YZ:
            if(((coord.x < vector_start.x) && (vector.x > 0))
               || ((coord.x > vector_start.x) && (vector.x < 0)))
                return false;

            k = (coord.x - vector_start.x) / vector.x;
            *result = Point3d(coord.x,
                              vector_start.y + vector.y * k,
                              vector_start.z + vector.z * k);
            break;

        case NONE:
            // Unreachable case
            printf("[vector_plane_intersection] Plane is NONE. Error");
            exit(1);
            break;
    }

    return true;
}

inline bool
voxel_intersection(const Vector3d vector,
                   const Point3d vector_start,
                   const Voxel v) {

    if (v.is_in_voxel(vector_start))
        return true;

    Point3d p;
    Coord c;

    c.z = v.z_min;
    if (vector_plane_intersection(vector, vector_start, XY, c, &p)
            && (p.x > v.x_min) && (p.x < v.x_max)
            && (p.y > v.y_min) && (p.y < v.y_max)) {

        return true;
    }

    c.z = v.z_max;
    if (vector_plane_intersection(vector, vector_start, XY, c, &p)
       && (p.x > v.x_min) && (p.x < v.x_max)
       && (p.y > v.y_min) && (p.y < v.y_max)) {

            return true;
    }

    c.y = v.y_min;
    if (vector_plane_intersection(vector, vector_start, XZ, c, &p)
       && (p.x > v.x_min) && (p.x < v.x_max)
       && (p.z > v.z_min) && (p.z < v.z_max)) {

            return true;
    }

    c.y = v.y_max;
    if(vector_plane_intersection(vector, vector_start, XZ, c, &p)
       && (p.x > v.x_min) && (p.x < v.x_max)
       && (p.z > v.z_min) && (p.z < v.z_max)) {

            return true;
    }

    c.x = v.x_min;
    if(vector_plane_intersection(vector, vector_start, YZ, c, &p)
       && (p.y > v.y_min) && (p.y < v.y_max)
       && (p.z > v.z_min) && (p.z < v.z_max)) {

            return true;
    }

    c.x = v.x_max;
    if(vector_plane_intersection(vector, vector_start, YZ, c, &p)
       && (p.y > v.y_min) && (p.y < v.y_max)
       && (p.z > v.z_min) && (p.z < v.z_max)) {

            return true;
    }

    return false;
}

bool KDTree::find_intersection_tree(const Point3d vector_start,
                                    const Vector3d vector,
                                    Object3d ** const nearest_obj_ptr,
                                    Point3d * const nearest_intersection_point_ptr,
                                    Float * const nearest_intersection_point_dist_ptr) {

    #ifndef NO_BOUNDING_BOX
    return (voxel_intersection(vector, vector_start, bounding_box)
            && find_intersection_node(root,
                                      bounding_box,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));
    #else
    // Do not take into account scene bounds
    return find_intersection_node(root,
                                      bounding_box,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr);
    #endif // NO_BOUNDING_BOX
}

inline bool
find_intersection_node(KDNode * const node,
                       const Voxel v,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr) {

    // Is leaf
    if (node->plane == NONE) {
        if (node->objects.size()) {
            Object3d * obj = NULL;
            Point3d intersection_point;
            Float sqr_curr_dist;

            Object3d * nearest_obj = NULL;
            Point3d nearest_intersection_point;
            Float sqr_nearest_dist = 1E99; // TODO REWRITE

            int intersected = false;

            // Finding nearest object
            // and intersection point
            for (size_t i = 0; i < node->objects.size(); ++i) {
                if (node->objects[i]) {
                    obj = node->objects[i];

                    if((obj->intersect(vector_start, vector, &intersection_point))
                       && (v.is_in_voxel(intersection_point))) {

                        sqr_curr_dist = Vector3d(vector_start, intersection_point).module2();

                        if((sqr_curr_dist < sqr_nearest_dist) || (!intersected)) {

                            nearest_obj = obj;
                            nearest_intersection_point = intersection_point;
                            sqr_nearest_dist = sqr_curr_dist;
                            intersected = true;
                        }
                    }
                }
            }

            if (intersected) {
                Float nearest_dist = sqrt(sqr_nearest_dist);

                if (nearest_dist < *nearest_intersection_point_dist_ptr) {
                    *nearest_intersection_point_dist_ptr = nearest_dist;
                    *nearest_obj_ptr = nearest_obj;
                    *nearest_intersection_point_ptr = nearest_intersection_point;
                }
            }

            return intersected;
        }
        return false;
    }

    // Otherwise

    Voxel front_voxel;
    Voxel back_voxel;

    KDNode * front_node;
    KDNode * back_node;

    switch (node->plane) {
    case XY:
        if (((node->coord.z > v.z_min) && (node->coord.z > vector_start.z))
           || ((node->coord.z < v.z_min) && (node->coord.z < vector_start.z))) {

            front_node = node->l;
            back_node = node->r;
            v.split_voxel(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split_voxel(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case XZ:
        if (((node->coord.y > v.y_min) && (node->coord.y > vector_start.y))
           || ((node->coord.y < v.y_min) && (node->coord.y < vector_start.y))) {

            front_node = node->l;
            back_node = node->r;
            v.split_voxel(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split_voxel(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case YZ:
        if (((node->coord.x > v.x_min) && (node->coord.x > vector_start.x))
           || ((node->coord.x < v.x_min) && (node->coord.x < vector_start.x))) {

            front_node = node->l;
            back_node = node->r;
            v.split_voxel(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split_voxel(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case NONE:
    default:
        // Unreachable case
        throw std::runtime_error("[find_intersection_node] Plane is NONE. Error");
        break;
    }

    if (voxel_intersection(vector, vector_start, front_voxel)
       && find_intersection_node(front_node,
                                 front_voxel,
                                 vector_start,
                                 vector,
                                 nearest_obj_ptr,
                                 nearest_intersection_point_ptr,
                                 nearest_intersection_point_dist_ptr))
        return true;

    return (voxel_intersection(vector, vector_start, back_voxel)
            && find_intersection_node(back_node,
                                      back_voxel,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));
}
