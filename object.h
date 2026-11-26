#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "vec2.h"
#include "rect.h"
#include "graphic.h"
#include "automata.h"
#include "reaction.h"

/**
 * @file object.h
 * @brief Модуль игровых объектов, хитбоксов и автоматической обработки столкновений.
 */

/**
 * @struct object
 * @brief Игровой объект платформера, объединяющий спрайт, физику, хитбокс и автомат состояний.
 */
struct object {
    int type;               /**< Идентификатор типа объекта для системы реакций */
    sprite_t *sprite;       /**< Связанный спрайт (может быть NULL для невидимого триггера) */
    vec2_t pos;             /**< Позиция (синхронизирована со sprite->gobj.pos) */
    vec2_t speed;           /**< Скорость (синхронизирована со sprite->gobj.speed) */
    figure_t hitbox;        /**< Хитбокс в локальных координатах относительно pos */
    automata_t *fsm;        /**< Автомат состояний объекта (необязательно, может быть NULL) */
    int state;              /**< Текущее состояние автомата */
    bool solid;             /**< Участвует ли в физическом выталкивании при столкновении */
    bool is_static;         /**< Неподвижный объект (стена, платформа) */
    bool active;            /**< Активность объекта в симуляции и столкновениях */
    bool pending_destroy;   /**< Флаг отложенного удаления после прохода столкновений */
    void *user_data;        /**< Пользовательские данные, доступные внутри react_func */

    struct object *next;
    struct object *prev;
};

/**
 * @brief Создаёт игровой объект и добавляет его в глобальный список объектов.
 *
 * Если у спрайта задано изображение с корректными размерами, автоматически
 * формируется прямоугольный хитбокс {0, 0, img->width, img->height}.
 *
 * @param type Тип объекта.
 * @param sprite Указатель на связанный спрайт (или NULL для невидимого объекта).
 * @param solid true — твёрдое тело (физическое выталкивание); false — триггер/сенсор.
 * @param is_static true — неподвижный объект; false — подвижный объект.
 * @return Указатель на созданный объект или NULL при ошибке выделения памяти.
 */
object_t *add_object(int type, sprite_t *sprite, bool solid, bool is_static);

/**
 * @brief Удаляет игровой объект (и его связанный спрайт, если он есть).
 *
 * Безопасно вызывать внутри коллбэка react_func: в этом случае удаление
 * откладывается до завершения текущего прохода проверки столкновений.
 *
 * @param obj Удаляемый объект.
 */
void remove_object(object_t *obj);

/**
 * @brief Удаляет все игровые объекты и связанные с ними спрайты.
 */
void clear_objects(void);

/**
 * @brief Возвращает первый объект в списке (для пользовательского обхода при необходимости).
 */
object_t *get_objects_head(void);

/**
 * @brief Устанавливает составной хитбокс объекта (в локальных координатах относительно pos).
 */
void object_set_hitbox(object_t *obj, figure_t hitbox);

/**
 * @brief Устанавливает простой прямоугольный хитбокс объекта (в локальных координатах).
 */
void object_set_hitbox_rect(object_t *obj, int offset_x, int offset_y, int w, int h);

/**
 * @brief Возвращает хитбокс объекта, переведённый в мировые координаты.
 */
figure_t object_get_world_hitbox(const object_t *obj);

/**
 * @brief Устанавливает позицию объекта (синхронно со спрайтом).
 */
void object_set_pos(object_t *obj, vec2_t pos);

/**
 * @brief Возвращает текущую позицию объекта.
 */
vec2_t object_get_pos(const object_t *obj);

/**
 * @brief Смещает объект на (dx, dy) (синхронно со спрайтом).
 */
void move_object(object_t *obj, int dx, int dy);

/**
 * @brief Устанавливает скорость объекта (синхронно со спрайтом).
 */
void object_set_speed(object_t *obj, vec2_t speed);

/**
 * @brief Возвращает текущую скорость объекта.
 */
vec2_t object_get_speed(const object_t *obj);

/**
 * @brief Привязывает автомат состояний и начальное состояние к объекту.
 */
void object_set_fsm(object_t *obj, automata_t *fsm, int initial_state);

/**
 * @brief Проверяет геометрическое пересечение двух активных объектов.
 */
bool objects_collide(const object_t *o1, const object_t *o2);

/**
 * @brief Проверяет все пары объектов на столкновения, разрешает физику для solid-пар
 *        и вызывает зарегистрированные через set_reaction() обработчики.
 */
void process_collisions(void);

/**
 * @brief Выполняет полный шаг обновления мира: двигает подвижные объекты по их скорости
 *        и запускает обработку столкновений (process_collisions).
 */
void update_objects(void);

#endif
