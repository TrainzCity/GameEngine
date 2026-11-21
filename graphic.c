#include <stdio.h>
#include <stdlib.h>
#include "graphic.h"
#include "vec2.h"
#include "animation.h"
#include "backend.h"

#define TEST_GRAPHIC_OBJECT \
    vec2_t pos_test = make_vec2(1, 1); \
    vec2_t speed_test = make_vec2(2, 2); \
    char *frames_test[] = {"1"}; \
    animation_t *anim_test = make_animation(frames_test, 1); \
    graphic_object_t o_test = make_graphic_object(pos_test, speed_test, anim_test)

#define ASSERT(c) \
    do { \
        if (c) \
            printf("OK\n"); \
        else \
            printf("Fail\n"); \
    } while (0)

// Макрос для создания тестовых спрайтов (texture = NULL, чтобы не крашить SDL)
#define TEST_SPRITES \
    image_t img1 = { NULL, 64, 64 }; \
    image_t img2 = { NULL, 64, 64 }; \
    sprite_t *s1 = add_sprite(&img1, 10, 10, 1); \
    sprite_t *s2 = add_sprite(&img2, 50, 50, 0)

/**
 * Создает объект с заданной анимацией.
 */
graphic_object_t make_graphic_object(vec2_t v, vec2_t v_speed, animation_t *a) {
    graphic_object_t ob;
    ob.pos = v;
    ob.speed = v_speed;
    ob.animation = a;
    return ob;
}

/**
 * Задает анимацию объекту.
 */
void set_animation(graphic_object_t *ob, animation_t *a) {
    if (!ob) return;
    ob->animation = a;
}

/**
 * Получает текущую анимацию.
 */
animation_t *get_animation(graphic_object_t ob) {
    return ob.animation;
}

/**
 * Устанавливает позицию объекту.
 */
void graphic_object_set_pos(graphic_object_t *ob, vec2_t v) {
    if (!ob) return;
    ob->pos = v;
}

/**
 * Получает текущую позицию.
 */
vec2_t graphic_object_get_pos(graphic_object_t ob) {
    return ob.pos;
}

/**
 * Передвигает графический объект со скоростью.
 */
void move_graphic_object(graphic_object_t *ob) {
    if (!ob) return;
    ob->pos = vec2_add(ob->pos, ob->speed);
}

/**
 * Устанавливает скорость графическому объекту.
 */
void graphic_object_set_speed(graphic_object_t *ob, vec2_t v) {
    if (!ob) return;
    ob->speed = v;
}

/**
 * Получает текущую скорость.
 */
vec2_t graphic_object_get_speed(graphic_object_t ob) {
    return ob.speed;
}

/**
 * Двусвязный список спрайтов.
 */
static sprite_t *head = NULL;
static sprite_t *tail = NULL;

/**
 * Создание спрайта.
 */
sprite_t *add_sprite(image_t *img, int x, int y, int z) {
    sprite_t *s = (sprite_t *)malloc(sizeof(sprite_t));
    if (s == NULL) return NULL;

    vec2_t pos = make_vec2(x, y);
    vec2_t speed = make_vec2(0, 0);

    s->gobj = make_graphic_object(pos, speed, NULL);
    s->img = img;
    s->z = z;
    s->next = NULL;
    s->prev = NULL;

    if (head == NULL) {
        head = s;
        tail = s;
    } else {
        s->prev = tail;
        tail->next = s;
        tail = s;
    }
    return s;
}

/**
 * Удаление конкретного спрайта из списка.
 */
void remove_sprite(sprite_t *s) {
    if (!s) return;

    if (s->prev) {
        s->prev->next = s->next;
    } else {
        head = s->next;
    }

    if (s->next) {
        s->next->prev = s->prev;
    } else {
        tail = s->prev;
    }

    free(s);
}

/**
 * Полная очистка списка спрайтов.
 */
void clear_sprites(void) {
    sprite_t *spr = head;
    while (spr != NULL) {
        sprite_t *next = spr->next;
        free(spr);
        spr = next;
    }
    head = NULL;
    tail = NULL;
}

/**
 * Перемещение спрайта.
 */
void move_sprite(sprite_t *s, int dx, int dy, int dz) {
    if (!s) return;
    vec2_t new_pos = make_vec2(s->gobj.pos.x + dx, s->gobj.pos.y + dy);
    graphic_object_set_pos(&s->gobj, new_pos);
    s->z += dz;
}

/**
 * Печать спрайтов в порядке возрастания слоя z (поддерживает любые значения int).
 */
void print_sprites(void) {
    if (head == NULL) return;

    int min_z = head->z;
    int max_z = head->z;
    for (sprite_t *spr = head->next; spr != NULL; spr = spr->next) {
        if (spr->z < min_z) min_z = spr->z;
        if (spr->z > max_z) max_z = spr->z;
    }

    int current_z = min_z;
    while (1) {
        int next_z = max_z;
        int has_next = 0;

        for (sprite_t *spr = head; spr != NULL; spr = spr->next) {
            if (spr->z == current_z) {
                image_draw(spr->img, (int)spr->gobj.pos.x, (int)spr->gobj.pos.y);
            } else if (spr->z > current_z) {
                if (!has_next || spr->z < next_z) {
                    next_z = spr->z;
                    has_next = 1;
                }
            }
        }

        if (!has_next) break;
        current_z = next_z;
    }
}

void test_make_graphic_object(void) {
    TEST_GRAPHIC_OBJECT;

    printf("Тестирование создания графического объекта: ");
    ASSERT(o_test.pos.x == pos_test.x &&
           o_test.pos.y == pos_test.y &&
           o_test.speed.x == speed_test.x &&
           o_test.speed.y == speed_test.y &&
           o_test.animation == anim_test);
}

void test_set_and_get_animation(void) {
    TEST_GRAPHIC_OBJECT;

    animation_t *anim_test2 = make_animation(frames_test, 1);
    set_animation(&o_test, anim_test2);

    printf("Тестирование установки анимации графического объекта: ");
    ASSERT(o_test.animation == anim_test2);

    printf("Тестирование получения анимации графического объекта: ");
    ASSERT(get_animation(o_test) == anim_test2);
}

void test_graphic_object_set_and_get_pos(void) {
    TEST_GRAPHIC_OBJECT;

    vec2_t pos_test2 = make_vec2(3, 3);
    graphic_object_set_pos(&o_test, pos_test2);

    printf("Тестирование установки позиции графического объекта: ");
    ASSERT(o_test.pos.x == pos_test2.x && o_test.pos.y == pos_test2.y);

    printf("Тестирование получения позиции графического объекта: ");
    ASSERT(graphic_object_get_pos(o_test).x == pos_test2.x &&
           graphic_object_get_pos(o_test).y == pos_test2.y);
}

void test_move_graphic_object(void) {
    TEST_GRAPHIC_OBJECT;

    move_graphic_object(&o_test);

    printf("Тестирование передвижения графического объекта: ");
    ASSERT(graphic_object_get_pos(o_test).x == vec2_add(pos_test, speed_test).x &&
           graphic_object_get_pos(o_test).y == vec2_add(pos_test, speed_test).y);
}

void test_graphic_object_set_and_get_speed(void) {
    TEST_GRAPHIC_OBJECT;

    vec2_t speed_test2 = make_vec2(3, 3);
    graphic_object_set_speed(&o_test, speed_test2);

    printf("Тестирование установки скорости графического объекта: ");
    ASSERT(o_test.speed.x == speed_test2.x && o_test.speed.y == speed_test2.y);

    printf("Тестирование получения скорости графического объекта: ");
    ASSERT(graphic_object_get_speed(o_test).x == speed_test2.x &&
           graphic_object_get_speed(o_test).y == speed_test2.y);
}

void test_add_sprite(void) {
    clear_sprites();
    TEST_SPRITES;
    (void)s2;

    printf("Тестирование создания спрайта: ");
    ASSERT(s1 != NULL);

    printf("Тестирование добавления в список: ");
    ASSERT(head == s1);

    clear_sprites();
}

void test_move_sprite(void) {
    clear_sprites();
    TEST_SPRITES;
    (void)s2;

    move_sprite(s1, 5, 5, 0);

    printf("Тестирование перемещения спрайта по х: ");
    ASSERT(s1->gobj.pos.x == 15);

    printf("Тестирование перемещения спрайта по у: ");
    ASSERT(s1->gobj.pos.y == 15);

    clear_sprites();
}

void test_sprite_z(void) {
    clear_sprites();
    TEST_SPRITES;
    (void)s1;
    (void)s2;

    printf("Тестирование порядка слоев:\n");
    print_sprites();

    clear_sprites();
}
