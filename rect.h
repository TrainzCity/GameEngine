#include <stdbool.h>
#define MAX_PARTS 10
typedef struct {
    int x;
    int y;
    int w;
    int h;
} rect_t;
typedef struct {
  rect_t parts[MAX_PARTS];
    int count;
} figure_t;
bool rect_valid(rect_t t);
bool rect_intorset(rect_t t1, rect_t t2);
bool figure_valid(figure_t f);
bool figures_intersect(figure_t f1,figure_t f2);
