#define MAX_CONNECTIONS 10
#define MAX_VERTEXS 10

typedef struct {
  int x;
  int y;
  int connections[MAX_CONNECTIONS];
  int connections_count;
} vertex_t;

typedef struct {
  vertex_t vertexs[MAX_VERTEXS];
  int vertexs_count;
} nav_graph_t;

// Создает навигационный графx
nav_graph_t create_nav_graph();

// Добавляет вершину в навигационный граф
void add_vertex(nav_graph_t *g, int x, int y);

// Создает связь между двумя вершинами
void add_connection(nav_graph_t *g, int vertex1, int vertex2);

// Возвращает вершину под индексу
vertex_t *get_vertex(nav_graph_t *g, int vertex_index);

// Возвращает количество соединений у вершины
int get_vertex_connections_count(nav_graph_t g, int vertex_index);

// Возвращает индекс соединенной вершины
int get_vertex_connection_index(nav_graph_t g, int vertex_index, int connection_index);

// Возвращает количество вершин у навигационного графа
int get_vertexs_count(nav_graph_t g);
