#include <include/scene.h>

Color Scene::trace(const Camera &camera, const Vector3d &vector) const {
    Vector3d r_vector = vector.rotate_x(camera.sin_al_x, camera.cos_al_x)
                              .rotate_z(camera.sin_al_z, camera.cos_al_z)
                              .rotate_y(camera.sin_al_y, camera.cos_al_y);
    
    return trace_recursively(camera.position, r_vector, INITIAL_RAY_INTENSITY, 0);
}

Color Scene::trace_recursively(const Point3d &vector_start,
                                      const Vector3d &vector, const Float &intensity,
                                      const int recursion_level) const {
    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLOAT_MAX;
    
    if (kd_tree->find_intersection_tree(vector_start, vector,
                               nearest_obj, nearest_intersection_point,
                               nearest_intersection_point_dist)) {

        return calculate_color(vector_start, vector, nearest_obj,
                               nearest_intersection_point, nearest_intersection_point_dist,
                               intensity, recursion_level);
    }
    
    return background_color;
}

Color Scene::calculate_color(const Point3d &vector_start,
                             const Vector3d &vector, const Object3d * const obj,
                             const Point3d &point, const Float &dist,
                             const Float &intensity, const int recursion_level) const {
    Float fog_density = fog->density(dist);
    const Material material = obj->get_material(point); 
    const Vector3d norm = obj->get_normal_vector(point);
    
    Color obj_color = obj->get_color(point);
    Vector3d reflected_ray;

    // Result
    Color result_color(0, 0, 0);

    if ((material.Ks) || (material.Kr)) {
        reflected_ray = vector.reflect(norm);
    }

    // Ambient
    if (material.Ka) {
        Color ambient_color = Color::mix(background_color, obj_color);

        result_color = Color::add(result_color,
                                  Color::multiply(ambient_color, material.Ka));
    }
    
    // Diffuse
    if (material.Kd) {
        Color diffuse_color = obj_color;
        if (light_sources.size()) {
            Color light_color = get_lighting_color(point, norm);
            diffuse_color = Color::mix(diffuse_color, light_color);
        }

        result_color = Color::add(result_color,
                                  Color::multiply(diffuse_color, material.Kd));
    }
    
    // Specular
    if (material.Ks) {
        Color specular_color = background_color;
        if (light_sources.size()) {
            specular_color = get_specular_color(point, reflected_ray, material.p);
        }

        result_color = Color::add(result_color,
                                  Color::multiply(specular_color, material.Ks));
    }
    
    
    // Reflect
    if (material.Kr) {
        Color reflected_color;
        if ((intensity > THRESHOLD_RAY_INTENSITY)
                && (recursion_level < MAX_RAY_RECURSION_LEVEL)) {
            reflected_color = trace_recursively(point, reflected_ray,
                                                intensity * material.Kr * (1 - fog_density),
                                                recursion_level + 1);
        } else {
            reflected_color = background_color;
        }

        result_color = Color::add(result_color,
                                  Color::multiply(reflected_color, material.Kr));
    }

    // Transparent
    if (material.Kt) {
        Color transparent_color;
        Vector3d transparent_ray = vector;
        bool refracted = refract(transparent_ray, norm, material.IOR);

        if ((intensity > THRESHOLD_RAY_INTENSITY)
                && (recursion_level < MAX_RAY_RECURSION_LEVEL)) {
            if (refracted) {
                transparent_color = trace_recursively(point, transparent_ray,
                                                      intensity * material.Kt * (1 - fog_density),
                                                      recursion_level + 1);
            } else {
                transparent_color = trace_recursively(point, reflected_ray,
                                                    intensity * material.Kt * (1 - fog_density),
                                                    recursion_level + 1);
            }
        } else {
            transparent_color = background_color; // TODO CHECK
        }

        result_color = Color::add(result_color,
                                  Color::multiply(transparent_color, material.Kt));
    }

    if (fog->fog()) {
        result_color = Color::add(Color::multiply(background_color, fog_density),
                                  Color::multiply(result_color, 1 - fog_density));
    }

    return result_color;
}

Color Scene::get_lighting_color(const Point3d &point, const Vector3d &norm_v) const {
    Color light_color = Color(0, 0, 0);
    for (size_t i = 0; i < light_sources.size(); i++) {
        if (light_sources[i]) {

            LightSource3d * ls = light_sources[i];
            if (is_viewable(ls->location, point)) {
                Vector3d v_ls = Vector3d(point, ls->location);
                Float cos_ls = fabs(Vector3d::cos(norm_v, v_ls));
                Color color_ls = Color::multiply(ls->color, cos_ls);
                light_color = Color::add(light_color, color_ls);
            }

            if (SECONDARY_LIGHT && false) { // TODO CHECK
                for (size_t j = 0; j < reflecting_objects.size(); ++j) {
                    Object3d * obj = reflecting_objects[j];
                    LightSource3d ls_secondary;
                    if (obj->secondary_light(point, *ls, ls_secondary)) {
                        if (is_viewable(ls_secondary.location, point)) {
                            Vector3d v_ls_s = Vector3d(point, ls_secondary.location);
                            Float cos_ls_s = fabs(Vector3d::cos(norm_v, v_ls_s));
                            Color color_ls_s = Color::multiply(ls_secondary.color, cos_ls_s);
                            light_color = Color::add(light_color, color_ls_s);
                        }
                    }
                }
            }
        }
    }
    
    return light_color;
}

Color Scene::get_specular_color(const Point3d &point, const Vector3d &reflected_ray,
                                const Float &p) const {
    Color light_color(0, 0, 0);
    for (size_t i = 0; i < light_sources.size(); i++) {
        if (light_sources[i]) {
            LightSource3d * ls = light_sources[i];
            if (is_viewable(ls->location, point)) {
                Vector3d v_ls = Vector3d(point, ls->location);
                Float cos_ls = Vector3d::cos(reflected_ray, v_ls);
                if (cos_ls > EPSILON) {
                    Color color_ls = Color::multiply(ls->color, pow(cos_ls, p));
                    light_color = Color::add(light_color, color_ls);
                }
            }

            if (SECONDARY_LIGHT) { // TODO CHECK
                for (size_t j = 0; j < reflecting_objects.size(); ++j) {
                    Object3d * obj = reflecting_objects[j];
                    LightSource3d ls_secondary;
                    if (obj->secondary_light(point, *ls, ls_secondary)) {
                        if (is_viewable(ls_secondary.location, point)) {
                            Vector3d v_ls_s = Vector3d(point, ls_secondary.location);
                            Float cos_ls_s = Vector3d::cos(reflected_ray, v_ls_s);
                            if (cos_ls_s > EPSILON) {
                                Color color_ls_s = Color::multiply(ls_secondary.color, pow(cos_ls_s, p));
                                light_color = Color::add(light_color, color_ls_s);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return light_color;
}

bool Scene::is_viewable(const Point3d &target_point, const Point3d &starting_point) const {
    const Vector3d ray = Vector3d(starting_point, target_point);
    const Float target_dist = ray.module();
    
    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLOAT_MAX;
    
    if (kd_tree->find_intersection_tree(starting_point,
                                        ray,
                                        nearest_obj,
                                        nearest_intersection_point,
                                        nearest_intersection_point_dist)) {

        // Check if intersection point is closer than target_point
        return (target_dist < nearest_intersection_point_dist);
    }
    // Ray doesn't intersect any of scene objects
    return true;
}

bool Scene::refract(Vector3d& ray_dir, Vector3d a_normal, const Float &a_matIOR) const {
    Float eta = 1. / a_matIOR; // eta = in_IOR/out_IOR
    Float cos_theta = 0. - Vector3d::dot(a_normal, ray_dir);
    if (cos_theta < 0) {
        cos_theta = cos_theta * (-1.);
        a_normal = a_normal.mul(-1.);
        eta = 1./eta;
    }
    Float k = 1. - eta*eta*(1. - cos_theta*cos_theta);
    if (k >= 0.) {
        ray_dir = (ray_dir.mul(eta) + a_normal.mul(eta*cos_theta - sqrt(k)));
        ray_dir.normalize();
    }
    return (k > 0);
}
