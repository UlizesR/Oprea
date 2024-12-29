#include "Vector.h"

/*========================
   2D operations
========================*/

void add_vec2(const Vec2 *a, const Vec2 *b, Vec2 *out)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void sub_vec2(const Vec2 *a, const Vec2 *b, Vec2 *out)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void mul_vec2(const Vec2 *a, double s, Vec2 *out)
{
    out->x = a->x * s;
    out->y = a->y * s;
}

void div_vec2(const Vec2 *a, double s, Vec2 *out)
{
    out->x = a->x / s;
    out->y = a->y / s;
}

double dot_vec2(const Vec2 *a, const Vec2 *b)
{
    return a->x * b->x + a->y * b->y;
}

double cross_vec2(const Vec2 *a, const Vec2 *b)
{
    // 2D "cross" product is a scalar representing the magnitude 
    // of the 3D cross in z-direction.
    return (a->x * b->y) - (a->y * b->x);
}

double norm_vec2(const Vec2 *a)
{
    return sqrt(a->x * a->x + a->y * a->y);
}

void normalize_vec2(const Vec2 *a, Vec2 *out)
{
    double n = norm_vec2(a);
    out->x = a->x / n;
    out->y = a->y / n;
}

/*========================
   3D operations
========================*/

void add_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void sub_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void mul_vec3(const Vec3 *a, double s, Vec3 *out)
{
    out->x = a->x * s;
    out->y = a->y * s;
    out->z = a->z * s;
}

void div_vec3(const Vec3 *a, double s, Vec3 *out)
{
    out->x = a->x / s;
    out->y = a->y / s;
    out->z = a->z / s;
}

double dot_vec3(const Vec3 *a, const Vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void cross_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out)
{
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

double norm_vec3(const Vec3 *a)
{
    return sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
}

void normalize_vec3(const Vec3 *a, Vec3 *out)
{
    double n = norm_vec3(a);
    out->x = a->x / n;
    out->y = a->y / n;
    out->z = a->z / n;
}