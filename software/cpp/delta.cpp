#include <iostream>
#include <cmath>
#include <vector>
#include <set>

// #define ALT_CI_DIJKSTRA_CHECK_STEP_0(A,B) __builtin_custom_fnff(ALT_CI_DIJKSTRA_CHECK_STEP_0_N,(A),(B))
// #define ALT_CI_DIJKSTRA_CHECK_STEP_0_N 0x0
// #define ALT_CI_LEF_0(A,B) __builtin_custom_inff(ALT_CI_LEF_0_N,(A),(B))
// #define ALT_CI_LEF_0_N 0x1
// #define ALT_CI_LTF_0(A,B) __builtin_custom_inff(ALT_CI_LTF_0_N,(A),(B))
// #define ALT_CI_LTF_0_N 0x2

// extern float __builtin_custom_fnff(int a, float b, float c);
// extern int __builtin_custom_inff(int a, float b, float c);

struct request
{
  int target;
  float newDist;
  int pred;
};

bool operator< (const request& a, const request& b) // required for std::set<request>
{
  return a.target < b.target;
}

class Graph
{
  float **graph;

  float *dist;
  bool *inShortestPath;
  int *predecessor;

  const int NUM_VERTICES;
  const int start;
  const int end;

  std::set<int> lightEdges, heavyEdges;
  std::vector<std::set<int>> buckets;
  int deltaVal;

  std::set<request> findRequests(std::set<int> vertx, bool isLight);
  void relax(request req);

  public:
  Graph(float inArr[9][9], const int p_NUM_VERTICES, int p_start, int p_end) : NUM_VERTICES(p_NUM_VERTICES), start(p_start), end(p_end), deltaVal(1)
  {
    graph = new float*[NUM_VERTICES];
    for (int i = 0; i < NUM_VERTICES; i++) graph[i] = new float[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      for (int j = 0; j < NUM_VERTICES; j++) graph[i][j] = inArr[i][j] < 0 ? __INT_MAX__ : inArr[i][j];
    }

    dist = new float[NUM_VERTICES]; 
    inShortestPath = new bool[NUM_VERTICES];
    predecessor = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      dist[i] = INFINITY;
      inShortestPath[i] = false;
    }

    dist[start] = 0; // dist to source is 0
    predecessor[start] = 0;
  }

  ~Graph()
  {
    for (int i = 0; i < NUM_VERTICES; i++) delete[] graph[i];
    delete[] graph;

    delete[] dist;
    delete[] inShortestPath;
    delete[] predecessor;
  }

  void dijkstra();

  void delta(int p_delta = 1);

  void reset();

  void printAdj() const;
  void print() const;

  int* shortest() const;
};

// bool bucketsEmpty(std::vector<std::set<int>> buckets)
// {
//   for (std::set<int> bucket : buckets) if (bucket.size()) return false;
//   return true;
// }

void Graph::delta(int p_delta)
{
  deltaVal = p_delta;

  for (int i = 0; i < 9; i++) buckets.push_back(std::set<int>());

  buckets.at(0).insert(start); // starting node

  predecessor[start] = start;

  // while (!bucketsEmpty(buckets)) 
  while (true)
  {
    std::set<int> del;

    int firstNonEmpty = -1;
    for (int i = 0; i < buckets.size(); i++) 
      if (buckets.at(i).size()) 
      {
        firstNonEmpty = i;
        break;
      }

    if (firstNonEmpty == -1) break;

    // int firstNonEmpty = 0; // alternative without all-empty detection
    // for (; firstNonEmpty < buckets.size(); firstNonEmpty++) if (buckets.at(firstNonEmpty).size()) break;

    while(buckets.at(firstNonEmpty).size())
    {
      std::set<request> reqs = findRequests(buckets.at(firstNonEmpty), true);
      del.insert(buckets.at(firstNonEmpty).begin(), buckets.at(firstNonEmpty).end());
      buckets.at(firstNonEmpty) = std::set<int>();

      for (request req : reqs) relax(req);
    }

    std::set<request> reqs = findRequests(del, false);
    for (request req : reqs) relax(req);
  }
}

std::set<request> Graph::findRequests(std::set<int> vertx, bool isLight)
{
  std::set<request> light, heavy;

  for (int vert : vertx)
  {
    for (int i = 0; i < NUM_VERTICES; i++)
    if(vert != i && graph[vert][i] <= deltaVal) // double check comparaison, optimal may be to split evenly when weight is equal to delta
      light.insert({i, dist[vert] + graph[vert][i], vert});
    else if (vert != i) heavy.insert({i, dist[vert] + graph[vert][i], vert});
  }

  return isLight ? light : heavy;
}

void Graph::relax(request req)
{
  if (req.newDist != __INT_MAX__ && req.newDist < dist[req.target])
  {
    buckets.at(floor(dist[req.target] / deltaVal)).erase(req.target);
    buckets.at(floor(req.newDist/deltaVal)).insert(req.target);

    dist[req.target] = req.newDist;
    predecessor[req.target] = req.pred;
  }
}

int main()
{
  const int SIZE = 9;
  float adjMatx[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};
  float x[SIZE] = {9.5, 7.74, 4.51, 2.53, 2.30, 6.07, 2.73, 6.71, 10.88};
  float y[SIZE] = {1.72, 4.59, 4.31, 4.38, 10.46, 6.24, 6.40, 9.03, 6.46};

  Graph *myGraph = new Graph(adjMatx, SIZE, 0, 8); // NEED TO DELETE BUT DELETE CAUSES PROBLEMS

  myGraph->delta();
}