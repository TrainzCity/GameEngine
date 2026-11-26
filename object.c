#include "object.h"
#include <stdlib.h>

static object_t *g_obj_head = NULL;
static object_t *g_obj_tail = NULL;
static int g_collision_depth = 0;

/**
 * @brief Немедленно исключает объект из списка и освобождает его память и спрайт.
 */
static void destroy_object_now(object_t *obj)
{
    if (!obj) {
        return;
    }

    if (obj->prev) {
        obj->prev->next = obj->next;
    } else {
        g_obj_head = obj->next;
    }

    if (obj->next) {
        obj->next->prev = obj->prev;
    } else {
        g_obj_tail = obj->prev;
    }

    if (obj->sprite) {
        remove_sprite(obj->sprite);
        obj->sprite = NULL;
    }

    free(obj);
}

/**
 * @brief Удаляет все объекты, помеченные флагом pending_destroy.
 */
static void flush_destroyed_objects(void)
{
    object_t *cur = g_obj_head;
    while (cur != NULL) {
        object_t *next = cur->next;
        if (cur->pending_destroy) {
            destroy_object_now(cur);
        }
        cur = next;
    }
}

object_t *add_object(int type, sprite_t *sprite, bool solid, bool is_static)
{
    object_t *obj = (object_t *)malloc(sizeof(object_t));
    if (!obj) {
        return NULL;
    }

    obj->type = type;
    obj->sprite = sprite;
    if (sprite) {
        obj->pos = graphic_object_get_pos(sprite->gobj);
        obj->speed = graphic_object_get_speed(sprite->gobj);
    } else {
        obj->pos = make_vec2(0, 0);
        obj->speed = make_vec2(0, 0);
    }

    obj->hitbox.count = 0;
    if (sprite && sprite->img && sprite->img->width > 0 && sprite->img->height > 0) {
        obj->hitbox.count = 1;
        obj->hitbox.parts[0].x = 0;
        obj->hitbox.parts[0].y = 0;
        obj->hitbox.parts[0].w = sprite->img->width;
        obj->hitbox.parts[0].h = sprite->img->height;
    }

    obj->fsm = NULL;
    obj->state = 0;
    obj->solid = solid;
    obj->is_static = is_static;
    obj->active = true;
    obj->pending_destroy = false;
    obj->user_data = NULL;
    obj->next = NULL;
    obj->prev = NULL;

    if (g_obj_head == NULL) {
        g_obj_head = obj;
        g_obj_tail = obj;
    } else {
        obj->prev = g_obj_tail;
        g_obj_tail->next = obj;
        g_obj_tail = obj;
    }

    return obj;
}

void remove_object(object_t *obj)
{
    if (!obj || obj->pending_destroy) {
        return;
    }

    if (g_collision_depth > 0) {
        obj->pending_destroy = true;
        obj->active = false;
    } else {
        destroy_object_now(obj);
    }
}

void clear_objects(void)
{
    if (g_collision_depth > 0) {
        for (object_t *cur = g_obj_head; cur != NULL; cur = cur->next) {
            cur->pending_destroy = true;
            cur->active = false;
        }
        return;
    }

    object_t *cur = g_obj_head;
    while (cur != NULL) {
        object_t *next = cur->next;
        destroy_object_now(cur);
        cur = next;
    }
    g_obj_head = NULL;
    g_obj_tail = NULL;
}

object_t *get_objects_head(void)
{
    return g_obj_head;
}

void object_set_hitbox(object_t *obj, figure_t hitbox)
{
    if (!obj) {
        return;
    }
    if (figure_valid(hitbox)) {
        obj->hitbox = hitbox;
    }
}

void object_set_hitbox_rect(object_t *obj, int offset_x, int offset_y, int w, int h)
{
    if (!obj) {
        return;
    }
    rect_t r = { offset_x, offset_y, w, h };
    if (!rect_valid(r)) {
        return;
    }
    obj->hitbox.count = 1;
    obj->hitbox.parts[0] = r;
}

vec2_t object_get_pos(const object_t *obj)
{
    if (!obj) {
        return make_vec2(0, 0);
    }
    if (obj->sprite) {
        return graphic_object_get_pos(obj->sprite->gobj);
    }
    return obj->pos;
}

void object_set_pos(object_t *obj, vec2_t pos)
{
    if (!obj) {
        return;
    }
    obj->pos = pos;
    if (obj->sprite) {
        graphic_object_set_pos(&obj->sprite->gobj, pos);
    }
}

void move_object(object_t *obj, int dx, int dy)
{
    if (!obj) {
        return;
    }
    vec2_t cur = object_get_pos(obj);
    object_set_pos(obj, vec2_add(cur, make_vec2(dx, dy)));
}

vec2_t object_get_speed(const object_t *obj)
{
    if (!obj) {
        return make_vec2(0, 0);
    }
    if (obj->sprite) {
        return graphic_object_get_speed(obj->sprite->gobj);
    }
    return obj->speed;
}

void object_set_speed(object_t *obj, vec2_t speed)
{
    if (!obj) {
        return;
    }
    obj->speed = speed;
    if (obj->sprite) {
        graphic_object_set_speed(&obj->sprite->gobj, speed);
    }
}

void object_set_fsm(object_t *obj, automata_t *fsm, int initial_state)
{
    if (!obj) {
        return;
    }
    obj->fsm = fsm;
    obj->state = initial_state;
}

figure_t object_get_world_hitbox(const object_t *obj)
{
    figure_t world;
    world.count = 0;

    if (!obj || obj->hitbox.count <= 0) {
        return world;
    }

    vec2_t pos = object_get_pos(obj);
    int n = obj->hitbox.count;
    if (n > MAX_PARTS) {
        n = MAX_PARTS;
    }

    world.count = n;
    for (int i = 0; i < n; i++) {
        world.parts[i].x = obj->hitbox.parts[i].x + vec2_x(pos);
        world.parts[i].y = obj->hitbox.parts[i].y + vec2_y(pos);
        world.parts[i].w = obj->hitbox.parts[i].w;
        world.parts[i].h = obj->hitbox.parts[i].h;
    }

    return world;
}

bool objects_collide(const object_t *o1, const object_t *o2)
{
    if (!o1 || !o2 || o1 == o2) {
        return false;
    }
    if (!o1->active || !o2->active || o1->pending_destroy || o2->pending_destroy) {
        return false;
    }
    if (o1->hitbox.count <= 0 || o2->hitbox.count <= 0) {
        return false;
    }

    figure_t f1 = object_get_world_hitbox(o1);
    figure_t f2 = object_get_world_hitbox(o2);
    return figures_intersect(f1, f2);
}

/**
 * @brief Вычисляет вектор минимального выталкивания (MTV) для разведения r1 от r2.
 */
static void compute_rect_mtv(rect_t r1, rect_t r2, int *out_dx, int *out_dy)
{
    int overlap_left  = (r1.x + r1.w) - r2.x;
    int overlap_right = (r2.x + r2.w) - r1.x;
    int overlap_top   = (r1.y + r1.h) - r2.y;
    int overlap_bot   = (r2.y + r2.h) - r1.y;

    int push_x = (overlap_left < overlap_right) ? -overlap_left : overlap_right;
    int push_y = (overlap_top < overlap_bot) ? -overlap_top : overlap_bot;

    int abs_x = (push_x < 0) ? -push_x : push_x;
    int abs_y = (push_y < 0) ? -push_y : push_y;

    if (abs_x < abs_y) {
        *out_dx = push_x;
        *out_dy = 0;
    } else {
        *out_dx = 0;
        *out_dy = push_y;
    }
}

/**
 * @brief Гасит составляющую скорости объекта, направленную внутрь препятствия.
 */
static void cancel_velocity_against_push(object_t *obj, int push_x, int push_y)
{
    vec2_t sp = object_get_speed(obj);
    int sx = vec2_x(sp);
    int sy = vec2_y(sp);

    if ((push_x < 0 && sx > 0) || (push_x > 0 && sx < 0)) {
        sx = 0;
    }
    if ((push_y < 0 && sy > 0) || (push_y > 0 && sy < 0)) {
        sy = 0;
    }

    object_set_speed(obj, make_vec2(sx, sy));
}

/**
 * @brief Автоматически разрешает физическое проникновение двух твёрдых объектов.
 */
static void resolve_solid_collision(object_t *o1, object_t *o2)
{
    if (!o1->solid || !o2->solid) {
        return;
    }
    if (o1->is_static && o2->is_static) {
        return;
    }

    for (int iter = 0; iter < MAX_PARTS; iter++) {
        figure_t f1 = object_get_world_hitbox(o1);
        figure_t f2 = object_get_world_hitbox(o2);

        bool found = false;
        int best_dx = 0;
        int best_dy = 0;
        int best_mag = -1;

        for (int i = 0; i < f1.count; i++) {
            for (int j = 0; j < f2.count; j++) {
                if (rect_intorset(f1.parts[i], f2.parts[j])) {
                    int dx = 0;
                    int dy = 0;
                    compute_rect_mtv(f1.parts[i], f2.parts[j], &dx, &dy);
                    int mag = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
                    if (!found || mag > best_mag) {
                        found = true;
                        best_dx = dx;
                        best_dy = dy;
                        best_mag = mag;
                    }
                }
            }
        }

        if (!found) {
            break;
        }

        if (!o1->is_static && o2->is_static) {
            move_object(o1, best_dx, best_dy);
            cancel_velocity_against_push(o1, best_dx, best_dy);
        } else if (o1->is_static && !o2->is_static) {
            move_object(o2, -best_dx, -best_dy);
            cancel_velocity_against_push(o2, -best_dx, -best_dy);
        } else {
            int d1x = best_dx / 2;
            int d1y = best_dy / 2;
            int d2x = -(best_dx - d1x);
            int d2y = -(best_dy - d1y);
            move_object(o1, d1x, d1y);
            move_object(o2, d2x, d2y);
            cancel_velocity_against_push(o1, best_dx, best_dy);
            cancel_velocity_against_push(o2, -best_dx, -best_dy);
        }
    }
}

void process_collisions(void)
{
    if (!g_obj_head) {
        return;
    }

    object_t *last_at_start = g_obj_tail;
    g_collision_depth++;

    for (object_t *a = g_obj_head; a != NULL; ) {
        bool is_last_a = (a == last_at_start);

        if (a->active && !a->pending_destroy) {
            for (object_t *b = a->next; b != NULL; ) {
                bool is_last_b = (b == last_at_start);

                if (!a->active || a->pending_destroy) {
                    break;
                }

                if (b->active && !b->pending_destroy && objects_collide(a, b)) {
                    resolve_solid_collision(a, b);
                    dispatch_reaction(a, b);
                }

                if (is_last_b) {
                    break;
                }
                b = b->next;
            }
        }

        if (is_last_a) {
            break;
        }
        a = a->next;
    }

    g_collision_depth--;
    if (g_collision_depth == 0) {
        flush_destroyed_objects();
    }
}

void update_objects(void)
{
    for (object_t *cur = g_obj_head; cur != NULL; cur = cur->next) {
        if (!cur->active || cur->pending_destroy || cur->is_static) {
            continue;
        }
        vec2_t sp = object_get_speed(cur);
        if (vec2_x(sp) != 0 || vec2_y(sp) != 0) {
            move_object(cur, vec2_x(sp), vec2_y(sp));
        }
    }

    process_collisions();
}
