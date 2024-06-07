#include "json.h"

const int MAX_AVG_OVER = 1000;

std::string deserialiseGraph(std::string &input, GraphFormat &graph)
{
  StaticJsonDocument<2 * CAPACITY> inDoc; // 2x capacity to play it safe
  DeserializationError err = deserializeJson(inDoc, input.c_str());

  if (err)
  {
    printf("\nError parsing following JSON: %s", input.c_str());
    return err.c_str();
  }

  graph.averageOver = inDoc["avgCount"]; // defaults to 0 if no "avgCount" property

  graph.averageOver = std::min(graph.averageOver, MAX_AVG_OVER);

  if (graph.size != inDoc["size"]) 
  {
    std::string errMsg = "graph size mismatch, expected " + std::to_string(graph.size) + " got " + std::to_string((int)inDoc["size"]);
    return errMsg;
  }

  graph.start = inDoc["start"];
  graph.end = inDoc["end"];

  for (int i = 0; i < graph.size; i++)
  {
    graph.x[i] = inDoc["x"][i];
    graph.y[i] = inDoc["y"][i];
    for (int j = 0; j < graph.size; j++)
    {
      graph.adj[i][j] = inDoc["adj"][i][j];
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

  do
  {
    sht.add(res.shortest[vert_id]);
    vert_id = res.shortest[vert_id];

  } while (res.shortest[vert_id] != res.start && i++ < 1000);

  if (i >= 1000)
  {
    printf("\n\nWARNING: Predecessor maximum length exceeded! Was the pathfinding executed?\n\n");
    return "Error during serialisation, maximum predecessor length exceeded (likely undefined predecessor array, or the requested path is impossible)";
  }

  sht.add(0);

  if(res.pathfindAvg) outDoc["pathfindAvg"] = res.pathfindAvg;

  // TAGS TO IDENTIFY SYSTEM WHICH GENERATED OUTPUT

  outDoc["max_iterations"] = MAX_AVG_OVER;
  outDoc["iCache"] = 16;
  outDoc["dCache"] = 8;
  outDoc["algorithm"] = "dijkstra";
  outDoc["note"] = "";
  
  serializeJson(outDoc, output);

  return "";
}
