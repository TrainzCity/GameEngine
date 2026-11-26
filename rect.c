#include "rect.h"
#include <stdbool.h>

/**
 * @brief Проверяет прямоугольник
 *
 * @param t Прямоугольник
 * @return Результат выполнения
 */
bool rect_valid(rect_t t)
{
    if (t.w <= 0 || t.h <= 0)
        return false;

    return true;
}

/**
 * @brief Проверяет пересечения прямоугольников
 *
 * @param t1 Первый прямоугольник
 * @param t2 Второй прямоугольник
 * @return Результат выполнения
 */
bool rect_intorset(rect_t t1, rect_t t2)
{
    if (!rect_valid(t1) || !rect_valid(t2))
        return false;

    return (bool)(t1.x < t2.x + t2.w &&
            t2.x < t1.x + t1.w &&
            t1.y < t2.y + t2.h &&
            t2.y < t1.y + t1.h);
}

/**
 * @brief Проверяет фигуру
 *
 * @param f Фигура
 * @return Результат выполнения
 */
bool figure_valid(figure_t f)
{
    if (f.count < 0 || f.count > MAX_PARTS)
        return false;

    for (int i = 0; i < f.count; i++) {
        if (!rect_valid(f.parts[i]))
            return false;
    }

    return true;
}

/**
 * @brief Проверяет пересечение фигур
 *
 * @param f1 Первая фигура
 * @param f2 Вторая фигура
 * @return Результат выполнения
 */
bool figures_intersect(figure_t f1, figure_t f2)
{
    if (!figure_valid(f1) || !figure_valid(f2))
        return false;

    for (int i = 0; i < f1.count; i++) {
        for (int j = 0; j < f2.count; j++) {
            if (rect_intorset(f1.parts[i], f2.parts[j]))
                return true;
        }
    }

    return false;
}
