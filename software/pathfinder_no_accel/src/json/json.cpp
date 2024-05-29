#include "json.h"

const int MAX_AVG_OVER = 10;

std::string deserialiseGraph(std::string input, GraphFormat &graph)
{
  StaticJsonDocument<CAPACITY> inDoc;

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

void serialiseResult(ResultFormat res, std::string &output)
{
  StaticJsonDocument<CAPACITY> outDoc;

  JsonArray sht = outDoc["sht"].to<JsonArray>();

  int vert_id = res.end; // starting vertex (dijkstra starts from end)

  int i = 0;

  do
  {
    sht.add(res.shortest[vert_id]);

    vert_id = res.shortest[vert_id];

  } while (res.shortest[vert_id] != res.start && i < 1000);

  if (i >= 1000) printf("WARNING: Predecessor maximum length exceeded");

  sht.add(0);

  if(res.pathfindAvg) outDoc["pathfindAvg"] = res.pathfindAvg;
  
  serializeJson(outDoc, output);
}