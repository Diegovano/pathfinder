#include "json.h"

std::string deserialiseGraph(std::string input, GraphFormat &graph)
{
  std::string res;

  StaticJsonDocument<CAPACITY> inDoc;

  DeserializationError err = deserializeJson(inDoc, res.c_str());

  if (err) return err.c_str();

  if (graph.size != inDoc["size"]) return "graph size mismatch";

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

  int vert_id = sizeof(res.shortest) / sizeof(res.shortest[0]) - 1; // starting vertex

  do
  {
    sht.add(res.shortest[vert_id]);

    vert_id = res.shortest[vert_id];

  } while (res.shortest[vert_id] != 0);

  sht.add(0);
  
  serializeJson(outDoc, output);
}