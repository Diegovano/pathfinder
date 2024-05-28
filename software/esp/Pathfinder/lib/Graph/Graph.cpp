#include <Graph.h>

Graph::Graph(std::string json_in)
{

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, json_in);

    size = doc["size"];
    x = new float[size];
    y = new float[size];
    adj = new float*[size];

    for (int i=0; i<size; i++) 
    {   
        x[i] = doc["x"][i];
        y[i] = doc["y"][i];

        adj[i] = new float[size];
        for (int j=0; j<size; j++)
        {
            adj[i][j] = doc["adj"][i][j];
        }
        
    }
    
    start = doc["start"];
    end = doc["end"];

}

std::string Graph::to_json() 
{
    JsonDocument doc;

    

    for (int i=0; i<size; i++) 
    {   
        doc["x"][i] = x[i];
        doc["y"][i] = y[i];

        for (int j=0; j<size; j++)
        {
            doc["adj"][i][j] = adj[i][j];
        }
        
    }
    
    doc["size"] = size;
    doc["start"] = start;
    doc["end"] = end;

    std::string json_out;
    serializeJson(doc, json_out);

    return json_out;
}