/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: diego
 * 
 * algos from geeksforgeeks
*/

#include <iostream>
#include <stdio.h>

// dijkstra bottom left to top right

const int NUM_VERTICES = 9;

int myArr[9][9] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};

void dijkstra (int graph[NUM_VERTICES][NUM_VERTICES])
{
  int dist[NUM_VERTICES];
  bool inShortestPath[NUM_VERTICES];

  int predecessor[NUM_VERTICES];

  // initialise

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    dist[i] = __INT_MAX__;
    inShortestPath[i] = false;
  }

  dist[0] = 0; // dist to source is 0 (0 is bottom left)
  predecessor[0] = 0;

  for (int step = 0; step < NUM_VERTICES - 1; step++)
  {
    // get min distance node

    int min = __INT_MAX__;
    int min_index = -1;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && dist[i] <= min) 
      {
        min = dist[i], min_index = i;
      }
    }

    inShortestPath[min_index] = true;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != __INT_MAX__ && dist[min_index] + graph[min_index][i] < dist[i]) 
      {
        dist[i] = dist[min_index] + graph[min_index][i];
        predecessor[i] = min_index;
      }
    }
  }

  std::cout << "distances from bottom left: " << std::endl;

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    std::cout << i << ": " << dist[i] << std::endl;
  }

  std::cout << "path from top right: ";

  int vert_id = NUM_VERTICES - 1;

  std::cout << NUM_VERTICES - 1 << ' ';

  do
  {
    std::cout << predecessor[vert_id] << ' ';

    vert_id = predecessor[vert_id];

  } while (predecessor[vert_id] != 0);

  std::cout << 0 << std::endl;
}

int main () {
  dijkstra(myArr);

  // printf("Hello from NIOS II!\n");

  return 0;
}