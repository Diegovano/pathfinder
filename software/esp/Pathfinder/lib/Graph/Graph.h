#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <ArduinoJson.h>

class Graph
{
private:
    float *x;
    float *y;
    float **adj;
    int size;
    int start;
    int end;

public:
    Graph(std::string);

    int get_start();
    int get_end();
    void copy_adj();
    void copy_x();
    void copy_y();
    std::string to_json();


};

#endif