#ifndef MATH_LINEAR
#define MATH_LINEAR

#include <math.h>

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef float mat4x4[4][4];

inline void vec3_sub(vec3 r, vec3 const a, vec3 const b) {
    int i;
    for (i = 0; i < 3; i++)
        r[i] = a[i] - b[i];
}

inline void vec3_scale(vec3 r, vec3 const v, float const s) {
    int i;
    for (i = 0; i < 3; i++)
        r[i] = v[i] * s;
}

inline float vec3_mul_inner(vec3 const a, vec3 const b) {
    float f = 0.0f;
    int i;
    for (i = 0; i < 3; i++)
        f += b[i] * a[i];

    return f;
}

inline float vec3_len(vec3 const v) {
    return (float)sqrt(vec3_mul_inner(v, v));
}

inline void vec3_norm(vec3 r, vec3 const v)
{
    float k = 1.0f / vec3_len(v);
    vec3_scale(r, v, k);
}

inline void vec4_scale(vec3 r, vec3 const v, float const s) {
    int i;
    for (i = 0; i < 4; i++)
        r[i] = v[i] * s;
}

inline void mat4x4_identity(mat4x4 M) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            M[i][j] = i == j ? 1.0f : 0.0f;
}

inline void mat4x4_translate(mat4x4 T, float x, float y, float z) {
    mat4x4_identity(T);
    T[3][0] = x;
    T[3][1] = y;
    T[3][2] = z;
}

inline void mat4x4_dup(mat4x4 R, mat4x4 M) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            R[i][j] = M[i][j];
}

inline void mat4x4_orthonormalize(mat4x4 R, mat4x4 M) {
    float s = 1.0;
    vec3 h;

    mat4x4_dup(R, M);
    vec3_norm(R[2], R[2]);

    s = vec3_mul_inner(R[1], R[2]);
    vec3_scale(h, R[2], s);
    vec3_sub(R[1], R[1], h);
    vec3_norm(R[2], R[2]);

    s = vec3_mul_inner(R[1], R[2]);
    vec3_scale(h, R[2], s);
    vec3_sub(R[1], R[1], h);
    vec3_norm(R[1], R[1]);

    s = vec3_mul_inner(R[0], R[1]);
    vec3_scale(h, R[1], s);
    vec3_sub(R[0], R[0], h);
    vec3_norm(R[0], R[0]);
}

inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f) {
    M[0][0] = 2.f / (r - l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;

    M[1][1] = 2.f / (t - b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;

    M[2][2] = -2.f / (f - n);
    M[2][0] = M[2][1] = M[2][3] = 0.f;

    M[3][0] = -(r + l) / (r - l);
    M[3][1] = -(t + b) / (t - b);
    M[3][2] = -(f + n) / (f - n);
    M[3][3] = 1.f;
}

inline void mat4x4_scale_aniso(mat4x4 M, mat4x4 a, float x, float y, float z) {
    int i;
    vec4_scale(M[0], a[0], x);
    vec4_scale(M[1], a[1], y);
    vec4_scale(M[2], a[2], z);
    for (i = 0; i < 4; i++)
        M[3][i] = a[3][i];
}

#endif
