#include <stdio.h>
#include <stdlib.h>
#include "automata.h"

/**
 * Создаёт и инициализирует новый экземпляр автомата в динамической памяти.
 *
 * @return automata_t * Указатель на новый пустой автомат или NULL при ошибке выделения памяти.
 */
automata_t *make_automata()
{
    automata_t *a = malloc(sizeof(automata_t));
    if (a == NULL)
        return NULL;

    for (int i = 0; i < MAX_STATE; i++)
        a->states[i][0] = '\0';

    a->last = a->states;
    return a;
}

/**
 * Записывает имя состояния по заданному идентификатору и при необходимости сдвигает границу last.
 *
 * @param a Указатель на структуру автомата.
 * @param id Идентификатор состояния.
 * @param name Строка с названием состояния.
 */
void add_state(automata_t *a, int id, char *name)
{
    if (a == NULL || name == NULL || id < 0 || id >= MAX_STATE)
        return;

    int i = 0;
    while (i < MAX_NAME_SIZE - 1 && name[i] != '\0')
    {
        a->states[id][i] = name[i];
        i++;
    }
    a->states[id][i] = '\0';

    state_t *added = &a->states[id];
    if (a->last != NULL && added >= a->last)
    {
        if (id + 1 < MAX_STATE)
            a->last = &a->states[id + 1];
        else
            a->last = NULL;
    }
}

/**
 * Возвращает название состояния по его идентификатору.
 *
 * @param a Указатель на структуру автомата.
 * @param id Идентификатор состояния.
 * @return char * Указатель на строку с именем состояния или NULL, если состояние отсутствует.
 */
char *get_state(automata_t *a, int id)
{
    if (a == NULL || id < 0 || id >= MAX_STATE)
        return NULL;

    if (a->states[id][0] == '\0')
        return NULL;

    return a->states[id];
}

/**
 * Заменяет название уже существующего состояния с указанным идентификатором.
 *
 * @param a Указатель на структуру автомата.
 * @param id Идентификатор существующего состояния.
 * @param name Новая строка с названием состояния.
 */
void set_state(automata_t *a, int id, char *name)
{
    if (a == NULL || name == NULL || id < 0 || id >= MAX_STATE)
        return;

    /* Заменяем только уже добавленное состояние */
    if (a->states[id][0] == '\0')
        return;

    int i = 0;
    while (i < MAX_NAME_SIZE - 1 && name[i] != '\0')
    {
        a->states[id][i] = name[i];
        i++;
    }
    a->states[id][i] = '\0';
}
