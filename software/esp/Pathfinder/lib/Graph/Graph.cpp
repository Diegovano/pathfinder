#include <Graph.h>

Graph::Graph(std::string json_in)
{

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, json_in);

    V = doc["V"];
    x = new float[V];
    y = new float[V];
    adj = new float*[V];

    for (int i=0; i<V; i++) 
    {   
        x[i] = doc["x"][i];
        y[i] = doc["y"][i];

        adj[i] = new float[V];
        for (int j=0; j<V; j++)
        {
            adj[i][j] = doc["adj"][i][j];
        }
        
    }
    
    start = doc["start"];
    goal = doc["goal"];

}

std::string Graph::to_json() 
{
    JsonDocument doc;

    

    for (int i=0; i<V; i++) 
    {   
        doc["x"][i] = x[i];
        doc["y"][i] = y[i];

        for (int j=0; j<V; j++)
        {
            doc["adj"][i][j] = adj[i][j];
        }
        
    }
    
    doc["V"] = V;
    doc["start"] = start;
    doc["goal"] = goal;

    std::string json_out;
    serializeJson(doc, json_out);

    return json_out;
}