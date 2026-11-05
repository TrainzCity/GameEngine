#include "backend.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Проверяет наличие тестового спрайта. При отсутствие - отрисовывает с сохранением в bmp файл
 *
 * @param filename Путь к файлу со спрайтом
 */
static void ensure_sample_asset(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return;
    }

    SDL_Surface *surface = SDL_CreateRGBSurface(0, 128, 128, 32, 0, 0, 0, 0);
    if (surface) {
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 50, 180, 100));
        SDL_SaveBMP(surface, filename);
        SDL_FreeSurface(surface);
    }
}

void run_demo(void) {
    const int window_height = 600;
    const int window_width = 800;
    const char *asset_path = "sample.bmp";

    ensure_sample_asset(asset_path);

    graphics_init(window_height, window_width);

    Image *sprite = image_load(asset_path);
    if (!sprite) {
        fprintf(stderr, "Не удалось загрузить изображение: %s\n", asset_path);
        graphics_close();
        return;
    }

    bool running = true;
    SDL_Event event;
    int pos_x = (window_width - sprite->width) / 2;
    int pos_y = (window_height - sprite->height) / 2;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        graphics_clear();
        image_draw(sprite, pos_x, pos_y);
        graphics_present();
    }

    image_free(sprite);
    graphics_close();
}
