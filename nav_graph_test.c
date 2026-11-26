#include "nav_graph.h"
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(c)\
  if (c)\
    printf("OK\n");\
  else\
    printf("Fail\n");

void create_nav_graph_test()
{
  nav_graph_t g = create_nav_graph();
  ASSERT(g.vertexs_count == 0);
}

void add_vertex_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  ASSERT(get_vertex(&g, 0)->x == 1 && get_vertex(&g, 0)->y == 1);
}

void add_connection_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  add_vertex(&g, 2, 2);
  add_connection(&g, 0, 1);
  ASSERT(get_vertex_connection_index(g, 0, 0) == 1 && get_vertex_connection_index(g, 1, 0) == 0);
}

void get_vertex_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  vertex_t *v = get_vertex(&g, 0);
  ASSERT(v->x == g.vertexs[0].x && v->y == g.vertexs[0].y);
}

void get_vertex_connection_count_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  add_vertex(&g, 2, 2);
  add_connection(&g, 0, 1);
  ASSERT(get_vertex(&g, 0)->connections_count == 1 && get_vertex(&g, 1)->connections_count == 1);
}

void get_vertex_connection_index_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  add_vertex(&g, 2, 2);
  add_connection(&g, 0, 1);
  ASSERT(get_vertex(&g, 0)->connections[0] == 1 && get_vertex(&g, 1)->connections[0] == 0);
}

void get_vertexs_count_test()
{
  nav_graph_t g = create_nav_graph();
  add_vertex(&g, 1, 1);
  add_vertex(&g, 2, 2);
  ASSERT(get_vertexs_count(g) == 2);
}

int main()
{
  create_nav_graph_test();
  add_vertex_test();
  add_connection_test();
  get_vertex_test();
  get_vertex_connection_count_test();
  get_vertex_connection_index_test();
  get_vertexs_count_test();
}
