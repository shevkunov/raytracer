#include "engine.h"

#include <stdio.h>

#include <include/canvas.h>
#include <include/render.h>
#include <include/obj_loader.h>
#include <include/sphere.h>
#include <include/triangle.h>

#define CANVAS_W 300
#define CANVAS_H 300

// Boost by rendering in parallel
#define THREADS_NUM 4

const Color BACKGROUND_COLOR = Color(255, 255, 255);
const int MAX_OBJECTS_NUMBER = 10000;
const int MAX_LIGHT_SOURCES_NUMBER = 5;

QImage engine() {
    // Allocating scene
    Scene * scene = new_scene(MAX_OBJECTS_NUMBER,
                              MAX_LIGHT_SOURCES_NUMBER,
                              BACKGROUND_COLOR);

    // Allocating new sphere
    Float radius = 100;
    Point3d center(0, 0, 0);
    Color sphere_color(250, 30, 30);
    Material sphere_material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new Sphere(center,
                                   radius,
                                   sphere_color,
                                   sphere_material);

    // Adding sphere to the scene
    add_object(scene,
               sphere);
{
    // Allocating new sphere
    Float radius = 10;
    Point3d center(100, 100, 100);
    Color sphere_color(30, 30, 230);
    Material sphere_material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new Sphere(center, radius, sphere_color, sphere_material);

    // Adding sphere to the scene
    add_object(scene,
               sphere);
}
    // Allocating new triangle
    Object3d * triangle = new Triangle3d(Point3d(-700, -700, -130), // vertex 1
                                       Point3d( 700, -700, -130), // vertex 2
                                       Point3d(   0,  400, -130), // vertex 3
                                       Color(100, 255, 30),       // color
                                       Material(1, 6, 0, 2, 0, 0) // surface params
                                       );

    // Adding triangle to the scene
    add_object(scene,
               triangle);

    // Loading 3D model of cow from *.obj file
    // defining transformations and parameters of 3D model
    // TODO: must be refactored...
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  // scale:
                                  40,
                                  // move dx, dy, dz:
                                  -150, -100, 30,
                                  // rotate around axises x, y, z:
                                  0, 0, 0,
                                  // color
                                  Color(200, 200, 50),
                                  // surface params
                                  Material(2, 3, 0, 0, 0, 0)
                                  );

    load_obj("./models/cow.obj",
             // default handler which adding polygons of 3D model to scene:
             scene_face_handler,
             &load_params);

    // This function is requried (bulding k-d tree of entire scene)
    prepare_scene(scene);

    printf("\nNumber of polygons: %i\n", scene->last_object_index + 1);

    // Allocating new light source

    LightSource3d * light_source
            = new LightSource3d(Point3d(-300, 300, 300), Color(255, 255, 255));
    // Adding light source to the scene
    add_light_source(scene,
                     light_source);

    // Adding fog
    Float density = 0.0001;
    set_exponential_fog(scene, density);

    // Allocating camera
    // TODO: It's a pity, but quaternions are not implemented yet :(
    Point3d camera_location = Point3d(0, 500, 0);
    Float focus = 320;
    Float x_angle = -1.57;
    Float y_angle = 0;
    Float z_angle = 3.14;
    Camera * camera = new_camera(camera_location,
                                 x_angle,
                                 y_angle,
                                 z_angle,
                                 focus);

    // Rotate camera if needed
    // rotate_camera(camera, d_x_angle, d_y_angle, d_z_angle);

    // Move camera if needed
    // move_camera(camera, vector3df(d_x, d_y, d_z));

    // Alocate new canvas, to render scene on it
    Canvas canvas(CANVAS_W, CANVAS_H);

    render_scene(scene,
                 camera,
                 canvas);
    // Saving rendered image in PNG format
    //canvas.write_png("example.png");

    //Сanvas grayscaled_canvas = canvas.grayscale();

    //grayscaled_canvas.write_png("gray_example.png");

    //Canvas * edges_canvas = detect_edges_canvas(canvas, THREADS_NUM);
   // write_png("edges_example.png", edges_canvas);

    QImage ret = canvas.getQImage();

    //release_canvas(grayscaled_canvas);
    //release_canvas(edges_canvas);
    release_scene(scene);
    release_camera(camera);

    return ret;
}
