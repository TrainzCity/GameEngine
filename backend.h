#ifndef BACKEND_H
#define BACKEND_H

#include <SDL2/SDL.h>

/**
 * @file backend.h
 * @brief Модуль низкоуровневой работы с 2D-графикой на базе библиотеки SDL2.
 */

/**
 * @struct Image
 * @brief Абстрактный тип данных, представляющий загруженное графическое изображение (спрайт).
 *
 * Содержит дескриптор аппаратной текстуры SDL и ее физические габариты в пикселях.
 */
typedef struct Image {
    SDL_Texture *texture; /**< Указатель на аппаратную текстуру графического ускорителя */
    int width;            /**< Ширина изображения в пикселях */
    int height;           /**< Высота изображения в пикселях */
} Image;

void graphics_init(int height, int width);

void graphics_close(void);

Image *image_load(const char *path);

void image_draw(const Image *image, int x, int y);

void image_free(Image *image);

void graphics_clear(void);

void graphics_present(void);

#endif
