#pragma once

enum States {
  IDLE,
  GRAPH_RX,
  PATHFINDING,
  ENQUEUE_RESPONSE,
  RESPONSE_TX
};