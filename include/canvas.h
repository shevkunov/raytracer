#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <QPainter>
#include <QPicture>

#include <include/color.h>

class Canvas {
public:
    Canvas(size_t width, size_t height);
    Canvas grayscale();
    Canvas detect_edges();

    void set_pixel(int x, int y, Color c) {
        img_.setPixelColor(x, y, QColor(c.r(), c.g(), c.b()));
    }

    Color get_pixel(int x, int y);

    QImage getQImage() {
        return img_;
    }

    size_t width() {
        return img_.width();
    }

    size_t height() {
        return img_.height();
    }

    void read_png(char * file_name);
    void write_png(char file_name[]);
    void clear();
private:
    QImage img_;
};

#endif //__CANVAS_H__
