#ifndef REACTION_H
#define REACTION_H

#include <stdbool.h>

/**
 * @file reaction.h
 * @brief Модуль регистрации и диспетчеризации реакций на столкновения объектов.
 */

struct object;
typedef struct object object_t;

/**
 * @brief Тип функции-обработчика взаимодействия двух объектов.
 */
typedef void (*reaction_func_t)(object_t *o1, object_t *o2);

/**
 * @brief Регистрирует реакцию на взаимодействие объектов с типами obj_type1 и obj_type2.
 *
 * Порядок аргументов при вызове react_func всегда соответствует порядку типов,
 * указанному при регистрации (o1 имеет тип obj_type1, o2 имеет тип obj_type2).
 * Повторный вызов для той же пары типов (в любом порядке) перезаписывает реакцию.
 * Передача react_func == NULL удаляет зарегистрированную реакцию.
 *
 * @param obj_type1 Тип первого объекта.
 * @param obj_type2 Тип второго объекта.
 * @param react_func Функция-обработчик столкновения или NULL для удаления.
 */
void set_reaction(int obj_type1, int obj_type2, void (*react_func)(object_t *o1, object_t *o2));

/**
 * @brief Возвращает зарегистрированный обработчик для пары типов (или NULL).
 *
 * @param obj_type1 Тип первого объекта.
 * @param obj_type2 Тип второго объекта.
 * @return Указатель на функцию-обработчик либо NULL, если реакция не задана.
 */
reaction_func_t get_reaction(int obj_type1, int obj_type2);

/**
 * @brief Вызывает зарегистрированную реакцию для пары столкнувшихся объектов.
 *
 * Автоматически переставляет аргументы, если порядок типов o1 и o2
 * противоположен порядку при регистрации в set_reaction().
 *
 * @param o1 Первый столкнувшийся объект.
 * @param o2 Второй столкнувшийся объект.
 * @return true, если реакция была найдена и вызвана; false в противном случае.
 */
bool dispatch_reaction(object_t *o1, object_t *o2);

/**
 * @brief Очищает таблицу реакций и освобождает занятую память.
 */
void clear_reactions(void);

#endif
