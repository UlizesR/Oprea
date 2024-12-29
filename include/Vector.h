#ifndef O_VECTOR_HPP
#define O_VECTOR_HPP

#include <math.h>   // for sqrt, etc.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents a 2D vector with double precision.
 */
typedef struct Vec2
{
    double x;
    double y;
} Vec2;

/**
 * Represents a 3D vector with double precision.
 */
typedef struct Vec3
{
    double x;
    double y;
    double z;
} Vec3;

/**
 * Represents a 4D vector with double precision.
 */
typedef struct Vec4
{
    double x;
    double y;
    double z;
    double w;
} Vec4;

/**
 * Represents a function-valued 2D vector:
 * x(t), y(t).
 */
typedef struct FuncVec2
{
    double (*x)(double);
    double (*y)(double);
} FuncVec2;

/**
 * Represents a function-valued 3D vector:
 * x(t), y(t), z(t).
 */
typedef struct FuncVec3
{
    double (*x)(double);
    double (*y)(double);
    double (*z)(double);
} FuncVec3;

/*---------------------------
   Vector arithmetic (2D)
 ---------------------------*/
void add_vec2(const Vec2 *a, const Vec2 *b, Vec2 *out);
void sub_vec2(const Vec2 *a, const Vec2 *b, Vec2 *out);
void mul_vec2(const Vec2 *a, double s, Vec2 *out);
void div_vec2(const Vec2 *a, double s, Vec2 *out);
double dot_vec2(const Vec2 *a, const Vec2 *b);
double cross_vec2(const Vec2 *a, const Vec2 *b);
double norm_vec2(const Vec2 *a);
void normalize_vec2(const Vec2 *a, Vec2 *out);

/*---------------------------
   Vector arithmetic (3D)
 ---------------------------*/
void add_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out);
void sub_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out);
void mul_vec3(const Vec3 *a, double s, Vec3 *out);
void div_vec3(const Vec3 *a, double s, Vec3 *out);
double dot_vec3(const Vec3 *a, const Vec3 *b);
void cross_vec3(const Vec3 *a, const Vec3 *b, Vec3 *out);
double norm_vec3(const Vec3 *a);
void normalize_vec3(const Vec3 *a, Vec3 *out);

#ifdef __cplusplus
}
#endif

#endif // O_VECTOR_HPP