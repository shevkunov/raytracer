#include <stdlib.h>
#include <string.h>

#include <include/render.h>
#include <include/canvas.h>
#include <include/color.h>

#define ANTIALIASING 1

#include <omp.h>

#define CHUNK 10

/* collapse is a feature from OpenMP 3 (2008) */
#if _OPENMP < 200805
    #define collapse(x) 
#endif

#ifdef RAY_INTERSECTIONS_STAT
extern long
intersections_per_ray;
#endif // RAY_INTERSECTIONS_STAT

#include <stdio.h>


void render_scene(const Scene * const scene,
             const Camera * const camera,
             Canvas& canvas) {
    
    const int w = canvas.width();
    const int h = canvas.height();
    const Float dx = w / 2.0;
    const Float dy = h / 2.0;
    const Float focus = camera->proj_plane_dist;
    
    // TODO: consider possibility to define these OpenMP parameters
    // in declarative style (using directives of preprocessor)
    //omp_set_num_threads((num_threads < 2) ? 1 : num_threads);
    
    #ifdef RAY_INTERSECTIONS_STAT
    // intersections_per_ray is not atomic variable
    // avoid multithreaded rendering to prevent from race-conditions
    // in case of incrementing this variable
    omp_set_num_threads(1);
    intersections_per_ray = 0;
    #endif // RAY_INTERSECTIONS_STAT
    
    int i;
    int j;
    // #pragma omp parallel private(i, j)
    // #pragma omp for collapse(2) schedule(dynamic, CHUNK)
    for(i = 0; i < w; i++) {
        for(j = 0; j < h; j++) {
            const Float x = i - dx;
            const Float y = j - dy;
            const Vector3d ray = Vector3d(x, y, focus);
            const Color col = trace(scene, camera, ray);
            canvas.set_pixel(i, j, col);
        }
    }
    
    // TODO: argument of the function? global variable?
    const int antialiasing = ANTIALIASING;
    
    if(antialiasing) {
        Canvas edges = canvas.detect_edges();
        //#pragma omp parallel private(i, j)
        //#pragma omp for collapse(2) schedule(dynamic, CHUNK)
        for(i = 1; i < w - 1; i++) {
            for(j = 1; j < h - 1; j++) {
                // edges canvas is grayscaled
                // it means that color components (r, g, b) are equal
                Byte gray = edges.get_pixel(i, j).r();
            
                // TODO: improve
                if(gray > 10) {
                    const Float x = i - dx;
                    const Float y = j - dy;
                
                    Color c = canvas.get_pixel(i, j);
                
                    const Float weight = 1.0 / 4;/*
                    c = mul_color(c, weight);
                    c = add_colors(c, mul_color(trace(scene, camera, Vector3d(x + 0.5, y, focus)), weight));
                    c = add_colors(c, mul_color(trace(scene, camera, Vector3d(x, y + 0.5, focus)), weight));
                    c = add_colors(c, mul_color(trace(scene, camera, Vector3d(x + 0.5, y + 0.5, focus)), weight));
                */
                    c = Color::multiply(c, weight);
                    c = Color::add(c,
                                   Color::multiply(trace(scene, camera, Vector3d(x + 0.5, y, focus)),
                                                   weight)
                                   );
                    c = Color::add(c,
                                   Color::multiply(trace(scene, camera, Vector3d(x, y + 0.5, focus)),
                                                   weight)
                                   );
                    c = Color::add(c,
                                   Color::multiply(trace(scene, camera, Vector3d(x + 0.5, y + 0.5, focus)),
                                                   weight)
                                   );
                    canvas.set_pixel(i, j, c);
                }
            }
        }
    }
    
    #ifdef RAY_INTERSECTIONS_STAT
    intersections_per_ray /= (w * h);
    printf("Average intersections number per pixel: %li\n", intersections_per_ray);
    #endif // RAY_INTERSECTIONS_STAT
}
