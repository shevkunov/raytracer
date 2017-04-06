#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <include/scene.h>

class Fog {
public:
    virtual ~Fog() {
    }

    // Required to return value from interval [0..1]
    virtual bool fog() const {
        return false;
    }

    virtual Float density(const Float &distance) const {
        (void) distance;
        return 0.;
    }
};

class ExponentialFog : public Fog {
public:
    ExponentialFog(const Float &k) : k(k) {
    }

    virtual bool fog() const {
        return true;
    }

    virtual Float density(const Float &distance) const {
        return 1. - exp(- (k) * distance);
    }
private:
    Float k;
};

