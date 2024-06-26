#include "json.h"
#include <vector>

const int MAX_AVG_OVER = 100;

JsonArray addSht(JsonArray sht, int *arr,  int start, int end)
{
  int vert_id = end; // starting vertex (dijkstra starts from end)

  int i = 0;

  if (arr[vert_id] < 0)
  {
    printf("\n\nWARNING: Could not compute shortest path\n\n");
  }

  sht.add(end);

  do
  {
    sht.add(arr[vert_id]);
    vert_id = arr[vert_id];

  } while (vert_id != start && arr[vert_id] != start && arr[vert_id] >= 0 && i++ < 1000);

  if (i >= 1000)
  {
    printf("\n\nWARNING: Predecessor maximum length exceeded! Was the pathfinding executed?\n\n");
    // return "Error during serialisation, maximum predecessor length exceeded (likely undefined predecessor array, or the requested path is impossible)";
  }

  if (vert_id != start) sht.add(start);

  return sht;
}

std::string deserialiseGraph(std::string &input, GraphFormat *graph)
{
  DynamicJsonDocument inDoc(2 * CAPACITY); // 2x capacity to play it safe


  // printf("%s\n\n\n", input.c_str());
  DeserializationError err = deserializeJson(inDoc, input.c_str());

  if (err)
  {
    printf("\nError parsing following JSON: %s", input.c_str());
    return err.c_str();
  }

  const int SIZE = inDoc["size"] | 219;

  graph->size = SIZE;

  graph->x = new float[SIZE];
  graph->y = new float[SIZE];

  graph->adj = new float*[SIZE];
  for (int i = 0; i < SIZE; i++) graph->adj[i] = new float[SIZE]; 

  graph->averageOver = std::min(inDoc["avgCount"] | 0, MAX_AVG_OVER);

  // if (graph.size != ()) 
  // {
  //   std::string errMsg = "graph size mismatch, expected " + std::to_string(graph.size) + " got " + std::to_string((int)inDoc["size"]);
  //   return errMsg;
  // }

  graph->start = inDoc["start"] | 73;
  graph->end = inDoc["end"] | 113;

  for (int i = 0; i < graph->size; i++)
  {
    graph->x[i] = inDoc["x"][i];
    graph->y[i] = inDoc["y"][i];
  }

  if (inDoc["adjMatx"])
  {
    graph->adjMatx = true;

    for (int i = 0; i < graph->size; i++)
    {
      for (int j = 0; j < graph->size; j++)
      {
        graph->adj[i][j] = inDoc["adjMatx"][i][j];
      }
    }
  }
  else
  {
    graph->adjMatx = false;

    for (int i = 0; i < graph->size; i++)
    {
      for (int j = 0; j < graph->size; j++)
      {
        graph->adj[i][j] = i == j ? 0 : -1;
      }
    }

    for (int i = 0; i < graph->size; i++)
    {
      for (size_t j = 0; j < inDoc["adjList"][i].as<JsonArray>().size(); j++)
      {
        Edge thisEdge;
        thisEdge.target = inDoc["adjList"][i][j][0];
        thisEdge.length = inDoc["adjList"][i][j][1];

        graph->adj[i][thisEdge.target] = thisEdge.length;
      }
    }
  }
  return "";
}

std::string serialiseResult(ResultFormat res, std::string &output)
{
  StaticJsonDocument<CAPACITY> outDoc;

  if (res.swDijkstraShortest)
  {
    JsonArray swDijkstraSht = outDoc["sw_dj_sht"].to<JsonArray>();
    JsonArray dijkstraSht = outDoc["hybrid_dj_sht"].to<JsonArray>();
    // JsonArray deltaSht = outDoc["delta_sht"].to<JsonArray>();
    // JsonArray swAstarSht = outDoc["sw_astar_sht"].to<JsonArray>();
    // JsonArray astarSht = outDoc["hybrid_astar_sht"].to<JsonArray>();

    swDijkstraSht = addSht(swDijkstraSht, res.swDijkstraShortest, res.start, res.end);
    dijkstraSht = addSht(dijkstraSht, res.dijkstraShortest, res.start, res.end);
    // deltaSht = addSht(deltaSht, res.deltaShortest, res.start, res.end);
    // swAstarSht = addSht(swAstarSht, res.swAstarShortest, res.start, res.end);
    // astarSht = addSht(astarSht, res.astarShortest, res.start, res.end);

    if(res.swDijkstraAvg) outDoc["sw_dj_time"] = res.swDijkstraAvg;
    if(res.dijkstraAvg) outDoc["hybrid_dj_time"] = res.dijkstraAvg;
    // if(res.swAstarAvg) outDoc["sw_astar_time"] = res.swAstarAvg;
    // if(res.astarAvg) outDoc["hybrid_astar_time"] = res.astarAvg;
    // if(res.deltaAvg) outDoc["delta_time"] = res.deltaAvg;
    if(res.hwDijkstraAvg) outDoc["hw_dj_time"] = res.hwDijkstraAvg;
  }
  else
  {
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

    } while (res.shortest[vert_id] != res.start && res.shortest[vert_id] >= 0 && i++ < 1000);

    if (i >= 1000)
    {
      printf("\n\nWARNING: Predecessor maximum length exceeded! Was the pathfinding executed?\n\n");
      return "Error during serialisation, maximum predecessor length exceeded (likely undefined predecessor array, or the requested path is impossible)";
    }

    sht.add(res.start);

    if(res.pathfindAvg) outDoc["pathfindAvg"] = res.pathfindAvg;
  }


  // TAGS TO IDENTIFY SYSTEM WHICH GENERATED OUTPUT

  outDoc["max_iterations"] = MAX_AVG_OVER;
  outDoc["iCache"] = ALT_CPU_ICACHE_SIZE;
  outDoc["dCache"] = ALT_CPU_DCACHE_SIZE;
  // outDoc["algorithm"] = "dijkstra";
  outDoc["algorithm"] = "delta";
  outDoc["note"] = "";
  if (res.distance) outDoc["total_distance"] = res.distance;
  else outDoc["total_distance"] = 0;
  
  serializeJson(outDoc, output);

  return "";
}

#define MAX_HARDWARE_NODES 128
void GraphFormat::matrixReshapeHWDijkstra(float* HWDijkstra_uncached_matrix)
{
  float *temp = HWDijkstra_uncached_matrix;
  for(int from = 0; from < MAX_HARDWARE_NODES; from++){
   	 for (int column = 0; column < MAX_HARDWARE_NODES ;column++){
   		 	 *temp = (adj[column][from] == -1) ? 0x7f800000 : adj[column][from];
   		 	 temp++;
     }
  }
}