// taken from mncc8337/rey-treycer with some modifications

#ifndef VEC3_H
#define VEC3_H

#include <math.h>

class Vec3 {
public:
    double x = 0;
    double y = 0;
    double z = 0;

    Vec3(double a, double b, double c) {
        x = a;
        y = b;
        z = c;
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    Vec3& operator+=(const Vec3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vec3& operator-=(const Vec3 &v) {
        return *this += -v;
    }
    Vec3 operator*=(const double t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }
    Vec3 operator/=(const double t) {
        return *this *= 1/t;
    }
    bool operator==(const Vec3 &v) {
        Vec3 u = *this;
        return u.x == v.x and u.y == v.y and u.z == v.z;
    }
    bool operator!=(const Vec3 &v) {
        return !(*this == v);
    }
    double dot(Vec3 v) {
        return x * v.x + y * v.y + z * v.z;
    }
    double squared_length() {
        return dot(*this);
    }
    double length() {
        return sqrt(dot(*this));
    }
    Vec3 cross(Vec3 v) {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    Vec3 normalize() {
        const double l = length();
        return Vec3(x / l, y / l, z / l);
    }
};

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return u + (-v);
}
inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}
inline Vec3 operator*(const Vec3 &u, const double t) {
    return Vec3(u.x * t, u.y * t, u.z * t);
}
inline Vec3 operator*(const double t, const Vec3 &u) {
    return u * t;
}
inline Vec3 operator/(const Vec3 v, const double t) {
    return v * (1/t);
}
inline Vec3 operator/(const double t, const Vec3 v) {
    return Vec3(t/v.x, t/v.y, t/v.z);
}
inline Vec3 operator/(const Vec3 u, const Vec3 v) {
    return u * (1/v);
}
inline Vec3 lerp(const Vec3 u, const Vec3 v, const double t) {
    return u * (1-t) + v * t;
}

#endif
