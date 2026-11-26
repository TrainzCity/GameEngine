#include "reaction.h"
#include "object.h"
#include <stdlib.h>
#include <stdint.h>

#define INITIAL_TABLE_CAPACITY 32

typedef enum {
    SLOT_EMPTY = 0,
    SLOT_OCCUPIED = 1,
    SLOT_DELETED = 2
} slot_state_t;

typedef struct {
    slot_state_t state;
    int key_min;          /**< Нормализованный ключ: min(type1, type2) */
    int key_max;          /**< Нормализованный ключ: max(type1, type2) */
    int reg_type1;        /**< Исходный obj_type1 при регистрации */
    int reg_type2;        /**< Исходный obj_type2 при регистрации */
    reaction_func_t func; /**< Указатель на коллбэк */
} reaction_slot_t;

static reaction_slot_t *g_table = NULL;
static size_t g_capacity = 0;
static size_t g_count = 0;
static size_t g_tombstones = 0;

/**
 * @brief Нормализует пару типов в канонический вид (min, max).
 */
static void normalize_types(int t1, int t2, int *out_min, int *out_max)
{
    if (t1 <= t2) {
        *out_min = t1;
        *out_max = t2;
    } else {
        *out_min = t2;
        *out_max = t1;
    }
}

/**
 * @brief Вычисляет 64-битный хеш (SplitMix64) для нормализованной пары типов.
 */
static size_t hash_pair(int k1, int k2)
{
    uint64_t x = ((uint64_t)(uint32_t)k1 << 32) | (uint64_t)(uint32_t)k2;
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;
    return (size_t)x;
}

/**
 * @brief Перестраивает хеш-таблицу с новой вместимостью (степень двойки).
 */
static bool rehash_table(size_t new_capacity)
{
    reaction_slot_t *new_table = (reaction_slot_t *)calloc(new_capacity, sizeof(reaction_slot_t));
    if (!new_table) {
        return false;
    }

    size_t mask = new_capacity - 1;
    for (size_t i = 0; i < g_capacity; i++) {
        if (g_table[i].state == SLOT_OCCUPIED) {
            size_t idx = hash_pair(g_table[i].key_min, g_table[i].key_max) & mask;
            while (new_table[idx].state == SLOT_OCCUPIED) {
                idx = (idx + 1) & mask;
            }
            new_table[idx] = g_table[i];
        }
    }

    free(g_table);
    g_table = new_table;
    g_capacity = new_capacity;
    g_tombstones = 0;
    return true;
}

/**
 * @brief Ищет слот по нормализованной паре типов.
 */
static reaction_slot_t *find_slot(int k1, int k2)
{
    if (!g_table || g_capacity == 0 || g_count == 0) {
        return NULL;
    }

    size_t mask = g_capacity - 1;
    size_t idx = hash_pair(k1, k2) & mask;

    for (size_t step = 0; step < g_capacity; step++) {
        reaction_slot_t *slot = &g_table[idx];
        if (slot->state == SLOT_EMPTY) {
            return NULL;
        }
        if (slot->state == SLOT_OCCUPIED &&
            slot->key_min == k1 &&
            slot->key_max == k2) {
            return slot;
        }
        idx = (idx + 1) & mask;
    }

    return NULL;
}

void set_reaction(int obj_type1, int obj_type2, void (*react_func)(object_t *o1, object_t *o2))
{
    int k1, k2;
    normalize_types(obj_type1, obj_type2, &k1, &k2);

    /* Передача NULL удаляет существующую реакцию */
    if (react_func == NULL) {
        reaction_slot_t *existing = find_slot(k1, k2);
        if (existing) {
            existing->state = SLOT_DELETED;
            existing->func = NULL;
            g_count--;
            g_tombstones++;
        }
        return;
    }

    if (g_capacity == 0) {
        if (!rehash_table(INITIAL_TABLE_CAPACITY)) {
            return;
        }
    } else if ((g_count + g_tombstones + 1) * 10 >= g_capacity * 7) {
        size_t next_cap = ((g_count + 1) * 10 >= g_capacity * 5) ? (g_capacity * 2) : g_capacity;
        if (!rehash_table(next_cap)) {
            return;
        }
    }

    size_t mask = g_capacity - 1;
    size_t idx = hash_pair(k1, k2) & mask;
    reaction_slot_t *first_deleted = NULL;

    for (size_t step = 0; step < g_capacity; step++) {
        reaction_slot_t *slot = &g_table[idx];

        if (slot->state == SLOT_EMPTY) {
            reaction_slot_t *target = first_deleted ? first_deleted : slot;
            if (target->state == SLOT_DELETED) {
                g_tombstones--;
            }
            target->state = SLOT_OCCUPIED;
            target->key_min = k1;
            target->key_max = k2;
            target->reg_type1 = obj_type1;
            target->reg_type2 = obj_type2;
            target->func = react_func;
            g_count++;
            return;
        }

        if (slot->state == SLOT_OCCUPIED &&
            slot->key_min == k1 &&
            slot->key_max == k2) {
            slot->reg_type1 = obj_type1;
            slot->reg_type2 = obj_type2;
            slot->func = react_func;
            return;
        }

        if (slot->state == SLOT_DELETED && first_deleted == NULL) {
            first_deleted = slot;
        }

        idx = (idx + 1) & mask;
    }
}

reaction_func_t get_reaction(int obj_type1, int obj_type2)
{
    int k1, k2;
    normalize_types(obj_type1, obj_type2, &k1, &k2);

    reaction_slot_t *slot = find_slot(k1, k2);
    return slot ? slot->func : NULL;
}

bool dispatch_reaction(object_t *o1, object_t *o2)
{
    if (!o1 || !o2) {
        return false;
    }

    int k1, k2;
    normalize_types(o1->type, o2->type, &k1, &k2);

    reaction_slot_t *slot = find_slot(k1, k2);
    if (!slot || !slot->func) {
        return false;
    }

    if (o1->type == slot->reg_type1 && o2->type == slot->reg_type2) {
        slot->func(o1, o2);
    } else {
        slot->func(o2, o1);
    }

    return true;
}

void clear_reactions(void)
{
    free(g_table);
    g_table = NULL;
    g_capacity = 0;
    g_count = 0;
    g_tombstones = 0;
}
