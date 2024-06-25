#include "json.h"
#include <vector>

const int MAX_AVG_OVER = 0;

const int SIZE = 219;
std::vector<std::vector<Edge>> adjList(SIZE);

std::string deserialiseGraph(std::string &input, GraphFormat &graph)
{
  StaticJsonDocument<2 * CAPACITY> inDoc; // 2x capacity to play it safe

  printf("%s\n\n\n", input.c_str());
  DeserializationError err = deserializeJson(inDoc, input.c_str());

  if (err)
  {
    printf("\nError parsing following JSON: %s", input.c_str());
    return err.c_str();
  }

  graph.averageOver = std::min(inDoc["avgCount"] | 0, MAX_AVG_OVER);

  if (graph.size != (inDoc["size"] | 219)) 
  {
    std::string errMsg = "graph size mismatch, expected " + std::to_string(graph.size) + " got " + std::to_string((int)inDoc["size"]);
    return errMsg;
  }

  graph.start = inDoc["start"] | 73;
  graph.end = inDoc["end"] | 113;

  for (int i = 0; i < graph.size; i++)
  {
    graph.x[i] = inDoc["x"][i];
    graph.y[i] = inDoc["y"][i];
  }

  if (inDoc["adjMatx"])
  {
    graph.adjMatx = true;

    for (int i = 0; i < graph.size; i++)
    {
      for (int j = 0; j < graph.size; j++)
      {
        graph.adj[i][j] = inDoc["adjMatx"][i][j];
      }
    }
  }
  else
  {
    graph.adjMatx = false;

    for (int i = 0; i < graph.size; i++)
    {
      for (int j = 0; j < graph.size; j++)
      {
        graph.adj[i][j] = i == j ? 0 : -1;
      }
    }

    for (int i = 0; i < graph.size; i++)
    {
      for (size_t j = 0; j < inDoc["adjList"][i].as<JsonArray>().size(); j++)
      {
        Edge thisEdge;
        thisEdge.target = inDoc["adjList"][i][j][0];
        thisEdge.length = inDoc["adjList"][i][j][1];

        graph.adj[i][thisEdge.target] = thisEdge.length;
      }
    }
  }
  return "";
}

std::string serialiseResult(ResultFormat res, std::string &output)
{
  StaticJsonDocument<CAPACITY> outDoc;

  JsonArray sht = outDoc["sht"].to<JsonArray>();
  int vert_id = res.end; // starting vertex (dijkstra starts from end)

  int i = 0;

  if (res.shortest[vert_id] < 0)
  {
    printf("\n\nWARNING: Could not compute shortest path\n\n");
    return "shortest array seems undefined";
  }

  sht.add(res.end);

  do
  {
    sht.add(res.shortest[vert_id]);
    vert_id = res.shortest[vert_id];

  } while (res.shortest[vert_id] != res.start && res.shortest[vert_id] < SIZE && res.shortest[vert_id] >= 0 && i++ < 1000);

  if (i >= 1000)
  {
    printf("\n\nWARNING: Predecessor maximum length exceeded! Was the pathfinding executed?\n\n");
    return "Error during serialisation, maximum predecessor length exceeded (likely undefined predecessor array, or the requested path is impossible)";
  }

  sht.add(res.start);

  if(res.pathfindAvg) outDoc["pathfindAvg"] = res.pathfindAvg;

  // TAGS TO IDENTIFY SYSTEM WHICH GENERATED OUTPUT

  outDoc["max_iterations"] = MAX_AVG_OVER;
  outDoc["iCache"] = ALT_CPU_ICACHE_SIZE;
  outDoc["dCache"] = ALT_CPU_DCACHE_SIZE;
  // outDoc["algorithm"] = "dijkstra";
  outDoc["algorithm"] = "delta";
  outDoc["note"] = "";
  
  serializeJson(outDoc, output);

  return "";
}
