#ifndef COLOR_H
#define COLOR_H

#include <algorithm>
#include <cmath>
#include <cstdint>

typedef uint8_t Byte;

class Color {
public:
    Color() : Color(0, 0, 0) {
    }

    Color(Byte r, Byte g, Byte b) : r_(r), g_(g), b_(b) {
    }

    static Color add(const Color& a, const Color& b) {
        return a.add_(b);
    }

    static Color mix(const Color& a, const Color& b) {
        return a.mix_(b);
    }

    static Color multiply(const Color& c, double k) {
        return c.multiply_(k);
    }

    Color grayscale() const;

    Byte r() const {
        return r_;
    }

    Byte g() const {
        return g_;
    }

    Byte b() const {
        return b_;
    }

    static const Byte MAX_R = 255;
    static const Byte MAX_G = 255;
    static const Byte MAX_B = 255;

private:
    Color mix_(const Color& other) const;
    Color add_(const Color& other) const;
    Color multiply_(double k) const;

    Byte r_;
    Byte g_;
    Byte b_;
};

#endif // COLOR_H
