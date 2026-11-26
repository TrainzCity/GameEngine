#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "object.h"

enum {
    T_PLAYER = 10,
    T_COIN   = 20,
    T_WALL   = 30,
    T_ENEMY  = -5
};

static int g_call_count = 0;
static int g_last_o1_type = 0;
static int g_last_o2_type = 0;
static int g_score = 0;

static void reset_counters(void)
{
    g_call_count = 0;
    g_last_o1_type = 0;
    g_last_o2_type = 0;
    g_score = 0;
}

static void react_record(object_t *o1, object_t *o2)
{
    g_call_count++;
    g_last_o1_type = o1->type;
    g_last_o2_type = o2->type;
}

static void react_collect_coin(object_t *player, object_t *coin)
{
    assert(player->type == T_PLAYER);
    assert(coin->type == T_COIN);
    g_call_count++;
    g_score += 100;
    remove_object(coin); /* Отложенное удаление внутри коллбэка */
}

static void react_spawn_on_hit(object_t *o1, object_t *o2)
{
    (void)o1;
    (void)o2;
    g_call_count++;
    /* Спавн нового объекта прямо внутри коллбэка не должен зациклить process_collisions */
    object_t *spawned = add_object(T_COIN, NULL, false, true);
    object_set_pos(spawned, make_vec2(0, 0));
    object_set_hitbox_rect(spawned, 0, 0, 50, 50);
}

static void test_symmetry_and_argument_order(void)
{
    clear_objects();
    clear_reactions();
    reset_counters();

    /* Создаём монету ДО игрока, но регистрируем (T_PLAYER, T_COIN) */
    object_t *coin = add_object(T_COIN, NULL, false, true);
    object_t *player = add_object(T_PLAYER, NULL, false, false);
    object_set_pos(coin, make_vec2(10, 10));
    object_set_hitbox_rect(coin, 0, 0, 20, 20);
    object_set_pos(player, make_vec2(15, 15));
    object_set_hitbox_rect(player, 0, 0, 20, 20);

    set_reaction(T_PLAYER, T_COIN, react_record);
    process_collisions();

    assert(g_call_count == 1);
    assert(g_last_o1_type == T_PLAYER);
    assert(g_last_o2_type == T_COIN);

    /* Перезапись в обратном порядке (T_COIN, T_PLAYER) меняет ожидаемый порядок аргументов */
    reset_counters();
    set_reaction(T_COIN, T_PLAYER, react_record);
    process_collisions();

    assert(g_call_count == 1);
    assert(g_last_o1_type == T_COIN);
    assert(g_last_o2_type == T_PLAYER);

    /* Удаление реакции через NULL */
    reset_counters();
    set_reaction(T_PLAYER, T_COIN, NULL);
    assert(get_reaction(T_PLAYER, T_COIN) == NULL);
    process_collisions();
    assert(g_call_count == 0);

    clear_objects();
    clear_reactions();
    printf("[OK] test_symmetry_and_argument_order\n");
}

static void test_same_type_and_negative_ids(void)
{
    clear_objects();
    clear_reactions();
    reset_counters();

    object_t *e1 = add_object(T_ENEMY, NULL, false, false);
    object_t *e2 = add_object(T_ENEMY, NULL, false, false);
    object_set_pos(e1, make_vec2(0, 0));
    object_set_hitbox_rect(e1, 0, 0, 32, 32);
    object_set_pos(e2, make_vec2(10, 10));
    object_set_hitbox_rect(e2, 0, 0, 32, 32);

    set_reaction(T_ENEMY, T_ENEMY, react_record);
    process_collisions();

    /* Для однотипной пары реакция обязана вызваться ровно 1 раз */
    assert(g_call_count == 1);
    assert(g_last_o1_type == T_ENEMY);
    assert(g_last_o2_type == T_ENEMY);

    clear_objects();
    clear_reactions();
    printf("[OK] test_same_type_and_negative_ids\n");
}

static void test_solid_physics_and_sprite_sync(void)
{
    clear_objects();
    clear_reactions();
    reset_counters();

    image_t img = { NULL, 32, 32 };
    sprite_t *p_spr = add_sprite(&img, 10, 0, 1);
    sprite_t *w_spr = add_sprite(&img, 30, 0, 1); /* перекрытие по X на 12 px */

    object_t *player = add_object(T_PLAYER, p_spr, true, false);
    object_t *wall = add_object(T_WALL, w_spr, true, true);
    (void)wall;

    object_set_speed(player, make_vec2(5, 0));
    set_reaction(T_PLAYER, T_WALL, react_record);

    process_collisions();

    /* Игрок должен быть вытолкнут влево в x = -2, скорость по X обнулена */
    assert(vec2_x(object_get_pos(player)) == -2);
    assert(vec2_x(graphic_object_get_pos(p_spr->gobj)) == -2);
    assert(vec2_x(object_get_speed(player)) == 0);
    assert(!objects_collide(player, wall));
    assert(g_call_count == 1);

    clear_objects();
    clear_sprites();
    clear_reactions();
    printf("[OK] test_solid_physics_and_sprite_sync\n");
}

static void test_deferred_destroy_and_spawn(void)
{
    clear_objects();
    clear_reactions();
    reset_counters();

    object_t *coin = add_object(T_COIN, NULL, false, true);
    object_t *player = add_object(T_PLAYER, NULL, false, false);
    object_set_pos(coin, make_vec2(0, 0));
    object_set_hitbox_rect(coin, 0, 0, 30, 30);
    object_set_pos(player, make_vec2(5, 5));
    object_set_hitbox_rect(player, 0, 0, 30, 30);

    set_reaction(T_PLAYER, T_COIN, react_collect_coin);
    process_collisions();

    assert(g_call_count == 1);
    assert(g_score == 100);
    /* Монета должна быть удалена из списка, остался только игрок */
    assert(get_objects_head() == player);
    assert(player->next == NULL);

    /* Проверка спавна внутри коллбэка */
    reset_counters();
    object_t *coin2 = add_object(T_COIN, NULL, false, true);
    object_set_pos(coin2, make_vec2(0, 0));
    object_set_hitbox_rect(coin2, 0, 0, 30, 30);
    set_reaction(T_PLAYER, T_COIN, react_spawn_on_hit);
    process_collisions();
    assert(g_call_count == 1);

    clear_objects();
    clear_reactions();
    printf("[OK] test_deferred_destroy_and_spawn\n");
}

static void test_composite_figure_hitbox(void)
{
    clear_objects();
    clear_reactions();
    reset_counters();

    object_t *o1 = add_object(T_PLAYER, NULL, false, false);
    object_t *o2 = add_object(T_WALL, NULL, false, true);

    /* L-образный составной хитбокс из двух прямоугольников */
    figure_t f1;
    f1.count = 2;
    f1.parts[0] = (rect_t){ 0, 0, 10, 40 };
    f1.parts[1] = (rect_t){ 0, 30, 40, 10 };
    object_set_hitbox(o1, f1);
    object_set_pos(o1, make_vec2(0, 0));

    /* В пустом углу L-образного хитбокса пересечения быть не должно */
    object_set_hitbox_rect(o2, 0, 0, 10, 10);
    object_set_pos(o2, make_vec2(20, 5));
    assert(!objects_collide(o1, o2));

    /* В нижней полке L-образного хитбокса пересечение есть */
    object_set_pos(o2, make_vec2(20, 32));
    assert(objects_collide(o1, o2));

    clear_objects();
    clear_reactions();
    printf("[OK] test_composite_figure_hitbox\n");
}

int main(void)
{
    test_symmetry_and_argument_order();
    test_same_type_and_negative_ids();
    test_solid_physics_and_sprite_sync();
    test_deferred_destroy_and_spawn();
    test_composite_figure_hitbox();
    printf("Все тесты пройдены успешно!\n");
    return 0;
}
