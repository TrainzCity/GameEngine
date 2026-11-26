#ifndef ANIMATION_H
#define ANIMATION_H


/**
 * Структура контейнера изображений
 *
 * @param frames массив указателей на строки (кадры)
 * @param num_frames общее число кадров
 * @param current_frame индекс текущего кадра 
 *
 * @return
 */
typedef struct
{
  char** frames;
  int num_frames;
  char** current_frame;
  int looping;
  int speed;
  int tick;
} animation_t;

/**
 * Функция создания анимации
 *
 * @param frames массив строк излбражений
 * @param num_frames число кадров
 *
 * @return указатель на создавнный объект анимации
 */
animation_t* make_animation(char** frames, int num_frame, int loop, int speed);

/**
 * Функция обновления анимации
 *
 * @param anim указатель на обЪект анимации
 */
void animation_update(animation_t* anim);

/**
 * Функция получения текущего изображения 
 *
 * @param anim указатель на объект анимации
 */
char* animation_get(animation_t* anim);

/**
 * Функция для особождения выделенной памяти, чтобы не было утечек
 *
 * @param anim указатель на объект анимации
 */
void animation_destroy(animation_t* anim);

int assert_str (const char* a, const char* b);


#endif
