#include "nav_graph.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Создает навигационный граф
 *
 * @return Навигационный граф
 */
nav_graph_t create_nav_graph()
{
  nav_graph_t g;
  g.vertexs_count = 0;
  return g;
}

/**
 * @brief Добавляет вершину в навигационный граф
 *
 * @param g Указатель на навигационный граф
 * @param x Координата x
 * @param y Координата y
 */
void add_vertex(nav_graph_t *g, int x, int y)
{
  if (g->vertexs_count >= MAX_VERTEXS)
    return;
  
  vertex_t v;
  v.connections_count = 0;
  v.x = x;
  v.y = y;
  g->vertexs[g->vertexs_count] = v;
  g->vertexs_count++;
}

/**
 * @brief Создает связь между двумя вершинами
 *
 * @param g Указатель на навигационный граф
 * @param vertex1_index Индекс первой вершины
 * @param vertex2_index Индекс второй вершины
 */
void add_connection(nav_graph_t *g, int vertex1_index, int vertex2_index)
{
  if (g->vertexs[vertex1_index].connections_count >= MAX_CONNECTIONS || g->vertexs[vertex2_index].connections_count >= MAX_CONNECTIONS)
    return;

  if (vertex1_index >= g->vertexs_count || vertex2_index >= g->vertexs_count)
    return;

  vertex_t *v1 = &g->vertexs[vertex1_index];
  vertex_t *v2 = &g->vertexs[vertex2_index];
  v1->connections[v1->connections_count] = vertex2_index;
  v1->connections_count++;
  v2->connections[v2->connections_count] = vertex1_index;
  v2->connections_count++;
}

/**
 * @brief Возвращает вершину под индексу
 *
 * @param g Указатель на навигационный граф
 * @param vertex_index Индекс вершины
 * @return Вершина
 */
vertex_t *get_vertex(nav_graph_t *g, int vertex_index)
{
  if (vertex_index >= g->vertexs_count)
    return NULL;

  return &g->vertexs[vertex_index];
}

/**
 * @brief Возвращает количество соединений у вершины
 *
 * @param g Навигационный граф
 * @param vertex_index Индекс вершины
 * @return Количество соединений
 */
int get_vertex_connections_count(nav_graph_t g, int vertex_index)
{
  if (vertex_index >= g.vertexs_count)
    return -1;

  return g.vertexs[vertex_index].connections_count;
}

/**
 * @brief Возвращает индекс соединенной вершины
 *
 * @param g Навигационный граф
 * @param vertex_index Индекс вершины
 * @param connection_index Индекс соединения
 * @return Индекс соединенной вершины
 */
int get_vertex_connection_index(nav_graph_t g, int vertex_index, int connection_index)
{
  if (vertex_index >= g.vertexs_count || connection_index >= g.vertexs[vertex_index].connections_count)
    return -1;

  return g.vertexs[vertex_index].connections[connection_index];
}

/**
 * @brief Возвращает количество вершин у навигационного графа
 *
 * @param g Навигационный граф
 * @return Количество вершин
 */
int get_vertexs_count(nav_graph_t g)
{
  return g.vertexs_count;
}
