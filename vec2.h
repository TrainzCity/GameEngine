#ifndef VEC2_H
#define VEC2_H
/**
 * Структура двухкомпонентного вектора с целочисленными координатами.
 *
 * @param x Координата вектора по оси абсцисс.
 * @param y Координата вектора по оси ординат.
 */
typedef struct
{
    int x;
    int y;
} vec2_t;

vec2_t make_vec2(int x, int y);

int vec2_x (vec2_t v);

int vec2_y (vec2_t v);

vec2_t vec2_add(vec2_t v1, vec2_t v2);

vec2_t vec2_sub(vec2_t v1, vec2_t v2);

vec2_t vec2_scale(vec2_t v, int scalar);
#endif /* VEC2_H */