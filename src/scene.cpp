#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include <include/scene.h>

Scene::Scene(const Color &background_color) :
        background_color(background_color),
        fog(new Fog()) {
}

Scene::~Scene() {
    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i]) {
            delete objects[i];
        }
    }
    
    for (size_t i = 0; i < light_sources.size(); i++) {
        if (light_sources[i]) {
            delete light_sources[i];
        }
    }
    delete fog;
    delete kd_tree;
}

void Scene::add_object(Object3d * const object) {
    objects.push_back(object);
    if (object->reflects()) {
        reflecting_objects.push_back(object);
    }
}

void Scene::prepare_scene() {
    rebuild_kd_tree();
}

void Scene::add_light_source(LightSource3d * const light_source) {
    light_sources.push_back(light_source);
}

void Scene::set_no_fog() {
    delete fog;
    fog = new Fog();
}

void Scene::set_exponential_fog(const Float &k) {
    delete fog;
    fog = new ExponentialFog(k);
}

void Scene::rebuild_kd_tree() {
    kd_tree = new KDTree(objects);
}

void Scene::render(const Camera &camera, Canvas& canvas) const {
    const int w = canvas.width();
    const int h = canvas.height();
    const Float dx = w / 2.0;
    const Float dy = h / 2.0;
    const Float focus = camera.proj_plane_dist;

    for(int i = 0; i < w; i++) {
        for(int j = 0; j < h; j++) {
            const Float x = i - dx;
            const Float y = j - dy;
            const Vector3d ray = Vector3d(x, y, focus);
            const Color col = trace(camera, ray);
            canvas.set_pixel(i, j, col);
        }
    }

    if(ANTIALIASING) {
        Canvas edges = canvas.detect_edges();
        for (int i = 1; i < w - 1; i++) {
            for (int j = 1; j < h - 1; j++) {
                // edges canvas is grayscaled
                // it means that color components (r, g, b) are equal
                Byte gray = edges.get_pixel(i, j).r();

                if (gray > 10) {
                    const Float x = i - dx;
                    const Float y = j - dy;

                    Color c = canvas.get_pixel(i, j);

                    const Float weight = 1.0 / 4;

                    c = Color::multiply(c, weight);
                    c = Color::add(c,
                                   Color::multiply(trace(camera, Vector3d(x + 0.5, y, focus)),
                                                   weight)
                                   );
                    c = Color::add(c,
                                   Color::multiply(trace(camera, Vector3d(x, y + 0.5, focus)),
                                                   weight)
                                   );
                    c = Color::add(c,
                                   Color::multiply(trace(camera, Vector3d(x + 0.5, y + 0.5, focus)),
                                                   weight)
                                   );
                    canvas.set_pixel(i, j, c);
                }
            }
        }
    }
}

size_t Scene::get_objects_count() const {
    return objects.size();
}

