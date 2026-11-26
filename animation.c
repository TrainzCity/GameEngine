#include <stdio.h>
#include <stdlib.h>
#include "animation.h"


/**
 * Функция создания анимации
 *
 * @param frames массив строк излбражений
 * @param num_frames число кадров
 *
 * @return указатель на создавнный объект анимации
 */
animation_t* make_animation(char** frames, int num_frames, int loop, int speed)
{
  if(frames == NULL || num_frames <= 0)
    return NULL;
  if(speed < 0)
    speed = 0;
  animation_t* anim = malloc(sizeof(animation_t));
  if (anim == NULL)
    return NULL;
  anim->frames = frames;
  anim->num_frames = num_frames;
  anim->current_frame = anim->frames;
  anim->looping = loop;
  anim->speed = speed;
  anim->tick = 0;
  return anim;
}

/**
 * Функция обновления анимации
 *
 * @param anim указатель на обЪект анимации
 */
void animation_update(animation_t* anim)
{
  if (anim != NULL && anim->frames != NULL && anim->num_frames > 0){
    if (anim->tick < anim->speed)
      {
	anim->tick++;
	return;
      }
    anim->tick = 0;
    
    char** last_frames = anim->frames + (anim->num_frames - 1);
    if (anim->current_frame < last_frames)
      anim->current_frame++;
    else if (anim -> looping == 1)
      anim->current_frame = anim->frames;
  }
}


/**
 * Функция обновления анимации
 *
 * @param anim указатель на обЪект анимации
 */
char* animation_get(animation_t* anim){
  if (anim != NULL && anim->frames != NULL)
    return *(anim->current_frame);
  return NULL;
}

/**
 * Функция для особождения выделенной памяти, чтобы не было утечек
 *
 * @param anim указатель на объект анимации
 */
void animation_destroy(animation_t* anim){
  if (anim != NULL)
    free(anim);
}

int assert_str(const char* a, const char* b){
  if (a == NULL || b == NULL)
    return 0;
 
  for (int i = 0; ; i++)
    {
      if (a[i] != b[i])
	return 0;
      if (a[i] == '\0')
	return 1;
    }
}

//добавить возможность добавить скорость анимации добавить параметр скорости каждый кадр анимации дублируется при значении 1, при 2 утраивается и так далее 
