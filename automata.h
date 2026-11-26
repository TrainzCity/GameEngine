/**
 * Максимальная длина строки с именем состояния (включая завершающий символ '\0').
 */
#define MAX_NAME_SIZE 64

/**
 * Максимальное количество состояний, которое может хранить автомат.
 */
#define MAX_STATE 100

/**
 * Идентификаторы состояний объекта.
 *
 * @param OBJ_STAND Состояние покоя/стойки.
 * @param OBJ_JUMP Состояние прыжка.
 * @param OBJ_KICK Состояние удара.
 */
enum
  {
    OBJ_STAND,
    OBJ_JUMP,
    OBJ_KICK,
  };
  
/**
 * Тип строки для хранения названия одного состояния.
 */
typedef char state_t[MAX_NAME_SIZE];

/**
 * Структура автомата, хранящая массив состояний и указатель на границу заполненных данных.
 *
 * @param states Массив строк с именами состояний.
 * @param last Указатель на ячейку сразу после последнего добавленного состояния.
 */
typedef struct automata
{
  state_t states[MAX_STATE];
  state_t *last;
} automata_t;


automata_t *make_automata();

void add_state(automata_t *a, int id, char *name);

char *get_state(automata_t *a, int id);

void set_state(automata_t *a, int id, char *name);
