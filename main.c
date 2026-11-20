#include "backend.h"
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

    int pos_x = (window_width - sprite->width) / 2;
    int pos_y = (window_height - sprite->height) / 2;

    /* 3. Главный цикл: отрисовка изображения и ожидание команд пользователя */
    while (is_quit()) {
        graphics_clear();
        image_draw(sprite, pos_x, pos_y);
	if (key_pressed('a'))
	    pos_x--;
	if (key_pressed('d'))
	    pos_x++;
	if (key_pressed('w'))
	    pos_y--;
	if (key_pressed('s'))
	    pos_y++;
              
        graphics_update();
    }

    /* 4. Освободить ресурсы и закрыть окно */
    image_free(sprite);
    graphics_close();

    return 0;
}

