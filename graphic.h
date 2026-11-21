#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "vec2.h"
#include "animation.h"
#include "backend.h"

// Структура графического объекта.
typedef struct {
    vec2_t pos;
    vec2_t speed;
    animation_t *animation;
} graphic_object_t;

// Структура спрайта.
typedef struct sprite {
    image_t *img;
    graphic_object_t gobj;
    int z;

    struct sprite *next;
    struct sprite *prev;
} sprite_t;

// Добавляет спрайт.
sprite_t *add_sprite(image_t *img, int x, int y, int z);

// Удаляет спрайт из списка и освобождает его память.
void remove_sprite(sprite_t *s);

// Удаляет все спрайты и освобождает память списка.
void clear_sprites(void);

// Передвигает спрайт.
void move_sprite(sprite_t *s, int dx, int dy, int dz);

// Отображение спрайтов.
void print_sprites(void);

// Создает объект с заданной анимацией.
graphic_object_t make_graphic_object(vec2_t v, vec2_t speed, animation_t *a);

// Задает анимацию объекту.
void set_animation(graphic_object_t *ob, animation_t *a);

// Получает текущую анимацию.
animation_t *get_animation(graphic_object_t ob);

// Устанавливает позицию объекту.
void graphic_object_set_pos(graphic_object_t *ob, vec2_t v);

// Получает текущую позицию.
vec2_t graphic_object_get_pos(graphic_object_t ob);

// Передвигает графический объект со скоростью.
void move_graphic_object(graphic_object_t *ob);

// Устанавливает скорость графическому объекту.
void graphic_object_set_speed(graphic_object_t *ob, vec2_t v);

// Получает текущую скорость.
vec2_t graphic_object_get_speed(graphic_object_t ob);

// Тестирование создания графического объекта.
void test_make_graphic_object(void);

// Тестирование установки и получения анимации графического объекта.
void test_set_and_get_animation(void);

// Тестирование установки и получения позиции графического объекта.
void test_graphic_object_set_and_get_pos(void);

// Тестирование передвижения графического объекта.
void test_move_graphic_object(void);

// Тестирование установки и получения скорости графического объекта.
void test_graphic_object_set_and_get_speed(void);

// Тестирование добавления спрайта.
void test_add_sprite(void);

// Тестирование перемещения спрайта.
void test_move_sprite(void);

// Тестирование порядка слоев.
void test_sprite_z(void);

#endif
