#include <stdio.h>
#include "rect.h"

#define ASSERT(c)\
  if (c)\
    printf("OK\n");\
  else\
    printf("Fail\n");

void rect_intersect_test()
{
    rect_t a = {0, 0, 3, 3};
    rect_t b = {2, 2, 2, 2};
    rect_t c = {5, 5, 2, 2};

    ASSERT(rect_intorset(a, b) == true);
    ASSERT(rect_intorset(a, c) == false);
}

void figure_quarters_test()
{
    figure_t a = {{{2, 2, 4, 4}}, 1};
    figure_t b = {{{4, 4, 3, 3}}, 1};
    ASSERT(figures_intersect(a, b) == true);

    figure_t c = {{{-5, 2, 4, 4}}, 1};
    figure_t d = {{{-3, 4, 3, 3}}, 1};
    ASSERT(figures_intersect(c, d) == true);

    figure_t e = {{{-5, -5, 4, 4}}, 1};
    figure_t f = {{{-3, -3, 3, 3}}, 1};
    ASSERT(figures_intersect(e, f) == true);

    figure_t g = {{{2, -5, 4, 4}}, 1};
    figure_t h = {{{4, -3, 3, 3}}, 1};
    ASSERT(figures_intersect(g, h) == true);
}

void figure_edges_test()
{
    figure_t a = {{{0, 0, 3, 3}}, 1};
    figure_t b = {{{2, 2, 2, 2}}, 1};

    ASSERT(figures_intersect(a, b) == true);

    figure_t c = {{{0, 0, 3, 3}}, 1};
    figure_t d = {{{3, 0, 3, 3}}, 1};

    ASSERT(figures_intersect(c, d) == false);

    figure_t e = {{{0, 0, 3, 3}}, 1};
    figure_t f = {{{4, 0, 3, 3}}, 1};

    ASSERT(figures_intersect(e, f) == false);
}

void figure_intersect_test()
{
    figure_t a = {
        {{0, 0, 2, 2}, {0, 3, 2, 2}},
        2
    };

    figure_t b = {
        {{5, 0, 2, 2}, {5, 3, 2, 2}},
        2
    };

    ASSERT(figures_intersect(a, b) == false);

    figure_t c = {
        {{0, 0, 2, 2}, {10, 10, 3, 3}},
        2
    };

    figure_t d = {
        {{5, 5, 2, 2}, {12, 12, 3, 3}},
        2
    };

    ASSERT(figures_intersect(c, d) == true);
}

void figure_inside_test()
{
    figure_t a = {{{0, 0, 10, 10}}, 1};
    figure_t b = {{{2, 2, 3, 3}}, 1};

    ASSERT(figures_intersect(a, b) == true);

    figure_t c = {{{1, 1, 5, 5}}, 1};
    figure_t d = {{{1, 1, 5, 5}}, 1};

    ASSERT(figures_intersect(c, d) == true);
}

int main()
{
  printf("Проверка пересечения прямоугольников:\n");
  rect_intersect_test();
  printf("Проверка всех четырех четвертей:\n");
  figure_quarters_test();
  printf("Проверка касаний и пересечения одним пикселем:\n");
  figure_edges_test();
  printf("Проверка сложных фигур:\n");
  figure_intersect_test();
  printf("Проверка вложенных и совпадающих фигур:\n");
  figure_inside_test();

  return 0;
}
