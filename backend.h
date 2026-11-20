#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>

/**
 * @file backend.h
 * @brief Модуль низкоуровневой работы с 2D-графикой на базе библиотеки SDL2.
 */

typedef struct SDL_Texture SDL_Texture;

/**
 * @struct Image
 * @brief Абстрактный тип данных, представляющий загруженное графическое изображение (спрайт).
 */
typedef struct image_t {
    SDL_Texture *texture; /**< Указатель на аппаратную текстуру графического ускорителя */
    int width;            /**< Ширина изображения в пикселях */
    int height;           /**< Высота изображения в пикселях */
} image_t;

void graphics_init(int height, int width);

void graphics_close(void);

void ensure_sample_asset(const char *filename);

image_t *image_load(const char *path);

void image_draw(const image_t *image, int x, int y);

void image_free(image_t *image);

void graphics_clear(void);

void graphics_update(void);

int key_pressed(char key);

bool is_quit(void);

#endif
