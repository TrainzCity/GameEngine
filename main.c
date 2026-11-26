#include <stdio.h>
#include <stdlib.h>
#include "backend.h"
#include "graphic.h"
#include "animation.h"
#include "automata.h"
#include "object.h"

/* Пользовательские типы объектов для системы реакций */
enum {
    TYPE_PLAYER = 1,
    TYPE_WALL   = 2,
    TYPE_COIN   = 3,
    TYPE_ENEMY  = 4
};

/* Пользовательский контекст игрока, передаваемый через object_t::user_data */
typedef struct {
    int score;
    int hp;
    int hit_cooldown;
} player_data_t;

void apply_fsm_state(automata_t *fsm,
                     int *current_state,
                     int *prev_state,
                     object_t *obj,
                     int next_state,
                     animation_t *anim)
{
    if (!fsm || !current_state || !prev_state || !obj || !obj->sprite) {
        return;
    }

    char *state_name = get_state(fsm, next_state);
    if (!state_name) {
        return;
    }

    *current_state = next_state;
    obj->state = next_state;
    set_animation(&obj->sprite->gobj, anim);

    animation_t *active_anim = get_animation(obj->sprite->gobj);
    if (active_anim) {
        animation_update(active_anim);
    }

    if (*current_state != *prev_state) {
        char *frame_info = animation_get(active_anim);
        if (frame_info == NULL) {
            frame_info = "нет";
        }
        printf("Состояние: %s | кадр: %s\n", state_name, frame_info);
        *prev_state = *current_state;
    }
}

/* Реакция: игрок подбирает монету (безопасное отложенное удаление внутри коллбэка) */
static void on_player_coin(object_t *player, object_t *coin)
{
    player_data_t *pdata = (player_data_t *)player->user_data;
    if (pdata) {
        pdata->score += 10;
        printf("[Реакция] Игрок подобрал монету! Счёт: %d\n", pdata->score);
    }
    remove_object(coin);
}

/* Реакция: столкновение игрока с врагом (зависит от состояния автомата) */
static void on_player_enemy(object_t *player, object_t *enemy)
{
    player_data_t *pdata = (player_data_t *)player->user_data;
    if (!pdata) {
        return;
    }

    if (player->state == OBJ_KICK) {
        pdata->score += 50;
        printf("[Реакция] Игрок победил врага ударом (KICK)! Счёт: %d\n", pdata->score);
        remove_object(enemy);
    } else if (pdata->hit_cooldown == 0) {
        pdata->hp -= 1;
        pdata->hit_cooldown = 30;
        move_object(player, -20, 0);
        printf("[Реакция] Враг задел игрока! Осталось HP: %d\n", pdata->hp);
    }
}

/* Реакция: столкновение игрока со стеной (физическое выталкивание уже выполнено библиотекой) */
static void on_player_wall(object_t *player, object_t *wall)
{
    (void)wall;
    vec2_t p = object_get_pos(player);
    printf("[Реакция] Касание стены, позиция игрока: (%d, %d)\n", vec2_x(p), vec2_y(p));
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    const int window_height = 600;
    const int window_width = 800;
    const char *asset_path = "sample.bmp";

    /* 1. Создать окно */
    graphics_init(window_height, window_width);

    /* 2. Загрузить изображение */
    image_t *sprite_img = image_load(asset_path);
    if (!sprite_img) {
        fprintf(stderr, "Не удалось загрузить изображение: %s\n", asset_path);
        graphics_close();
        return 1;
    }

    /* Создаём объекты (намеренно добавляем монету раньше игрока для проверки симметрии set_reaction) */
    sprite_t *coin_spr   = add_sprite(sprite_img, 240, 420, 1);
    sprite_t *wall_spr   = add_sprite(sprite_img, 360, 180, 1);
    sprite_t *enemy_spr  = add_sprite(sprite_img, 580, 220, 1);
    sprite_t *player_spr = add_sprite(sprite_img, 80,  220, 2);

    if (!coin_spr || !wall_spr || !enemy_spr || !player_spr) {
        fprintf(stderr, "Не удалось создать спрайты\n");
        clear_sprites();
        image_free(sprite_img);
        graphics_close();
        return 1;
    }

    object_t *coin   = add_object(TYPE_COIN,   coin_spr,   false, true);  /* триггер */
    object_t *wall   = add_object(TYPE_WALL,   wall_spr,   true,  true);  /* твёрдая стена */
    object_t *enemy  = add_object(TYPE_ENEMY,  enemy_spr,  true,  false); /* подвижный враг */
    object_t *player = add_object(TYPE_PLAYER, player_spr, true,  false); /* игрок */

    if (!coin || !wall || !enemy || !player) {
        fprintf(stderr, "Не удалось создать игровые объекты\n");
        clear_objects();
        clear_sprites();
        image_free(sprite_img);
        graphics_close();
        return 1;
    }

    player_data_t pdata = { .score = 0, .hp = 3, .hit_cooldown = 0 };
    player->user_data = &pdata;

    /* Регистрация реакций на взаимодействие объектов через API из tz.rst */
    set_reaction(TYPE_PLAYER, TYPE_COIN,  on_player_coin);
    set_reaction(TYPE_PLAYER, TYPE_ENEMY, on_player_enemy);
    set_reaction(TYPE_PLAYER, TYPE_WALL,  on_player_wall);

    /* Инициализация автомата состояний */
    automata_t *fsm = make_automata();
    if (!fsm) {
        fprintf(stderr, "Не удалось создать автомат состояний\n");
        clear_reactions();
        clear_objects();
        image_free(sprite_img);
        graphics_close();
        return 1;
    }

    add_state(fsm, OBJ_STAND, "STAND");
    add_state(fsm, OBJ_JUMP,  "JUMP");
    add_state(fsm, OBJ_KICK,  "KICK");
    set_state(fsm, OBJ_STAND, "STAND");
    object_set_fsm(player, fsm, OBJ_STAND);

    /* Создание анимаций (4 аргумента: frames, num_frames, loop, speed) */
    char *stand_frames[] = { "stand_0.bmp", "stand_1.bmp" };
    char *jump_frames[]  = { "jump_0.bmp",  "jump_1.bmp"  };
    char *kick_frames[]  = { "kick_0.bmp",  "kick_1.bmp"  };

    animation_t *anim_stand = make_animation(stand_frames, 2, 1, 5);
    animation_t *anim_jump  = make_animation(jump_frames,  2, 0, 3);
    animation_t *anim_kick  = make_animation(kick_frames,  2, 0, 2);

    int current_state = OBJ_STAND;
    int prev_state = -1;
    apply_fsm_state(fsm, &current_state, &prev_state, player, OBJ_STAND, anim_stand);

    /* 3. Главный цикл: управление, физика/столкновения, анимации и отрисовка */
    while (is_quit()) {
        int vx = 0;
        int vy = 0;
        int moved = 0;

        graphics_clear();

        if (pdata.hit_cooldown > 0) {
            pdata.hit_cooldown--;
        }

        if (key_pressed('a')) { vx -= 5; moved = 1; }
        if (key_pressed('d')) { vx += 5; moved = 1; }
        if (key_pressed('w')) { vy -= 5; moved = 1; }
        if (key_pressed('s')) { vy += 5; moved = 1; }

        if (key_pressed(' ')) {
            vy -= 10;
            apply_fsm_state(fsm, &current_state, &prev_state, player, OBJ_JUMP, anim_jump);
        } else if (key_pressed('k')) {
            apply_fsm_state(fsm, &current_state, &prev_state, player, OBJ_KICK, anim_kick);
        } else {
            if (!moved) {
                apply_fsm_state(fsm, &current_state, &prev_state, player, OBJ_STAND, anim_stand);
            }
            animation_t *cur_anim = get_animation(player->sprite->gobj);
            if (cur_anim) {
                animation_update(cur_anim);
            }
        }

        object_set_speed(player, make_vec2(vx, vy));

        /* Шаг симуляции: движение по скорости + автоматическая обработка столкновений и реакций */
        update_objects();

        print_sprites();
        graphics_update();
    }

    /* 4. Освободить ресурсы и закрыть окно */
    animation_destroy(anim_stand);
    animation_destroy(anim_jump);
    animation_destroy(anim_kick);
    free(fsm);
    clear_reactions();
    clear_objects();
    clear_sprites();
    image_free(sprite_img);
    graphics_close();

    return 0;
}
