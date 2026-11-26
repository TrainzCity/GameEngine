#include <stdio.h>
#include <stdlib.h>
#include "backend.h"
#include "graphic.h"
#include "animation.h"
#include "automata.h"
void apply_fsm_state(automata_t *fsm,
                            int *current_state,
                            int *prev_state,
                            sprite_t *spr,
                            int next_state,
                            animation_t *anim)
{
    if (!fsm || !current_state || !prev_state || !spr) {
        return;
    }

    char *state_name = get_state(fsm, next_state);
    if (!state_name) {
        return;
    }

    *current_state = next_state;
    set_animation(&spr->gobj, anim);

    animation_t *active_anim = get_animation(spr->gobj);
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

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    const int window_height = 600;
    const int window_width = 800;
    const char *asset_path = "sample.bmp";

    /* 1. Создать окно */
    graphics_init(window_height, window_width);

    /* 2. Загрузить изображение */
    image_t *sprite = image_load(asset_path);
    if (!sprite) {
        fprintf(stderr, "Не удалось загрузить изображение: %s\n", asset_path);
        graphics_close();
        return 1;
    }

    /* Создание спрайта */
    sprite_t *p1 = add_sprite(sprite, 100, 100, 1);
    if (!p1) {
        fprintf(stderr, "Не удалось создать спрайт\n");
        image_free(sprite);
        graphics_close();
        return 1;
    }

    /* Инициализация автомата состояний */
    automata_t *fsm = make_automata();
    if (!fsm) {
        fprintf(stderr, "Не удалось создать автомат состояний\n");
        clear_sprites();
        image_free(sprite);
        graphics_close();
        return 1;
    }

    add_state(fsm, OBJ_STAND, "STAND");
    add_state(fsm, OBJ_JUMP,  "JUMP");
    add_state(fsm, OBJ_KICK,  "KICK");

    /* Проверка set_state / get_state */
    set_state(fsm, OBJ_STAND, "STAND");

    /* Создание анимаций (4 аргумента: frames, num_frames, loop, speed) */
    char *stand_frames[] = { "stand_0.bmp", "stand_1.bmp" };
    char *jump_frames[]  = { "jump_0.bmp", "jump_1.bmp" };
    char *kick_frames[]  = { "kick_0.bmp", "kick_1.bmp" };

    animation_t *anim_stand = make_animation(stand_frames, 2, 1, 5);
    animation_t *anim_jump  = make_animation(jump_frames,  2, 0, 3);
    animation_t *anim_kick  = make_animation(kick_frames,  2, 0, 2);

    int current_state = OBJ_STAND;
    int prev_state = -1;
    apply_fsm_state(fsm, &current_state, &prev_state, p1, OBJ_STAND, anim_stand);

    /* 3. Главный цикл: отрисовка изображения, логика автомата и анимаций */
    while (is_quit()) {
        int moved = 0;

        graphics_clear();

        if (key_pressed('a')) {
            move_sprite(p1, -5, 0, 0);
            moved = 1;
        }
        if (key_pressed('d')) {
            move_sprite(p1, 5, 0, 0);
            moved = 1;
        }
        if (key_pressed('w')) {
            move_sprite(p1, 0, -5, 0);
            moved = 1;
        }
        if (key_pressed('s')) {
            move_sprite(p1, 0, 5, 0);
            moved = 1;
        }

        if (key_pressed(' ')) {
            move_sprite(p1, 0, -10, 0);
            apply_fsm_state(fsm, &current_state, &prev_state, p1, OBJ_JUMP, anim_jump);
        } else if (key_pressed('k')) {
            apply_fsm_state(fsm, &current_state, &prev_state, p1, OBJ_KICK, anim_kick);
        } else {
            if (!moved) {
                apply_fsm_state(fsm, &current_state, &prev_state, p1, OBJ_STAND, anim_stand);
            }
            animation_t *cur_anim = get_animation(p1->gobj);
            if (cur_anim) {
                animation_update(cur_anim);
            }
        }

        print_sprites();
        graphics_update();
    }

    /* 4. Освободить ресурсы и закрыть окно
*/
    animation_destroy(anim_stand);
    animation_destroy(anim_jump);
    animation_destroy(anim_kick);
    free(fsm);
    clear_sprites();
    image_free(sprite);
    graphics_close();

    return 0;
}
