#include <include/color.h>

Color Color::grayscale() const {
    // https://en.wikipedia.org/wiki/Grayscale
    const Byte gray = (Byte)(r_ * 0.2126 + g_ * 0.7152 + b_ * 0.0722);
    return Color(gray, gray, gray);
}

Color Color::mix_(const Color& other) const {
    uint16_t r = (r_ * other.r_) >> 8;
    uint16_t g = (g_ * other.g_) >> 8;
    uint16_t b = (b_ * other.b_) >> 8;
    return Color((Byte) r, (Byte) g, (Byte) b);
}

Color Color::add_(const Color& other) const {
    int r = static_cast<int>(r_) + static_cast<int>(other.r_);
    int g = static_cast<int>(g_) + static_cast<int>(other.g_);
    int b = static_cast<int>(b_) + static_cast<int>(other.b_);
    return Color(std::min((int)MAX_R, r), // TODO REFAC
                 std::min((int)MAX_G, g), std::min((int)MAX_B, b));
}

Color Color::multiply_(double k) const {
    return Color(r_ * k, g_ * k, b_ * k);
}
