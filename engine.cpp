#include "engine.h"

#include <cstdio>
#include <string>

#include <iostream>

#include <include/canvas.h>
#include <include/camera.h>
#include <include/obj_loader.h>
#include <include/sphere.h>
#include <include/triangle.h>
#include <include/scene.h>

const Color BACKGROUND_COLOR = Color(255, 255, 255);

QImage engine(size_t width, size_t height) {
    // Allocating scene
    Scene * scene = new Scene(BACKGROUND_COLOR);

    // Allocating new sphere
    Float radius = 100;
    Point3d center(0, 0, 0);
    Color sphere_color(250, 30, 30);
    Material sphere_material(1, 5, 5, 10, 0, 10);

    Object3d* sphere = new Sphere(center, radius, sphere_color, sphere_material);
    //scene->add_object(sphere);

{
    // Allocating new sphere
    Float radius = 10;
    Point3d center(100, 100, 100);
    Color sphere_color(30, 30, 230);
    Material sphere_material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new Sphere(center, radius, sphere_color, sphere_material);

    // Adding sphere to the scene
    scene->add_object(sphere);
}
//{
    Canvas tex("./models/wall.png");

    scene->add_object(new TexturedTriangle3d(Point3d(-300, -300, -120),
                                             Point3d(300, -300, -120),
                                             Point3d(300, 300, -120),
                                             Point2d(5, 0),
                                             Point2d(0, 0),
                                             Point2d(0, 5),
                                             &tex,
                                             Color(55, 255, 55),
                                             Material(1, 6, 0, 2, 0, 0)));

//}
    // Allocating new triangle
    Object3d * triangle = new Triangle3d(Point3d(-700, -700, -130), // vertex 1
                                       Point3d( 700, -700, -130), // vertex 2
                                       Point3d(   0,  400, -130), // vertex 3
                                       Color(100, 255, 30),       // color
                                       Material(1, 6, 0, 2, 0, 0) // surface params
                                       );

    // Adding triangle to the scene
    scene->add_object(triangle);

    // Loading 3D model of cow from *.obj file
    // defining transformations and parameters of 3D model
    // TODO: must be refactored...

    SceneFaceHandler pram1(scene,
                                  20, 0, 0, -20, 0, 0, 20,
                                  Color(220, 220, 220),
                                  Material(1, 3, 5, 0, 0, 10));

    pram1.load_obj("./models/teapot.obj");

    SceneFaceHandler scene_face_handler(scene,
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

    scene_face_handler.load_obj("./models/cow.obj");
             // default handler which adding polygons of 3D model to scene:
    /*
    for (int i = 0; i < 10; ++i) {
        Object3d *o = (scene->objects[i]);
        std::cout << "\n OBJS:" << o->get_min_boundary_point().x << " " << o->get_max_boundary_point().x;
    }*/

    // This function is requried (bulding k-d tree of entire scene)
    scene->prepare_scene();

    printf("\nNumber of polygons: %d\n", (int)scene->objects.size());

    // Allocating new light source

    LightSource3d * light_source
            = new LightSource3d(Point3d(-300, 300, 300), Color(255, 255, 255));
    // Adding light source to the scene
    scene->add_light_source(light_source);

    // Adding fog
    //Float density = 0.001;
    //scene->set_exponential_fog(density);

    // Allocating camera
    // TODO: It's a pity, but quaternions are not implemented yet :(
    Point3d camera_location = Point3d(0, 500, 0);
    Float focus = 320;
    Float x_angle = -1.57;
    Float y_angle = 0;
    Float z_angle = 3.14;
    Camera camera(camera_location, x_angle,
                  y_angle, z_angle, focus);

    // Rotate camera if needed
    // rotate_camera(camera, d_x_angle, d_y_angle, d_z_angle);

    // Move camera if needed
    // move_camera(camera, vector3df(d_x, d_y, d_z));

    // Alocate new canvas, to render scene on it
    Canvas canvas(width, height);

    scene->render(camera, canvas);

    canvas.write_png("rendered.png");

    // Saving rendered image in PNG format
    //canvas.write_png("example.png");

    //Сanvas grayscaled_canvas = canvas.grayscale();

    //grayscaled_canvas.write_png("gray_example.png");

    //Canvas * edges_canvas = detect_edges_canvas(canvas, THREADS_NUM);
   // write_png("edges_example.png", edges_canvas);

    QImage ret = canvas.getQImage();

    //release_canvas(grayscaled_canvas);
    //release_canvas(edges_canvas);
    delete scene;
    return ret;
}
