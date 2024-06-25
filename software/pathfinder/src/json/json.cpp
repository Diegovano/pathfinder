#include "json.h"
#include <vector>

const int MAX_AVG_OVER = 0;

const int SIZE = 219;
std::vector<std::vector<Edge>> adjList(SIZE);

std::string deserialiseGraph(std::string &input, GraphFormat &graph)
{
  StaticJsonDocument<2 * CAPACITY> inDoc; // 2x capacity to play it safe

  printf("%s\n\n\n", input.c_str());
  // DeserializationError err = deserializeJson(inDoc, input.c_str());

  // if (err)
  // {
  //   printf("\nError parsing following JSON: %s", input.c_str());
  //   return err.c_str();
  // }

  graph.averageOver = 1000; // defaults to 0 if no "avgCount" property
  // graph.averageOver = inDoc["avgCount"] || 1000; // defaults to 0 if no "avgCount" property

  graph.averageOver = std::min(graph.averageOver, MAX_AVG_OVER);

  if (graph.size != 219) 
  // if (graph.size != (inDoc["size"] || 219)) 
  {
    std::string errMsg = "graph size mismatch, expected " + std::to_string(graph.size) + " got " + std::to_string((int)inDoc["size"]);
    return errMsg;
  }

  graph.start = 73;
  // graph.start = inDoc["start"] || 73;
  graph.end = 113;
  // graph.end = inDoc["end"] || 113;

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
    adjList[0] = {{1, 303.519}, {2, 156.256}, {3, 41.309}, {4, 184.269}};
    adjList[1] = {{43, 6.469}, {44, 158.274}, {45, 130.929}, {0, 303.519}};
    adjList[2] = {{56, 8.134}, {53, 18.088}, {0, 156.256}};
    adjList[3] = {{212, 12.853}, {218, 41.046}};
    adjList[4] = {{15, 33.532}, {0, 184.269}, {16, 53.887}};
    adjList[5] = {{6, 9.148}, {7, 13.492}};
    adjList[6] = {{94, 36.41}, {5, 9.148}, {215, 64.191}};
    adjList[7] = {{194, 160.067}, {195, 31.353}};
    adjList[8] = {{9, 90.08}, {10, 135.415}};
    adjList[9] = {{82, 64.446}, {10, 45.849}};
    adjList[10] = {{9, 45.849}, {84, 49.829}, {8, 135.415}};
    adjList[11] = {{12, 9.495}, {13, 28.159}, {14, 25.482}};
    adjList[12] = {{11, 9.495}, {8, 40.632}, {83, 95.945}};
    adjList[13] = {{11, 28.159}, {14, 41.505}, {81, 64.061}};
    adjList[14] = {};
    adjList[15] = {{4, 33.532}, {112, 181.396}, {113, 183.129}};
    adjList[16] = {{20, 19.803}, {4, 53.887}};
    adjList[17] = {{18, 11.932}, {19, 87.616}, {20, 299.832}};
    adjList[18] = {{17, 11.932}, {145, 45.248}};
    adjList[19] = {{22, 11.471}, {23, 99.879}};
    adjList[20] = {{16, 19.803}, {67, 28.225}, {17, 299.832}};
    adjList[21] = {{22, 99.828}, {23, 12.184}, {24, 50.876}};
    adjList[22] = {{19, 11.471}, {122, 52.567}, {18, 87.146}};
    adjList[23] = {{21, 12.184}, {118, 21.38}};
    adjList[24] = {{163, 61.58}, {159, 45.668}, {21, 50.876}, {144, 31.192}};
    adjList[25] = {{26, 11.11}, {27, 90.188}};
    adjList[26] = {{25, 11.11}, {86, 42.316}, {174, 54.822}};
    adjList[27] = {{72, 52.166}, {25, 90.188}, {73, 83.867}};
    adjList[28] = {{29, 98.271}, {30, 22.932}, {31, 43.708}};
    adjList[29] = {{64, 8.946}, {167, 30.487}, {28, 98.271}};
    adjList[30] = {{101, 187.399}, {28, 22.932}};
    adjList[31] = {{32, 225.606}, {28, 43.708}};
    adjList[32] = {{31, 225.606}};
    adjList[33] = {{34, 161.072}, {35, 57.548}, {36, 82.925}};
    adjList[34] = {{57, 55.096}, {145, 12.546}, {17, 44.789}};
    adjList[35] = {{57, 160.516}, {33, 57.548}, {55, 92.211}};
    adjList[36] = {{126, 69.516}, {127, 227.766}, {33, 82.925}};
    adjList[37] = {{38, 6.012}, {39, 84.841}};
    adjList[38] = {{37, 6.012}};
    adjList[39] = {{62, 84.941}};
    adjList[40] = {{41, 9.452}, {42, 292.743}};
    adjList[41] = {{40, 9.452}, {43, 19.619}};
    adjList[42] = {{211, 13.449}, {40, 292.743}};
    adjList[43] = {{1, 6.469}, {41, 19.028}};
    adjList[44] = {{152, 31.392}, {186, 36.661}, {1, 158.274}};
    adjList[45] = {{58, 102.963}, {59, 27.134}, {1, 130.929}};
    adjList[46] = {{47, 63.576}, {48, 30.539}};
    adjList[47] = {{49, 21.383}, {50, 62.293}, {46, 63.576}};
    adjList[48] = {{201, 46.425}, {46, 30.539}};
    adjList[49] = {{111, 61.768}, {90, 62.151}, {141, 85.51}};
    adjList[50] = {{47, 62.293}, {64, 10.137}};
    adjList[51] = {{52, 41.125}, {53, 35.057}, {54, 72.929}};
    adjList[52] = {{51, 41.125}, {66, 28.433}, {54, 53.581}};
    adjList[53] = {{51, 35.057}, {56, 18.83}};
    adjList[54] = {{52, 53.581}, {51, 72.929}, {67, 62.337}};
    adjList[55] = {{56, 83.249}, {35, 92.211}, {57, 197.262}};
    adjList[56] = {{2, 8.134}, {55, 83.249}};
    adjList[57] = {{34, 55.096}, {55, 197.262}, {35, 160.516}};
    adjList[58] = {{181, 17.978}, {45, 102.963}, {134, 26.784}};
    adjList[59] = {{180, 92.753}, {45, 27.134}};
    adjList[60] = {{37, 77.749}, {61, 52.921}, {62, 65.778}, {63, 49.173}};
    adjList[61] = {{156, 9.761}, {69, 106.961}, {60, 52.921}};
    adjList[62] = {{193, 73.342}, {60, 65.778}};
    adjList[63] = {{192, 68.171}, {60, 49.173}, {68, 66.895}};
    adjList[64] = {{50, 10.137}, {29, 8.946}, {65, 35.414}};
    adjList[65] = {{97, 68.541}, {64, 35.414}};
    adjList[66] = {{52, 28.433}};
    adjList[67] = {{54, 62.337}, {16, 33.73}};
    adjList[68] = {{69, 113.846}, {63, 66.895}, {70, 49.887}, {71, 44.508}};
    adjList[69] = {{209, 8.879}, {208, 180.91}};
    adjList[70] = {{178, 44.999}, {177, 19.229}, {68, 49.887}};
    adjList[71] = {{217, 6.779}, {68, 44.508}};
    adjList[72] = {{172, 44.731}, {27, 52.166}};
    adjList[73] = {{108, 17.191}, {109, 80.303}, {27, 83.867}};
    adjList[74] = {{75, 10.796}, {76, 154.774}, {77, 122.668}};
    adjList[75] = {{112, 41.055}, {74, 10.796}, {117, 143.458}};
    adjList[76] = {{74, 154.774}};
    adjList[77] = {{74, 122.668}};
    adjList[78] = {{79, 180.735}, {80, 66.795}};
    adjList[79] = {{11, 70.616}};
    adjList[80] = {{78, 66.795}, {86, 42.77}};
    adjList[81] = {{13, 64.061}};
    adjList[82] = {{191, 66.782}, {148, 187.738}};
    adjList[83] = {{84, 143.796}, {12, 95.945}, {85, 87.682}};
    adjList[84] = {{10, 49.829}, {146, 92.489}};
    adjList[85] = {{149, 5.95}, {83, 87.682}};
    adjList[86] = {{166, 166.008}, {80, 42.77}, {26, 42.316}};
    adjList[87] = {{39, 63.597}, {88, 7.721}};
    adjList[88] = {{102, 44.375}};
    adjList[89] = {{87, 70.767}, {90, 121.925}};
    adjList[90] = {{49, 62.151}, {50, 23.779}};
    adjList[91] = {{89, 59.352}};
    adjList[92] = {{93, 79.946}, {94, 2.434}, {95, 46.751}};
    adjList[93] = {{92, 79.946}, {100, 39.623}};
    adjList[94] = {{92, 2.434}, {6, 36.41}, {96, 92.613}};
    adjList[95] = {{99, 4.425}, {161, 90.643}, {92, 46.751}};
    adjList[96] = {{97, 115.514}, {94, 92.613}};
    adjList[97] = {{98, 66.199}, {65, 68.541}, {96, 115.514}};
    adjList[98] = {{99, 50.721}, {97, 66.199}, {65, 183.491}};
    adjList[99] = {{98, 50.721}, {95, 4.425}, {161, 27.53}};
    adjList[100] = {{93, 39.623}, {197, 10.153}, {196, 32.566}};
    adjList[101] = {{102, 2.466}, {30, 187.399}};
    adjList[102] = {{101, 2.466}, {88, 44.375}, {103, 173.401}};
    adjList[103] = {{216, 36.882}, {102, 173.401}};
    adjList[104] = {{105, 17.304}, {106, 11.51}, {107, 119.501}};
    adjList[105] = {{104, 17.304}};
    adjList[106] = {{104, 11.51}};
    adjList[107] = {{31, 50.696}, {150, 9.735}, {104, 119.501}};
    adjList[108] = {{73, 17.191}, {162, 23.23}, {203, 36.406}};
    adjList[109] = {{216, 73.829}};
    adjList[110] = {{111, 48.283}};
    adjList[111] = {{49, 61.768}, {46, 24.546}, {110, 48.283}};
    adjList[112] = {{75, 41.055}, {15, 181.396}, {156, 266.825}};
    adjList[113] = {{15, 183.129}, {114, 282.168}, {151, 42.826}};
    adjList[114] = {{115, 43.977}, {116, 50.728}, {113, 282.168}};
    adjList[115] = {{114, 43.977}, {184, 103.918}, {131, 84.102}};
    adjList[116] = {{114, 50.728}, {171, 26.736}, {188, 145.457}};
    adjList[117] = {{75, 143.458}};
    adjList[118] = {{119, 82.659}, {120, 55.22}, {121, 12.015}};
    adjList[119] = {{139, 11.912}, {140, 187.945}};
    adjList[120] = {{118, 55.22}};
    adjList[121] = {{118, 12.015}, {21, 21.481}};
    adjList[122] = {{22, 52.567}, {123, 25.604}, {124, 45.22}};
    adjList[123] = {{122, 25.604}, {143, 163.854}, {144, 100.741}};
    adjList[124] = {{122, 45.22}, {125, 162.404}};
    adjList[125] = {{124, 162.404}};
    adjList[126] = {{36, 69.516}, {128, 87.123}};
    adjList[127] = {{36, 227.766}};
    adjList[128] = {{129, 9.381}, {126, 87.123}, {185, 65.664}};
    adjList[129] = {{128, 9.381}, {129, 139.486}, {129, 139.486}};
    adjList[130] = {{131, 25.312}};
    adjList[131] = {{130, 25.312}, {151, 109.154}, {115, 84.102}};
    adjList[132] = {{133, 47.027}, {100, 49.601}};
    adjList[133] = {{132, 47.027}};
    adjList[134] = {{135, 5.829}, {58, 26.784}};
    adjList[135] = {{138, 33.427}, {134, 5.829}, {136, 179.075}};
    adjList[136] = {{137, 27.176}, {135, 179.075}, {138, 121.893}};
    adjList[137] = {{171, 13.794}, {136, 27.176}, {183, 46.414}};
    adjList[138] = {{135, 33.427}, {187, 43.996}, {136, 121.893}};
    adjList[139] = {{148, 67.296}, {121, 82.785}, {119, 11.912}};
    adjList[140] = {{206, 9.517}, {207, 87.655}};
    adjList[141] = {{91, 40.412}, {142, 39.009}};
    adjList[142] = {{141, 39.009}};
    adjList[143] = {{123, 163.854}, {157, 101.325}};
    adjList[144] = {{24, 31.192}, {123, 100.741}, {154, 52.178}};
    adjList[145] = {{34, 12.546}, {33, 160.778}};
    adjList[146] = {{147, 56.067}, {148, 66.093}, {149, 148.135}};
    adjList[147] = {{146, 56.067}, {154, 45.086}, {159, 82.252}, {160, 158.953}};
    adjList[148] = {{139, 67.296}, {146, 66.093}, {82, 187.738}};
    adjList[149] = {{85, 5.95}};
    adjList[150] = {{107, 9.735}};
    adjList[151] = {{131, 109.154}, {152, 84.72}, {153, 65.657}, {113, 42.826}};
    adjList[152] = {{184, 3.726}, {44, 31.392}, {151, 84.72}};
    adjList[153] = {{151, 65.657}};
    adjList[154] = {{147, 45.086}, {155, 5.341}, {158, 109.966}};
    adjList[155] = {{154, 5.341}, {144, 52.294}};
    adjList[156] = {{61, 9.761}, {112, 266.825}, {200, 140.769}};
    adjList[157] = {{158, 3.927}, {160, 0.974}, {143, 101.325}};
    adjList[158] = {{157, 3.927}, {155, 109.914}};
    adjList[159] = {{164, 9.493}, {24, 45.668}, {147, 82.252}};
    adjList[160] = {{157, 0.974}, {147, 158.953}};
    adjList[161] = {{95, 90.643}, {99, 27.53}};
    adjList[162] = {{108, 23.23}, {71, 61.58}};
    adjList[163] = {{24, 61.58}};
    adjList[164] = {{159, 9.493}};
    adjList[165] = {{80, 170.36}, {79, 105.808}};
    adjList[166] = {{165, 37.495}, {86, 166.008}};
    adjList[167] = {{29, 30.487}, {168, 27.624}, {169, 48.065}};
    adjList[168] = {{167, 27.624}};
    adjList[169] = {{167, 48.065}};
    adjList[170] = {{171, 66.257}};
    adjList[171] = {{137, 13.794}, {170, 66.257}, {116, 26.736}};
    adjList[172] = {{173, 44.014}, {72, 44.731}, {175, 61.797}, {176, 19.581}};
    adjList[173] = {{174, 11.273}, {172, 44.014}, {25, 53.42}};
    adjList[174] = {{173, 11.273}, {190, 105.928}};
    adjList[175] = {{172, 61.797}};
    adjList[176] = {{172, 19.581}};
    adjList[177] = {{70, 19.229}};
    adjList[178] = {{179, 8.248}, {70, 44.999}, {210, 16.945}};
    adjList[179] = {{178, 8.248}};
    adjList[180] = {{182, 67.496}, {181, 13.492}, {59, 92.753}};
    adjList[181] = {{58, 17.978}, {134, 64.049}, {180, 13.492}};
    adjList[182] = {{180, 67.496}};
    adjList[183] = {{137, 46.414}};
    adjList[184] = {{115, 103.918}, {152, 3.726}, {188, 47.556}};
    adjList[185] = {{128, 65.664}};
    adjList[186] = {{44, 36.661}};
    adjList[187] = {{138, 43.996}};
    adjList[188] = {{189, 7.153}, {184, 47.556}, {116, 145.457}};
    adjList[189] = {{188, 7.153}};
    adjList[190] = {{191, 9.331}, {166, 46.897}};
    adjList[191] = {{140, 11.572}, {139, 188.014}};
    adjList[192] = {{63, 68.171}};
    adjList[193] = {{199, 5.414}, {91, 112.385}, {62, 73.342}};
    adjList[194] = {{198, 52.481}};
    adjList[195] = {{196, 9.798}, {197, 30.856}};
    adjList[196] = {{6, 44.174}};
    adjList[197] = {{100, 10.153}, {202, 105.792}};
    adjList[198] = {{195, 155.121}, {194, 31.107}};
    adjList[199] = {{193, 5.414}, {200, 22.298}, {61, 125.363}};
    adjList[200] = {{213, 174.384}};
    adjList[201] = {{213, 12.354}, {199, 185.367}};
    adjList[202] = {};
    adjList[203] = {{204, 13.168}, {205, 7.835}, {108, 36.406}};
    adjList[204] = {{203, 13.168}};
    adjList[205] = {{203, 7.835}};
    adjList[206] = {{190, 11.367}, {173, 106.016}};
    adjList[207] = {{208, 9.088}, {209, 180.81}};
    adjList[208] = {{72, 106.915}, {206, 87.726}};
    adjList[209] = {{156, 107.001}};
    adjList[210] = {{178, 16.945}};
    adjList[211] = {{0, 40.113}};
    adjList[212] = {{42, 11.386}, {211, 9.717}};
    adjList[213] = {{5, 87.628}};
    adjList[214] = {{48, 34.092}, {96, 29.532}};
    adjList[215] = {{201, 33.122}, {214, 38.258}};
    adjList[216] = {{103, 36.882}, {217, 47.677}, {109, 73.829}};
    adjList[217] = {{71, 6.779}, {162, 62.442}, {216, 47.677}};
    adjList[218] = {{212, 31.976}};

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
      for (size_t j = 0; j < adjList.at(i).size(); j++)
      {
        Edge thisEdge = adjList.at(i).at(j);

        graph.adj[i][thisEdge.target] = thisEdge.length;
      }
      // for (size_t j = 0; j < inDoc["adjList"][i].as<JsonArray>().size(); j++)
      // {
      //   Edge thisEdge;
      //   thisEdge.target = inDoc["adjList"][i][j][0];
      //   thisEdge.length = inDoc["adjList"][i][j][1];

      //   graph.adj[i][thisEdge.target] = thisEdge.length;
      // }
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

#define MAX_HARDWARE_NODES 128
void GraphFormat::Full_HW_dijkstra_reshape(float* HWDijkstra_uncached_matrix)
{
  *float temp = HWDijkstra_uncached_matrix;
  for(int from = 0; from<MAX_HARDWARE_NODES;from++){
   	 for (int column = 0; column <MAX_HARDWARE_NODES ; column++){
   		 	 *temp = (graph.adj[column][from] == -1) ? 0x7f800000 : graph.adj[column][from];
   		 	 temp++;
     }
  }
}