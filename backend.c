#include "backend.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>

static SDL_Window *g_window = NULL;
static SDL_Renderer *g_renderer = NULL;

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
}

/**
 * @brief Завершает работу графической подсистемы и освобождает ресурсы окна и рендерера.
 */
void graphics_close(void) {
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
 * @brief Загружает растровое изображение по указанному файловому пути.
 *
 * Поддерживает форматы PNG, JPG и BMP при использовании библиотеки SDL2_image.
 *
 * @param path Путь к файлу изображения на диске.
 * @return Указатель на структуру Image в случае успеха, либо NULL при ошибке загрузки или нехватке памяти.
 */
Image *image_load(const char *path) {
    if (!g_renderer || !path) {
        return NULL;
    }

    SDL_Texture *texture = IMG_LoadTexture(g_renderer, path);
    if (!texture) {
        return NULL;
    }

    Image *image = (Image *)malloc(sizeof(Image));
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
void image_draw(const Image *image, int x, int y) {
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
void image_free(Image *image) {
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
 * @brief Выводит сформированный кадровый буфер на экран (выполняет смену буферов).
 */
void graphics_present(void) {
    if (g_renderer) {
        SDL_RenderPresent(g_renderer);
    }
}
