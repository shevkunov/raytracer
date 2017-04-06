#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <stdexcept>

#include <include/camera.h>
#include <include/kdtree.h>
#include <include/utils.h>

bool KDTree::Voxel::is_in(const Point3d p) const {
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

KDTree::KDNode::~KDNode() {
    if (l) {
        delete l;
    }
    if (r) {
        delete r;
    }
}

KDTree::KDNode* KDTree::rec_build(std::vector<Object3d*> &objects, Voxel v, int iter) {
    enum Plane p;
    Point3d c;
    v.find_plane(objects, iter, p, c);

    if (p == NONE) {
        return new KDNode(objects);
    }

    Voxel vl, vr;
    v.split(p, c, vl, vr);

    std::vector<Object3d*> l_obj = vl.filter_overlapped_objects(objects);
    KDNode * l = rec_build(l_obj, vl, iter + 1);

    std::vector<Object3d*> r_obj = vr.filter_overlapped_objects(objects);
    KDNode * r = rec_build(r_obj, vr, iter + 1);

    KDNode * node = new KDNode(p, c, std::vector<Object3d*>(), l, r);
    return node;
}

std::vector<Object3d*>
KDTree::Voxel::filter_overlapped_objects(const std::vector<Object3d *> &objects) const {
    std::vector<Object3d*> result;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (is_in(objects[i])) {
            result.push_back(objects[i]);
        }
    }
    return result;
}

void KDTree::Voxel::split(const enum Plane p, const Point3d c,
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

void KDTree::Voxel::find_plane(const std::vector<Object3d*> &objects, const int tree_depth,
                       enum Plane &p, Point3d &c) const {
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
        Point3d curr_split_coord;

        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.z = z_min + l * hz;

        split(XY, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.count_objects_in(objects)
                + (S_split + r * S_non_split) * vr.count_objects_in(objects)
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
        Point3d curr_split_coord;
        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.y = y_min + l * hy;

        split(XZ, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.count_objects_in(objects)
                + (S_split + r * S_non_split) * vr.count_objects_in(objects)
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
        Point3d curr_split_coord;
        l = ((Float) i) / MAX_SPLITS_OF_VOXEL;
        r = 1. - l;

        // Current coordinate of split surface
        curr_split_coord.x = x_min + l * hx;

        split(YZ, curr_split_coord, vl, vr);

        currSAH = (S_split + l * S_non_split) * vl.count_objects_in(objects)
                + (S_split + r * S_non_split) * vr.count_objects_in(objects)
                + SPLIT_COST;

        if (currSAH < bestSAH) {
            bestSAH = currSAH;
            p = YZ;
            c = curr_split_coord;
        }
    }
}

int KDTree::Voxel::count_objects_in(const std::vector<Object3d*> &objects) const {
    int count = 0;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (is_in(objects[i])) {
            ++count;
        }
    }

    return count;
}

KDTree::Voxel::Voxel(const std::vector<Object3d*> &objects) {
    if (objects.empty()) {
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

bool KDTree::Voxel::is_in(Object3d * const obj) const {
    Point3d min_p = obj->get_min_boundary_point();
    Point3d max_p = obj->get_max_boundary_point();

    return !((max_p.x < x_min) || (max_p.y < y_min) || (max_p.z < z_min)
          || (min_p.x > x_max) || (min_p.y > y_max) || (min_p.z > z_max) );
}

KDTree::KDNode::KDNode(const std::vector<Object3d*> &objects) {
    plane = NONE;

    for (size_t i = 0; i < objects.size(); ++i) {
        this->objects.push_back(objects[i]); // TODO REWRITE
    }

    l = NULL;
    r = NULL;
}

bool KDTree::Voxel::vector_plane_intersection(const Vector3d vector,
                                      const Point3d vector_start,
                                      const enum Plane plane,
                                      const Point3d coord,
                                      Point3d &result) {
    Float k;
    switch (plane) {
    case XY:
        if (((coord.z < vector_start.z) && (vector.z > 0))
                || ((coord.z > vector_start.z) && (vector.z < 0))) {
            return false;
        }

        k = (coord.z - vector_start.z) / vector.z;
        result = Point3d(vector_start.x + vector.x * k,
                          vector_start.y + vector.y * k,
                          coord.z);
        break;

    case XZ:
        if (((coord.y < vector_start.y) && (vector.y > 0))
                || ((coord.y > vector_start.y) && (vector.y < 0)))
            return false;

        k = (coord.y - vector_start.y) / vector.y;
        result = Point3d(vector_start.x + vector.x * k,
                          coord.y,
                          vector_start.z + vector.z * k);
        break;

    case YZ:
        if (((coord.x < vector_start.x) && (vector.x > 0))
           || ((coord.x > vector_start.x) && (vector.x < 0)))
            return false;

        k = (coord.x - vector_start.x) / vector.x;
        result = Point3d(coord.x,
                          vector_start.y + vector.y * k,
                          vector_start.z + vector.z * k);
        break;

    case NONE:
    default:
        // Unreachable case
        throw new std::runtime_error("[vector_plane_intersection] Plane is NONE. Error");
        break;
    }

    return true;
}

inline bool KDTree::Voxel::x_in_voxel(const Float x) const {
    return (x_min < x) && (x < x_max);
}

inline bool KDTree::Voxel::y_in_voxel(const Float y) const {
    return (y_min < y) && (y < y_max);
}

inline bool KDTree::Voxel::z_in_voxel(const Float z) const {
    return (z_min < z) && (z < z_max);
}

bool KDTree::Voxel::intersection(const Vector3d vector,
                               const Point3d vector_start) const {
    if (is_in(vector_start)) {
        return true;
    }

    Point3d p, c;

    c.z = z_min;
    if (vector_plane_intersection(vector, vector_start, XY, c, p)
            && x_in_voxel(p.x) && y_in_voxel(p.y)) {
        return true;
    }

    c.z = z_max;
    if (vector_plane_intersection(vector, vector_start, XY, c, p)
            && x_in_voxel(p.x) && y_in_voxel(p.y)) {
        return true;
    }

    c.y = y_min;
    if (vector_plane_intersection(vector, vector_start, XZ, c, p)
            && x_in_voxel(p.x) && z_in_voxel(p.z)) {
        return true;
    }

    c.y = y_max;
    if (vector_plane_intersection(vector, vector_start, XZ, c, p)
            && x_in_voxel(p.x) && z_in_voxel(p.z)) {
        return true;
    }

    c.x = x_min;
    if (vector_plane_intersection(vector, vector_start, YZ, c, p)
            && y_in_voxel(p.y) && z_in_voxel(p.z)) {
        return true;
    }

    c.x = x_max;
    if (vector_plane_intersection(vector, vector_start, YZ, c, p)
            && y_in_voxel(p.y) && z_in_voxel(p.z)) {
        return true;
    }

    return false;
}

bool KDTree::find_intersection_tree(const Point3d vector_start,
                                    const Vector3d vector,
                                    Object3d* &nearest_obj_ptr,
                                    Point3d &nearest_intersection_point_ptr,
                                    Float &nearest_intersection_point_dist_ptr) {

    return (bounding_box.intersection(vector, vector_start)
            && find_intersection_node(root,
                                      bounding_box,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));

}

bool KDTree::find_intersection_node(KDNode * const node,
                            const Voxel v,
                            const Point3d vector_start,
                            const Vector3d vector,
                            Object3d* &nearest_obj_ptr,
                            Point3d &nearest_intersection_point_ptr,
                            Float &nearest_intersection_point_dist_ptr) {
    if (node->plane == NONE) {
        if (node->objects.size()) {
            Object3d * nearest_obj = NULL;
            Point3d nearest_intersection_point;
            Float sqr_nearest_dist = 1E99; // TODO REWRITE

            bool intersected = false;

            // Finding nearest object
            // and intersection point
            for (size_t i = 0; i < node->objects.size(); ++i) {
                if (node->objects[i]) {
                    Object3d * obj = node->objects[i];
                    Point3d intersection_point;

                    if ((obj->intersect(vector_start, vector, intersection_point))
                            && (v.is_in(intersection_point))) {

                        Float sqr_curr_dist = Vector3d(vector_start, intersection_point).module2();

                        if ((sqr_curr_dist < sqr_nearest_dist) || (!intersected)) {

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

                if (nearest_dist < nearest_intersection_point_dist_ptr) {
                    nearest_intersection_point_dist_ptr = nearest_dist;
                    nearest_obj_ptr = nearest_obj;
                    nearest_intersection_point_ptr = nearest_intersection_point;
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
            v.split(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case XZ:
        if (((node->coord.y > v.y_min) && (node->coord.y > vector_start.y))
           || ((node->coord.y < v.y_min) && (node->coord.y < vector_start.y))) {

            front_node = node->l;
            back_node = node->r;
            v.split(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case YZ:
        if (((node->coord.x > v.x_min) && (node->coord.x > vector_start.x))
           || ((node->coord.x < v.x_min) && (node->coord.x < vector_start.x))) {

            front_node = node->l;
            back_node = node->r;
            v.split(node->plane, node->coord, front_voxel, back_voxel);
        } else {
            front_node = node->r;
            back_node = node->l;
            v.split(node->plane, node->coord, back_voxel, front_voxel);
        }
        break;

    case NONE:
    default:
        // Unreachable case
        throw std::runtime_error("[find_intersection_node] Plane is NONE. Error");
        break;
    }

    if (front_voxel.intersection(vector, vector_start)
       && find_intersection_node(front_node,
                                 front_voxel,
                                 vector_start,
                                 vector,
                                 nearest_obj_ptr,
                                 nearest_intersection_point_ptr,
                                 nearest_intersection_point_dist_ptr))
        return true;

    return (back_voxel.intersection(vector, vector_start)
            && find_intersection_node(back_node,
                                      back_voxel,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));
}
