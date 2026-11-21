#include "backend.h"
#include "graphic.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    const int window_height = 600;
    const int window_width = 800;
    const char *asset_path = "sample.bmp";

    /* 1. Создать окно */
    graphics_init(window_height, window_width);

    /* 2. Загрузить изображение (если файла нет, он создастся автоматически) */
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

    /* 3. Главный цикл: отрисовка изображения и ожидание команд пользователя */
    while (is_quit()) {
        graphics_clear();

        if (key_pressed('a')) move_sprite(p1, -5, 0, 0);
        if (key_pressed('d')) move_sprite(p1, 5, 0, 0);
        if (key_pressed('w')) move_sprite(p1, 0, -5, 0);
        if (key_pressed('s')) move_sprite(p1, 0, 5, 0);

        print_sprites();
        graphics_update();
    }

    /* 4. Освободить ресурсы и закрыть окно */
    clear_sprites();
    image_free(sprite);
    graphics_close();

    return 0;
}
