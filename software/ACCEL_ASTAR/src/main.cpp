#include <cstdio>
#include <cstddef>
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdio>
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"
#include "alt_types.h"
#include "sys/alt_timestamp.h"
#include "sys/alt_cache.h"
#include <unistd.h>
#include "DMA/DMA.h"
#include "Accelerator/Accelerator.h"

#define EUCLID_DIST_BASE 0x1001000
#define EUCLID_DIST_SPAN 64
#define EUCLID_DIST_IRQ 5
#define EUCLID_DIST_IRQ_INTERRUPT_CONTROLLER_ID 0

constexpr double PI = 3.14159265358979323846;
const float MAX_FLOAT = 3.40282e+38;

// Struct for a Node
struct Node {
    float x;
    float y;
};

// Struct for an Edge
struct Edge {
    int target;
    float length;
};

float get_distance(DMA& dma, Accelerator& accel, const Node& n1, const Node& n2)
{
    accel.input[0] = n1.x;
    accel.input[1] = n2.x;
    accel.input[2] = n1.y;
    accel.input[3] = n2.y;

    dma.copy((void*)(accel.input), (void*)(accel.base()+4), 16);

    accel.exec();

    dma.copy((void*)(EUCLID_DIST_BASE+36), (void*)(accel.res), 4);

    return *(accel.res);
}

std::vector<int> findPath(DMA& dma, Accelerator& accel, const std::vector<Node>& nodes, const std::vector<std::vector<Edge>>& adjList, int source, int target) {

    // g_value is a vector of floats which stores the cost of the cheapest path to a node
    std::vector<float> g_value(nodes.size(), MAX_FLOAT);

    // f_value is a vector of floats which stores the estimated total cost from the start node to the goal node
    std::vector<float> f_value(nodes.size(), MAX_FLOAT);

    // came_from is a vector of ints which stores the parent node of a node
    std::vector<int> came_from(nodes.size(), -1);

    // closed_nodes is a set of ints which stores the nodes that have been visited
    std::set<int> closed_nodes;

    // initialise the g value of the source node to 0
    g_value[source] = 0;
    // initialise the f value of the source node to the heuristic value
    f_value[source] = get_distance(dma, accel, nodes[source], nodes[target]);

    // create a priority queue of pairs of floats and ints which represents the f value and the node
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> queue;
    // push the source node onto the queue
    queue.push({0.0f, source});

    // while the queue is not empty
    while (!queue.empty()) {
        // get the node with the smallest f value
        int current_node = queue.top().second;
        // remove the node from the queue
        queue.pop();

        // if the node is in the closed nodes set, skip the rest of the loop
        if (closed_nodes.count(current_node)) continue;
        // if the node is the target node, we have found the path
        if (current_node == target) break;

        // add the node to the closed nodes set
        closed_nodes.insert(current_node);

        // for each neighbour of the current node
        for (auto& edge : adjList[current_node]) {
            // get the neighbour node
            int neighbour = edge.target;
            // if the neighbour node is in the closed nodes set, skip the rest of the loop
            if (closed_nodes.count(neighbour)) continue;

            // calculate the tentative g value
//            float temp_g_score = g_value[current_node]+edge.length;
            float temp_g_score = g_value[current_node] + edge.length;

            // if the tentative g value is less than the g value of the neighbour node
            if (temp_g_score < g_value[neighbour]) {
                // update the came from, g value and f value of the neighbour node
                came_from[neighbour] = current_node;
                g_value[neighbour] = temp_g_score;
//                f_value[neighbour] = temp_g_score+get_distance(dma, accel, nodes[neighbour], nodes[target]);
                f_value[neighbour] = temp_g_score + get_distance(dma, accel, nodes[neighbour], nodes[target]);
                // push the f value and the neighbour node to the queue
                queue.push({f_value[neighbour], neighbour});
            }
        }
    }

    // reconstruct the path
    std::vector<int> path;
    // start from the target node
    for (int current_node = target; current_node != -1; current_node = came_from[current_node]) {
        path.push_back(current_node);
    }
    std::reverse(path.begin(), path.end());

    return path;
}

int main(){
    printf("Pathfinding\n");
    //INITIALISE DMA
    printf("INITIALISE DMA\n");
    int ctrl = DMA_WORD | DMA_LEEN | DMA_IRQ_E_BIT;
    DMA dma(DMA_BASE, ctrl);
    if (dma.irq_reg(DMA_IRQ_INTERRUPT_CONTROLLER_ID,DMA_IRQ))
    {
        printf("ERROR failed to register dma isr\n");
    }

    //INITIALISE ACCELERATOR
    printf("INITIALISE ACCELERATOR\n");
    Accelerator accel(EUCLID_DIST_BASE, EUCLID_DIST_SPAN);
    if (accel.irq_reg(EUCLID_DIST_IRQ_INTERRUPT_CONTROLLER_ID,EUCLID_DIST_IRQ))
    {
        printf("ERROR failed to register custom isr\n");
    }

    // create nodes list which is a vector of Node objects
    std::vector<Node> nodes(109);

    // create adjacency list which is a vector of vectors of Edge objects
    std::vector<std::vector<Edge>> adjList(109);

    // Initialise the nodes list with the coordinates of the nodes
//    nodes[0]  = {5709222.9834205909, 696108.62011866691}; // Node 276548
//    nodes[1]  = {5709191.8782056486, 696118.27943041269}; // Node 25291708
//    nodes[2]  = {5709211.2912476128, 696055.84373104712}; // Node 25291709
//    nodes[3]  = {5709236.2511749854, 696033.52349726949}; // Node 25473589
//    nodes[4]  = {5709284.5927414894, 695997.60383067257}; // Node 25473590
//    nodes[5]  = {5709208.1398356995, 696036.22818386171}; // Node 25473592
//    nodes[6]  = {5709009.2122519007, 696190.09544731071}; // Node 26389375
//    nodes[7]  = {5709019.8233349705, 696188.06609560957}; // Node 26389434
//    nodes[8]  = {5709006.0310580395, 696323.27681472222}; // Node 26389442
//    nodes[9]  = {5709029.0720731048, 696311.54391213995}; // Node 32618386
//    nodes[10] = {5709012.7329801535, 696147.49481777672}; // Node 34519894
//    nodes[11] = {5709225.9788216352, 696298.79496300314}; // Node 1691189781
//    nodes[12] = {5709058.8068370707, 696308.78188286372}; // Node 3764277424
//
//    // Initialise the adjacency list with the edges
//    adjList[0]  = {{6, 33.532}, {12, 53.887}};
//    adjList[1]  = {{11, 62.337}, {12, 33.73}};
//    adjList[2]  = {{12, 19.803}, {1, 28.225}};
//    adjList[3]  = {{8, 10.796}, {5, 154.774}, {4, 122.668}};
//    adjList[4]  = {{3, 122.668}};
//    adjList[5]  = {{3, 154.774}};
//    adjList[6]  = {{0, 33.532}, {10, 181.396}, {9, 183.129}};
//    adjList[7]  = {{8, 143.458}};
//    adjList[8]  = {{10, 41.055}, {3, 10.796}, {7, 143.458}};
//    adjList[9]  = {{6, 183.129}};
//    adjList[10] = {{8, 41.055}, {6, 181.396}};
//    adjList[11] = {{1, 62.337}};
//    adjList[12] = {{0, 53.887}, {2, 19.803}};


    // Initialise the nodes list with the coordinates of the nodes
    nodes[0] = {5709406.592593853, 696093.7863483471}; // Node 109670
    nodes[1] = {5709438.015823614, 696396.2634310309}; // Node 12196781
    nodes[2] = {5709381.557287798, 695939.0986185186}; // Node 3764277440
    nodes[3] = {5709445.713900904, 696085.5413105368}; // Node 7911351682
    nodes[4] = {5709222.983420591, 696108.6201186669}; // Node 276548
    nodes[5] = {5709191.878205649, 696118.2794304127}; // Node 26389375
    nodes[6] = {5709211.291247613, 696055.8437310471}; // Node 3764277424
    nodes[7] = {5709160.448641967, 695739.23620667}; // Node 276549
    nodes[8] = {5709158.147415086, 695727.4894879166}; // Node 860183706
    nodes[9] = {5709073.889511777, 695753.1787173111}; // Node 276551
    nodes[10] = {5709208.139835699, 696036.2281838617}; // Node 25291709
    nodes[11] = {5708973.6509467, 695758.5931036496}; // Node 276550
    nodes[12] = {5709072.126067131, 695741.8072089367}; // Node 860183709
    nodes[13] = {5708975.4864559425, 695770.6779056357}; // Node 3960600156
    nodes[14] = {5708964.289938448, 695708.420401278}; // Node 265563776
    nodes[15] = {5709361.046023356, 695698.5969165337}; // Node 12195919
    nodes[16] = {5709204.718285029, 695732.2408049735}; // Node 29353839
    nodes[17] = {5709371.905924698, 695755.2959538447}; // Node 385312258
    nodes[18] = {5709469.453153156, 696397.2155050545}; // Node 12196664
    nodes[19] = {5709460.002069923, 696397.567223864}; // Node 1973883128
    nodes[20] = {5709454.374156674, 696103.9215622969}; // Node 4322519851
    nodes[21] = {5709444.412417221, 696397.259933658}; // Node 1973882968
    nodes[22] = {5709281.325204759, 696419.0041420648}; // Node 392512612
    nodes[23] = {5708769.674973994, 696483.461031195}; // Node 14457894
    nodes[24] = {5708708.673839894, 696500.9108534758}; // Node 14457897
    nodes[25] = {5708800.075679975, 696482.0831574204}; // Node 1960925514
    nodes[26] = {5708701.434384973, 696480.721817096}; // Node 27089782
    nodes[27] = {5708650.455622036, 696523.2299281717}; // Node 1960954870
    nodes[28] = {5709332.6284358045, 695952.0228566583}; // Node 20821126
    nodes[29] = {5709338.004398434, 695992.9288725917}; // Node 1691189790
    nodes[30] = {5709363.779561127, 695937.1995158533}; // Node 1691189798
    nodes[31] = {5709284.592741489, 695997.6038306726}; // Node 1691189781
    nodes[32] = {5709379.090655007, 695847.4676591664}; // Node 20947178
    nodes[33] = {5709381.120316871, 695930.9498430908}; // Node 3764277439
    nodes[34] = {5709215.018426711, 695786.5446752683}; // Node 29353841
    nodes[35] = {5708687.444765689, 696200.9779529992}; // Node 20963439
    nodes[36] = {5708739.947435228, 696194.1049815264}; // Node 6280192965
    nodes[37] = {5708715.397339266, 696260.7281027433}; // Node 1668506746
    nodes[38] = {5708659.3383828215, 696160.5763704968}; // Node 1360237602
    nodes[39] = {5709340.304641033, 696021.3610122055}; // Node 25291706
    nodes[40] = {5709236.251174985, 696033.5234972695}; // Node 25291708
    nodes[41] = {5709009.212251901, 696190.0954473107}; // Node 25473589
    nodes[42] = {5709019.8233349705, 696188.0660956096}; // Node 26389442
    nodes[43] = {5709006.0310580395, 696323.2768147222}; // Node 25473592
    nodes[44] = {5709029.072073105, 696311.54391214}; // Node 25473590
    nodes[45] = {5708664.519258925, 695679.5507079257}; // Node 26389013
    nodes[46] = {5708674.89556865, 695743.3657228529}; // Node 1116949690
    nodes[47] = {5708709.68251757, 695671.4716524454}; // Node 265563774
    nodes[48] = {5708741.458335954, 696416.0911551546}; // Node 26389363
    nodes[49] = {5708759.969653688, 696460.8376846968}; // Node 27089784
    nodes[50] = {5709012.7329801535, 696147.4948177767}; // Node 34519894
    nodes[51] = {5709225.978821635, 696298.7949630031}; // Node 32618386
    nodes[52] = {5709101.5239167195, 696453.6833576608}; // Node 26389381
    nodes[53] = {5709144.493144636, 696444.1743869598}; // Node 387940759
    nodes[54] = {5709113.294114814, 696503.1909305681}; // Node 392513282
    nodes[55] = {5709058.806837071, 696308.7818828637}; // Node 26389434
    nodes[56] = {5708954.459312898, 695774.6022412439}; // Node 26389449
    nodes[57] = {5708872.963502869, 695788.7534463173}; // Node 26966492
    nodes[58] = {5708963.282568445, 695829.2923988366}; // Node 26389456
    nodes[59] = {5708952.55292382, 695762.7008228918}; // Node 3960591923
    nodes[60] = {5709064.026648708, 695689.6964891664}; // Node 26389476
    nodes[61] = {5709060.086602483, 695664.3144744546}; // Node 27481160
    nodes[62] = {5709108.500183443, 695681.3453547781}; // Node 26389479
    nodes[63] = {5709268.312660494, 695658.9518780139}; // Node 7563549239
    nodes[64] = {5709102.061487894, 696366.545222468}; // Node 26389795
    nodes[65] = {5709126.914879193, 696361.6573379814}; // Node 387940757
    nodes[66] = {5708870.950451003, 695776.9741017206}; // Node 860183693
    nodes[67] = {5708687.662264314, 695820.9266493975}; // Node 2180739564
    nodes[68] = {5708671.0841495935, 696400.505558549}; // Node 27089790
    nodes[69] = {5708656.800930689, 696362.5728924433}; // Node 26389318
    nodes[70] = {5708707.409194799, 696386.1866887494}; // Node 27089791
    nodes[71] = {5708960.15488499, 695677.4019872209}; // Node 33709382
    nodes[72] = {5709202.769146369, 695719.806495815}; // Node 860183704
    nodes[73] = {5708849.579932537, 695644.8759800524}; // Node 31022083
    nodes[74] = {5708859.842967963, 695710.3819226052}; // Node 1027237558
    nodes[75] = {5709234.168033434, 696340.9690208059}; // Node 32618385
    nodes[76] = {5709250.378391642, 696424.3990213815}; // Node 6024047911
    nodes[77] = {5709267.913135615, 696373.9538398493}; // Node 387940544
    nodes[78] = {5708749.569936095, 696192.4265331503}; // Node 6280192964
    nodes[79] = {5708758.830306789, 695663.0584611883}; // Node 57774606
    nodes[80] = {5709025.0024342295, 695697.872452572}; // Node 265563775
    nodes[81] = {5708919.289220821, 695716.3821496984}; // Node 265563778
    nodes[82] = {5708909.960474659, 695718.1734398978}; // Node 265563777
    nodes[83] = {5708669.900299487, 695764.5624575751}; // Node 267342172
    nodes[84] = {5708664.01424785, 695727.4099483389}; // Node 265565438
    nodes[85] = {5709119.49292055, 696529.2842655771}; // Node 277474200
    nodes[86] = {5709246.692110399, 696424.9586198049}; // Node 1938018124
    nodes[87] = {5709288.013531757, 696455.1686483962}; // Node 392512622
    nodes[88] = {5709255.443806828, 696471.8573038887}; // Node 1938018125
    nodes[89] = {5709262.445806333, 696470.3701446847}; // Node 392513284
    nodes[90] = {5708685.640292249, 695809.4947390868}; // Node 2180739560
    nodes[91] = {5708694.259334698, 696179.3628700112}; // Node 3925964624
    nodes[92] = {5708759.844700483, 696317.9702929279}; // Node 1960925470
    nodes[93] = {5708765.2185551245, 696317.2829592463}; // Node 1960925507
    nodes[94] = {5708781.792780736, 696329.5211868151}; // Node 3818359076
    nodes[95] = {5708846.21701122, 696484.0509293894}; // Node 3818359116
    nodes[96] = {5708678.254798091, 695822.4084393473}; // Node 2180744124
    nodes[97] = {5708701.954468783, 695907.6933396384}; // Node 2180741853
    nodes[98] = {5708692.981172952, 695909.1724414208}; // Node 2180741854
    nodes[99] = {5708729.894272659, 696086.9082076679}; // Node 2360746811
    nodes[100] = {5708676.451028778, 695811.1488049312}; // Node 2180744123
    nodes[101] = {5709445.871834418, 696094.2428263614}; // Node 3765186615
    nodes[102] = {5709455.41573433, 696092.5468539153}; // Node 3765209084
    nodes[103] = {5708857.333992817, 696487.0402949713}; // Node 3818359130
    nodes[104] = {5708832.5002054395, 696492.7205030674}; // Node 3818359102
    nodes[105] = {5708817.441034672, 696518.2062604495}; // Node 26389320
    nodes[106] = {5708865.878417359, 696510.7367182315}; // Node 3818359137
    nodes[107] = {5708721.14660294, 696088.4623191241}; // Node 5898995362
    nodes[108] = {5709486.415730588, 696084.7753791723}; // Node 7911369886


    // Initialise the adjacency list with the edges
    adjList[0] = {{1, 303.519}, {2, 156.256}, {3, 41.309}, {4, 184.269}};
    adjList[1] = {{21, 6.469}, {22, 158.274}, {0, 303.519}};
    adjList[2] = {{33, 8.134}, {30, 18.088}, {0, 156.256}};
    adjList[3] = {{102, 12.853}, {108, 41.046}};
    adjList[4] = {{5, 33.532}, {0, 184.269}, {6, 53.887}};
    adjList[5] = {{4, 33.532}, {50, 181.396}, {51, 183.129}};
    adjList[6] = {{10, 19.803}, {4, 53.887}};
    adjList[7] = {{8, 11.932}, {9, 87.616}, {10, 299.832}};
    adjList[8] = {{7, 11.932}, {72, 45.248}};
    adjList[9] = {{12, 11.471}, {13, 99.879}};
    adjList[10] = {{6, 19.803}, {40, 28.225}, {7, 299.832}};
    adjList[11] = {{12, 99.828}, {13, 12.184}, {14, 50.876}};
    adjList[12] = {{9, 11.471}, {60, 52.567}, {8, 87.146}};
    adjList[13] = {{11, 12.184}, {56, 21.38}};
    adjList[14] = {{80, 61.58}, {81, 45.668}, {11, 50.876}, {71, 31.192}};
    adjList[15] = {{16, 161.072}, {17, 57.548}};
    adjList[16] = {{34, 55.096}, {72, 12.546}, {7, 44.789}};
    adjList[17] = {{34, 160.516}, {15, 57.548}, {32, 92.211}};
    adjList[18] = {{19, 9.452}, {20, 292.743}};
    adjList[19] = {{18, 9.452}, {21, 19.619}};
    adjList[20] = {{101, 13.449}, {18, 292.743}};
    adjList[21] = {{1, 6.469}, {19, 19.028}};
    adjList[22] = {{76, 31.392}, {87, 36.661}, {1, 158.274}};
    adjList[23] = {{24, 63.576}, {25, 30.539}};
    adjList[24] = {{26, 21.383}, {27, 62.293}, {23, 63.576}};
    adjList[25] = {{95, 46.425}, {23, 30.539}};
    adjList[26] = {{49, 61.768}, {68, 85.51}};
    adjList[27] = {{24, 62.293}};
    adjList[28] = {{29, 41.125}, {30, 35.057}, {31, 72.929}};
    adjList[29] = {{28, 41.125}, {39, 28.433}, {31, 53.581}};
    adjList[30] = {{28, 35.057}, {33, 18.83}};
    adjList[31] = {{29, 53.581}, {28, 72.929}, {40, 62.337}};
    adjList[32] = {{33, 83.249}, {17, 92.211}, {34, 197.262}};
    adjList[33] = {{2, 8.134}, {32, 83.249}};
    adjList[34] = {{16, 55.096}, {32, 197.262}, {17, 160.516}};
    adjList[35] = {{36, 52.921}, {37, 65.778}, {38, 49.173}};
    adjList[36] = {{78, 9.761}, {107, 106.961}, {35, 52.921}};
    adjList[37] = {{92, 73.342}, {35, 65.778}};
    adjList[38] = {{91, 68.171}, {35, 49.173}};
    adjList[39] = {{29, 28.433}};
    adjList[40] = {{31, 62.337}, {6, 33.73}};
    adjList[41] = {{42, 10.796}, {43, 154.774}, {44, 122.668}};
    adjList[42] = {{50, 41.055}, {41, 10.796}, {55, 143.458}};
    adjList[43] = {{41, 154.774}};
    adjList[44] = {{41, 122.668}};
    adjList[45] = {{46, 64.446}, {47, 45.849}};
    adjList[46] = {{90, 66.782}, {74, 187.738}};
    adjList[47] = {{45, 45.849}, {79, 49.829}};
    adjList[48] = {{49, 48.283}};
    adjList[49] = {{26, 61.768}, {23, 24.546}, {48, 48.283}};
    adjList[50] = {{42, 41.055}, {5, 181.396}, {78, 266.825}};
    adjList[51] = {{5, 183.129}, {52, 282.168}, {75, 42.826}};
    adjList[52] = {{53, 43.977}, {54, 50.728}, {51, 282.168}};
    adjList[53] = {{52, 43.977}, {86, 103.918}, {65, 84.102}};
    adjList[54] = {{52, 50.728}, {85, 26.736}, {88, 145.457}};
    adjList[55] = {{42, 143.458}};
    adjList[56] = {{57, 82.659}, {58, 55.22}, {59, 12.015}};
    adjList[57] = {{66, 11.912}, {67, 187.945}};
    adjList[58] = {{56, 55.22}};
    adjList[59] = {{56, 12.015}, {11, 21.481}};
    adjList[60] = {{12, 52.567}, {61, 25.604}, {62, 45.22}};
    adjList[61] = {{60, 25.604}, {71, 100.741}};
    adjList[62] = {{60, 45.22}, {63, 162.404}};
    adjList[63] = {{62, 162.404}};
    adjList[64] = {{65, 25.312}};
    adjList[65] = {{64, 25.312}, {75, 109.154}, {53, 84.102}};
    adjList[66] = {{74, 67.296}, {59, 82.785}, {57, 11.912}};
    adjList[67] = {{96, 9.517}, {97, 87.655}};
    adjList[68] = {{69, 40.412}, {70, 39.009}};
    adjList[69] = {};
    adjList[70] = {{68, 39.009}};
    adjList[71] = {{14, 31.192}, {61, 100.741}};
    adjList[72] = {{16, 12.546}, {15, 160.778}};
    adjList[73] = {{74, 66.093}};
    adjList[74] = {{66, 67.296}, {73, 66.093}, {46, 187.738}};
    adjList[75] = {{65, 109.154}, {76, 84.72}, {77, 65.657}, {51, 42.826}};
    adjList[76] = {{86, 3.726}, {22, 31.392}, {75, 84.72}};
    adjList[77] = {{75, 65.657}};
    adjList[78] = {{36, 9.761}, {50, 266.825}, {94, 140.769}};
    adjList[79] = {{47, 49.829}, {73, 92.489}};
    adjList[80] = {{14, 61.58}};
    adjList[81] = {{82, 9.493}, {14, 45.668}};
    adjList[82] = {{81, 9.493}};
    adjList[83] = {{84, 37.495}};
    adjList[84] = {};
    adjList[85] = {{54, 26.736}};
    adjList[86] = {{53, 103.918}, {76, 3.726}, {88, 47.556}};
    adjList[87] = {{22, 36.661}};
    adjList[88] = {{89, 7.153}, {86, 47.556}, {54, 145.457}};
    adjList[89] = {{88, 7.153}};
    adjList[90] = {{67, 11.572}, {66, 188.014}};
    adjList[91] = {{38, 68.171}};
    adjList[92] = {{93, 5.414}, {69, 112.385}, {37, 73.342}};
    adjList[93] = {{92, 5.414}, {94, 22.298}, {36, 125.363}};
    adjList[94] = {{103, 174.384}};
    adjList[95] = {{103, 12.354}, {93, 185.367}};
    adjList[96] = {{100, 11.367}};
    adjList[97] = {{98, 9.088}, {99, 180.81}};
    adjList[98] = {{96, 87.726}};
    adjList[99] = {{78, 107.001}};
    adjList[100] = {{90, 9.331}, {83, 46.897}};
    adjList[101] = {{0, 40.113}};
    adjList[102] = {{20, 11.386}, {101, 9.717}};
    adjList[103] = {};
    adjList[104] = {{25, 34.092}, {105, 29.532}};
    adjList[105] = {};
    adjList[106] = {{95, 33.122}, {104, 38.258}};
    adjList[107] = {{99, 8.879}, {98, 180.91}};
    adjList[108] = {{102, 31.976}};

    // source and target nodes
    int source = 6;
    int target = 94;

    int iterations = 100;
    std::vector<int> result;

    // start the timer
    //clock_t exec_t1, exec_t2, total_time = 0;

    alt_64 proc_ticks = 0;
    alt_u64 time1 = 0;
    alt_u64 time3 = 0;

    alt_timestamp_start();

    for (int i = 0; i < iterations; ++i) {
        //exec_t1 = times(NULL);
        alt_icache_flush_all();
        alt_dcache_flush_all();

        time1 = alt_timestamp();
        result = findPath(dma, accel, nodes, adjList, source, target);
        time3 = alt_timestamp();
        //exec_t2 = times(NULL);
        //total_time += (exec_t2 - exec_t1);
        proc_ticks += (time3 - time1);
    }

    int k = alt_timestamp_freq() * 1e-6 * iterations;
    double proc_us = (double)proc_ticks / (double)k;

    printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
                iterations, proc_ticks, proc_us, proc_us);

    // Print the final path and the average duration
    std::cout << "Final path: ";
    for (int node : result) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

//    double average_time = (double)total_time / iterations;
//    printf("Average processing time = %f ticks\n", average_time);

    return 0;
}


