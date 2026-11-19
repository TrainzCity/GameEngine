#include "backend.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *g_window = NULL;
static SDL_Renderer *g_renderer = NULL;
static bool g_is_running = false;

/**
 * @brief Инициализирует графическую подсистему, создавая окно и аппаратный рендерер.
 *
 * @param height Высота создаваемого графического окна в пикселях.
 * @param width Ширина создаваемого графического окна в пикселях.
 */
void graphics_init(int height, int width) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;
    }

    g_window = SDL_CreateWindow(
        "Graphics Display",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!g_window) {
        SDL_Quit();
        return;
    }

    g_renderer = SDL_CreateRenderer(
        g_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!g_renderer) {
        SDL_DestroyWindow(g_window);
        g_window = NULL;
        SDL_Quit();
        return;
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    g_is_running = true;
}

/**
 * @brief Завершает работу графической подсистемы и освобождает ресурсы окна и рендерера.
 */
void graphics_close(void) {
    g_is_running = false;
    IMG_Quit();

    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = NULL;
    }

    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = NULL;
    }

    SDL_Quit();
}

/**
 * @brief Проверяет наличие тестового спрайта. При отсутствии генерирует и сохраняет его в BMP-файл.
 *
 * @param filename Путь к файлу со спрайтом.
 */
void ensure_sample_asset(const char *filename) {
    if (!filename) {
        return;
    }

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

/**
 * @brief Загружает растровое изображение по указанному файловому пути.
 *
 * Если файл по указанному пути отсутствует, автоматически создаёт тестовый ассет.
 *
 * @param path Путь к файлу изображения на диске.
 * @return Указатель на структуру Image в случае успеха, либо NULL при ошибке.
 */
image_t *image_load(const char *path) {
    if (!g_renderer || !path) {
        return NULL;
    }

    FILE *file = fopen(path, "rb");
    if (!file) {
        ensure_sample_asset(path);
    } else {
        fclose(file);
    }

    SDL_Texture *texture = IMG_LoadTexture(g_renderer, path);
    if (!texture) {
        return NULL;
    }

    image_t *image = (image_t *)malloc(sizeof(image_t));
    if (!image) {
        SDL_DestroyTexture(texture);
        return NULL;
    }

    image->texture = texture;
    SDL_QueryTexture(texture, NULL, NULL, &image->width, &image->height);

    return image;
}

/**
 * @brief Отрисовывает переданное изображение в буфер кадра по заданным экранным координатам.
 *
 * @param image Указатель на предварительно загруженную структуру Image.
 * @param x Экранная координата X верхнего левого угла отрисовки.
 * @param y Экранная координата Y верхнего левого угла отрисовки.
 */
void image_draw(const image_t *image, int x, int y) {
    if (!g_renderer || !image || !image->texture) {
        return;
    }

    SDL_Rect dst_rect;
    dst_rect.x = x;
    dst_rect.y = y;
    dst_rect.w = image->width;
    dst_rect.h = image->height;

    SDL_RenderCopy(g_renderer, image->texture, NULL, &dst_rect);
}

/**
 * @brief Освобождает память, выделенную под структуру Image, и уничтожает связанную с ней текстуру.
 *
 * @param image Указатель на освобождаемое изображение.
 */
void image_free(image_t *image) {
    if (!image) {
        return;
    }

    if (image->texture) {
        SDL_DestroyTexture(image->texture);
    }

    free(image);
}

/**
 * @brief Очищает текущий кадровый буфер базовым цветом.
 */
void graphics_clear(void) {
    if (g_renderer) {
        SDL_SetRenderDrawColor(g_renderer, 24, 24, 28, 255);
        SDL_RenderClear(g_renderer);
    }
}

/**
 * @brief Выводит сформированный кадровый буфер на экран и обрабатывает события окна.
 */
void graphics_update(void) {
    if (g_renderer) {
        SDL_RenderPresent(g_renderer);
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT ||
           (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            g_is_running = false;
        }
    }
}

/**
 * @brief Возвращает статус активности главного цикла приложения.
 *
 * @return true, если программа продолжает работу; false, если запрошен выход.
 */
bool is_quit(void) {
    return g_is_running;
}
