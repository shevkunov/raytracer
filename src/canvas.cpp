#include <include/canvas.h>
#include <include/color.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <stdarg.h>

#include <math.h>


Canvas::Canvas(const char * const file_name) {
    read_png(file_name);
}

Canvas::Canvas(size_t width, size_t height)
    : img_(QImage(width, height, QImage::Format_RGB32)) {
}

void Canvas::clear() {
    img_.fill(QColor(0, 0, 0));
}

void Canvas::write_png(const char * const file_name) const {
    img_.save(file_name);
}

void Canvas::read_png(const char * const file_name) {
    img_.load(file_name);
}

Canvas Canvas::grayscale() const {
    const int w = img_.width();
    const int h = img_.height();
    Canvas ret(w, h);

    for(int x = 0; x < w; ++x) {
        for(int y = 0; y < h; ++y) {
            const Color gray = get_pixel(x, y).grayscale();
            ret.set_pixel(x, y, gray);
        }
    }

    return ret;
}

// Edges detection for antialiasing
// See: http://en.wikipedia.org/wiki/Sobel_operator

int mattrix_x[3][3] =
    {{-1, 0, 1},
     {-2, 0, 2},
     {-1, 0, 1}};

int mattrix_y[3][3] =
    {{-1, -2, -1},
     { 0,  0,  0},
     { 1,  2,  1}};

Canvas Canvas::detect_edges() const {
    Canvas grayscaled_canv = grayscale();
    const int w = img_.width();
    const int h = img_.height();
    Canvas grad_canv(w, h);
    
    for(int x = 1; x < w - 1; ++x) {
        for(int y = 1; y < h - 1; ++y) {
            int gx = 0;
            for(int i = -1; i < 2; ++i) {
                for(int j = -1; j < 2; ++j) {
                    gx += mattrix_x[i + 1][j + 1] *
                            grayscaled_canv.get_pixel(x + i, y + j).r();
                }
            }
            
            int gy = 0;
            for(int i = -1; i < 2; ++i) {
                for(int j = -1; j < 2; ++j) {
                    gy += mattrix_y[i + 1][j + 1] *
                            grayscaled_canv.get_pixel(x + i, y + j).r();
                }
            }
            
            Byte grad = (Byte) sqrt(gx * gx + gy * gy);
            grad_canv.set_pixel(x, y, Color(grad, grad, grad));
        }
    }
    
    return grad_canv;
}

Color Canvas::get_pixel(int x, int y) const {
    if ((x < 0) || (x > img_.width()) ||
            (y < 0) || (y > img_.height())) {
        return Color(0, 0, 0);
    }
    QColor c = img_.pixel(x, y);
    int r, g, b;
    c.getRgb(&r, &g, &b);
    return Color(r, g, b);
}
