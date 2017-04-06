#include <include/scene.h>

const int INITIAL_RAY_INTENSITY = 100;
const int THRESHOLD_RAY_INTENSITY = 10;
const int MAX_RAY_RECURSION_LEVEL = 10;

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

        return calculate_color(vector_start, vector, &nearest_obj,
                               &nearest_intersection_point, &nearest_intersection_point_dist,
                               intensity, recursion_level);
    }
    
    return background_color;
}

Color Scene::calculate_color(const Point3d &vector_start,
                             const Vector3d &vector, Object3d * const * obj_ptr,
                             const Point3d * const point_ptr, const Float * const dist_ptr,
                             const Float &intensity, const int recursion_level) const {

    const Object3d * obj = *obj_ptr;
    const Point3d point = *point_ptr;
    const Float dist = *dist_ptr;
    

    Float fog_density = fog->density(dist);
    const Material material = obj->get_material(point); 
    const Vector3d norm = obj->get_normal_vector(point);
    
    Color obj_color = obj->get_color(point);
    Color ambient_color;
    Color diffuse_color;
    Color reflected_color;
    Color specular_color;

    Vector3d reflected_ray;
    if ((material.Ks) || (material.Kr)) {
        reflected_ray = vector.reflect(norm);
    }
    
    // Ambient
    if (material.Ka) {
        ambient_color = Color::mix(background_color, obj_color);
    }
    
    // Diffuse
    if (material.Kd) {
        diffuse_color = obj_color;
        if (light_sources.size()) {
            Color light_color = get_lighting_color(point, norm);
            diffuse_color = Color::mix(diffuse_color, light_color);
        }
    }
    
    // Specular
    if (material.Ks) {
        specular_color = background_color;
        if (light_sources.size()) {
            specular_color = get_specular_color(point, reflected_ray, material.p);
        }
    }
    
    
    // Reflect
    if (material.Kr) {
        // Avoid deep recursion by tracing rays, which have intensity is greather than threshold
        // and avoid infinite recursion by limiting number of recursive calls
        if ((intensity > THRESHOLD_RAY_INTENSITY)
                && (recursion_level < MAX_RAY_RECURSION_LEVEL)) {
            reflected_color = trace_recursively(point, reflected_ray,
                                                intensity * material.Kr * (1 - fog_density),
                                                recursion_level + 1);
        } else {
            reflected_color = background_color;
        }
    }
    
    // Result
    Color result_color(0, 0, 0);

    if (material.Ka) {
        result_color = Color::add(result_color,
                                  Color::multiply(ambient_color, material.Ka));
    }

    if (material.Kd) {
        result_color = Color::add(result_color,
                                  Color::multiply(diffuse_color, material.Kd));
    }

    if (material.Ks) {
        result_color = Color::add(result_color,
                                  Color::multiply(specular_color, material.Ks));
    }

    if (material.Kr) {
        result_color = Color::add(result_color,
                                  Color::multiply(reflected_color, material.Kr));
    }
    
    if (fog->fog()) {
        result_color = Color::add(Color::multiply(background_color, fog_density),
                                  Color::multiply(result_color, 1 - fog_density));
    }

    return result_color;
}

Color Scene::get_lighting_color(const Point3d &point, const Vector3d &norm_v) const {
    Color light_color = Color(0, 0, 0);

    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    for (size_t i = 0; i < light_sources.size(); i++) {
        if (light_sources[i]) {
            ls = light_sources[i];
            if (is_viewable(ls->location, point)) {
                v_ls = Vector3d(point, ls->location);
                cos_ls = fabs(Vector3d::cos(norm_v, v_ls));
                color_ls = Color::multiply(ls->color, cos_ls);
                light_color = Color::add(light_color, color_ls);
            }
        }
    }
    
    return light_color;
}

Color Scene::get_specular_color(const Point3d &point, const Vector3d &reflected_ray,
                                const Float &p) const {
    Color light_color(0, 0, 0);
    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    for (size_t i = 0; i < light_sources.size(); i++) {
        if (light_sources[i]) {
            ls = light_sources[i];
            if(is_viewable(ls->location, point)) {
                v_ls = Vector3d(point, ls->location);
                cos_ls = Vector3d::cos(reflected_ray, v_ls);
                if(cos_ls > EPSILON) {
                    color_ls = Color::multiply(ls->color, pow(cos_ls, p));
                    light_color = Color::add(light_color, color_ls);
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
