#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <QPainter>
#include <QPicture>

#include <include/color.h>

class Canvas {
public:
    Canvas(const char * const file_name);
    Canvas(size_t width, size_t height);
    Canvas grayscale() const;
    Canvas detect_edges() const;

    void set_pixel(int x, int y, Color c) {
        img_.setPixelColor(x, y, QColor(c.r(), c.g(), c.b()));
    }

    Color get_pixel(int x, int y) const;

    QImage getQImage() {
        return img_;
    }

    size_t width() const {
        return img_.width();
    }

    size_t height() const {
        return img_.height();
    }

    void read_png(const char * const file_name);
    void write_png(const char * const file_name) const;
    void clear();
private:
    QImage img_;
};

#endif //__CANVAS_H__
