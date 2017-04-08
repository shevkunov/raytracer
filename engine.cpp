#include "engine.h"

#include <cstdio>
#include <string>

#include <iostream>

#include <include/canvas.h>
#include <include/camera.h>
#include <include/obj_loader.h>
#include <include/sphere.h>
#include <include/triangle.h>
#include <include/quadrangle.h>
#include <include/scene.h>

QImage engine(size_t width, size_t height) {
    // Allocating scene
    Color BACKGROUND_COLOR = Color(240, 240, 240);
    Scene * scene = new Scene(BACKGROUND_COLOR);

    /// Quadrangle3d 1
    Object3d* quad = new Quadrangle3d(Point3d(-500, -500, -100),
                                      Point3d(+800, -0, -100),
                                      Point3d(+800, -0, +300),
                                      Point3d(-500, -500, +300),
                                      Color(255, 0, 0),
                                      Material(15, 5, 50, 100, 0, 10));
    scene->add_object(quad);


    /// Sphere 0
    {
        Float radius = 50;
        Point3d center(50, 100, 0);
        Color sphere_color(100, 200, 30);
        Material sphere_material(1, 5, 5, 0, 50, 10, 0.9);
        Object3d* sphere = new Sphere(center, radius, sphere_color, sphere_material);
        scene->add_object(sphere);
    }

    /// Sphere 0.5
    {
        Float radius = 50;
        Point3d center(75, 125, -100);
        Color sphere_color(250, 250, 50);
        Material sphere_material(1, 3, 2, 0, 0, 10);
        Object3d* sphere = new Sphere(center, radius, sphere_color, sphere_material);
        scene->add_object(sphere);
    }


    /*
    /// Sphere 1
    Float radius = 100;
    Point3d center(0, 0, 0);
    Color sphere_color(250, 30, 30);
    Material sphere_material(1, 5, 5, 10, 0, 10);
    Object3d* sphere = new Sphere(center, radius, sphere_color, sphere_material);
    scene->add_object(sphere);
    */

    /// Sphere 2
    Float radius = 10;
    Point3d center(100, 100, 100);
    Color sphere_color(30, 30, 230);
    Material sphere_material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new Sphere(center, radius, sphere_color, sphere_material);
    scene->add_object(sphere);


    /// TexturedTriangle3d 1
    Canvas tex("./models/wall.png");
    scene->add_object(new TexturedTriangle3d(Point3d(-300-5, -300, -120),
                                             Point3d(300, -300, -120),
                                             Point3d(300, 300, -120),
                                             Point2d(5, 0),
                                             Point2d(0, 0),
                                             Point2d(0, 5),
                                             &tex,
                                             Color(55, 255, 55),
                                             Material(1, 6, 0, 2, 0, 0)));
/*
    /// TexturedQuadrangle3d 1
    Canvas tex2("./models/skybox/posx.png");
    scene->add_object(new TexturedQuadrangle3d(Point3d(-300, -120, -300),
                                               Point3d(300, -120, -300),
                                               Point3d(300, -120, 300),
                                               Point3d(-300,-120, 300),
                                               Point2d(5, 0),
                                               Point2d(0, 0),
                                               Point2d(0, 5),
                                               Point2d(5, 5),
                                               &tex2,
                                               Color(55, 255, 55),
                                               Material(1, 6, 0, 2, 0, 0)));
 */
    /// Triangle3d 2
    Object3d * triangle = new Triangle3d(Point3d(-700, -700, -130), // vertex 1
                                       Point3d( 700, -700, -130), // vertex 2
                                       Point3d(   0,  500, -130), // vertex 3
                                       Color(255, 100, 30),       // color
                                       Material(1, 6, 0, 2, 0, 0) // surface params
                                       );
    scene->add_object(triangle);


    /// Load Teapot Model
    SceneFaceHandler pram1(scene,
                           20, 0, 0, -20, 0, 0, 20,
                           Color(220, 220, 220),
                           Material(1, 3, 5, 0, 0, 10));
    pram1.load_obj("./models/teapot.obj");



    /// Load Cow Model
    SceneFaceHandler scene_face_handler(scene,
                                      // scale:
                                      40,
                                      // move dx, dy, dz:
                                      -150, -100, 0,
                                      // rotate around axises x, y, z:
                                      0, 0, 0,
                                      // color
                                      Color(50, 50, 50),
                                      // surface params
                                      Material(3, 3, 1, 0, 0, 5)
                                      );
    scene_face_handler.load_obj("./models/lamp.obj");



    /// Build KDTree
    scene->prepare_scene();
    std::cout << "\nNumber of polygons:" << scene->get_objects_count() << "\n";




    /// Adding LS
    LightSource3d *light_source = new LightSource3d(Point3d(-300, 300, 300),
                                                    Color(255, 255, 255));
    scene->add_light_source(light_source);

    // Adding fog
    Float density = 0.00001;
    scene->set_exponential_fog(density);


    Camera camera(Point3d(0, 500, 0),
                  -1.57, 0, 3.14, //3.14
                  320);

    Canvas canvas(width, height);
    scene->render(camera, canvas);
    canvas.write_png("rendered.png");

    QImage ret = canvas.getQImage();
    delete scene;
    return ret;
}
