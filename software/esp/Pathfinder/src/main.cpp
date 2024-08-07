#include <Arduino.h>
#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <Graph.h>
#include <Slave.h>
#include <vector>

#define SKIP_RX false
#define WIFI true
#define ADJ_MATX false
#define XY_DONTCARE true

const int SPI_CS = D2; // Slave select is active-low
const int BUF_LEN = 4;

enum State
{
  HOST_LISTEN,
  HOST_RX,
  FPGA_TX,
  FPGA_RX,
  HOST_TX,
  RESET
};

State state;

int state_change = 0;

// WPA2 Personal Authentication

const char *PER_SSID = "Vincenzo's Laptop";
const char *PER_PASSWORD = "0D9}558z";

// Server declaration
WiFiServer server(80);
WiFiClient client;
char request[80000];
unsigned int offset;
std::string response;

// LED states
const int LED = D0;
int LEDState;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 60000;

Slave FPGA(SPI_CS, BUF_LEN);

struct Edge {
    int target;
    float length;
};

bool convertToJson(const Edge& t, JsonVariant variant) {
  char buffer[128];

  JsonObject edge;

  variant.add(t.target);
  variant.add(t.length);
  
  return variant;
}

void setup()
{

  // Serial setup
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Serial started");

#if WIFI
  // Connect to Wi-Fi network with SSID and password
  // WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");

  Serial.print(PER_SSID);
  WiFi.begin(PER_SSID, PER_PASSWORD);

  // Serial.println(EAP_SSID);
  // WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD, ROOT_CA);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
#else
  #if ADJ_MATX
  
  const int SIZE = 9;
  float adjMatx[SIZE][SIZE] = {{0, 1, -1, -1, -1, -1, -1, -1, -1}, {1, 0, 1, -1, -1, -1, -1, -1, -1}, {-1, 1, 0, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, 0, 1, -1, 1, -1, -1}, {-1, -1, -1, 1, 0, 1, -1, -1, -1}, {-1, -1, 1, -1, 1, 0, -1, -1, -1}, {-1, -1, -1, 1, -1, -1, 0, 1, -1}, {-1, -1, -1, -1, -1, -1, 1, 0, 1}, {-1, -1, -1, -1, -1, -1, -1, 1, 0}};
  float x[SIZE] = {9.5, 7.74, 4.51, 2.53, 2.30, 6.07, 2.73, 6.71, 10.88};
  float y[SIZE] = {1.72, 4.59, 4.31, 4.38, 10.46, 6.24, 6.40, 9.03, 6.46};

  #else


  const int SIZE = 219;
  std::vector<std::vector<Edge>> adjList(SIZE);;
  
  // adjList[0]  = {{6, 33.532}, {12, 53.887}};
  // adjList[1]  = {{11, 62.337}, {12, 33.73}};
  // adjList[2]  = {{12, 19.803}, {1, 28.225}};
  // adjList[3]  = {{8, 10.796}, {5, 154.774}, {4, 122.668}};
  // adjList[4]  = {{3, 122.668}};
  // adjList[5]  = {{3, 154.774}};
  // adjList[6]  = {{0, 33.532}, {10, 181.396}, {9, 183.129}};
  // adjList[7]  = {{8, 143.458}};
  // adjList[8]  = {{10, 41.055}, {3, 10.796}, {7, 143.458}};
  // adjList[9]  = {{6, 183.129}};
  // adjList[10] = {{8, 41.055}, {6, 181.396}};
  // adjList[11] = {{1, 62.337}};
  // adjList[12] = {{0, 53.887}, {2, 19.803}};

  // nodes[0]  = {5709222.9834205909, 696108.62011866691}; // Node 276548
  // nodes[1]  = {5709191.8782056486, 696118.27943041269}; // Node 25291708
  // nodes[2]  = {5709211.2912476128, 696055.84373104712}; // Node 25291709
  // nodes[3]  = {5709236.2511749854, 696033.52349726949}; // Node 25473589
  // nodes[4]  = {5709284.5927414894, 695997.60383067257}; // Node 25473590
  // nodes[5]  = {5709208.1398356995, 696036.22818386171}; // Node 25473592
  // nodes[6]  = {5709009.2122519007, 696190.09544731071}; // Node 26389375
  // nodes[7]  = {5709019.8233349705, 696188.06609560957}; // Node 26389434
  // nodes[8]  = {5709006.0310580395, 696323.27681472222}; // Node 26389442
  // nodes[9]  = {5709029.0720731048, 696311.54391213995}; // Node 32618386
  // nodes[10] = {5709012.7329801535, 696147.49481777672}; // Node 34519894
  // nodes[11] = {5709225.9788216352, 696298.79496300314}; // Node 1691189781
  // nodes[12] = {5709058.8068370707, 696308.78188286372}; // Node 3764277424

  // nodes[0] = {5709406.592593853, 696093.7863483471}; // Node 109670
  // nodes[1] = {5709438.015823614, 696396.2634310309}; // Node 12196781
  // nodes[2] = {5709381.557287798, 695939.0986185186}; // Node 3764277440
  // nodes[3] = {5709445.713900904, 696085.5413105368}; // Node 7911351682
  // nodes[4] = {5709222.983420591, 696108.6201186669}; // Node 276548
  // nodes[5] = {5708917.241430879, 696550.950544321}; // Node 109679
  // nodes[6] = {5708910.453599325, 696557.1070687224}; // Node 3818359146
  // nodes[7] = {5708926.926476872, 696560.3823611179}; // Node 1691189628
  // nodes[8] = {5708650.025973048, 695590.3512704588}; // Node 109869
  // nodes[9] = {5708664.519258925, 695679.5507079257}; // Node 26389013
  // nodes[10] = {5708709.68251757, 695671.4716524454}; // Node 265563774
  // nodes[11] = {5708634.410067102, 695552.9268367707}; // Node 109870
  // nodes[12] = {5708643.488061944, 695550.1168012435}; // Node 1356901661
  // nodes[13] = {5708607.986124036, 695562.7297971516}; // Node 27322675
  // nodes[14] = {5708630.143624787, 695527.7219922208}; // Node 5898890847
  // nodes[15] = {5709191.878205649, 696118.2794304127}; // Node 26389375
  // nodes[16] = {5709211.291247613, 696055.8437310471}; // Node 3764277424
  // nodes[17] = {5709160.448641967, 695739.23620667}; // Node 276549
  // nodes[18] = {5709158.147415086, 695727.4894879166}; // Node 860183706
  // nodes[19] = {5709073.889511777, 695753.1787173111}; // Node 276551
  // nodes[20] = {5709208.139835699, 696036.2281838617}; // Node 25291709
  // nodes[21] = {5708973.6509467, 695758.5931036496}; // Node 276550
  // nodes[22] = {5709072.126067131, 695741.8072089367}; // Node 860183709
  // nodes[23] = {5708975.4864559425, 695770.6779056357}; // Node 3960600156
  // nodes[24] = {5708964.289938448, 695708.420401278}; // Node 265563776
  // nodes[25] = {5708520.635064215, 695847.0795237639}; // Node 276556
  // nodes[26] = {5708517.582765468, 695836.3611816618}; // Node 860183705
  // nodes[27] = {5708536.110423783, 695936.2222455945}; // Node 25291780
  // nodes[28] = {5708534.235304299, 696523.0984327237}; // Node 276557
  // nodes[29] = {5708632.320982646, 696528.8040949589}; // Node 277313428
  // nodes[30] = {5708512.032146047, 696517.3405482845}; // Node 26389340
  // nodes[31] = {5708527.729286418, 696566.4586224909}; // Node 276558
  // nodes[32] = {5708710.425563906, 696698.8763464226}; // Node 276559
  // nodes[33] = {5709361.046023356, 695698.5969165337}; // Node 12195919
  // nodes[34] = {5709204.718285029, 695732.2408049735}; // Node 29353839
  // nodes[35] = {5709371.905924698, 695755.2959538447}; // Node 385312258
  // nodes[36] = {5709356.1536479015, 695615.565522306}; // Node 26389508
  // nodes[37] = {5708610.679631182, 696213.626050669}; // Node 12196057
  // nodes[38] = {5708608.854152233, 696207.8789561834}; // Node 3925964632
  // nodes[39] = {5708637.341913043, 696294.4384493381}; // Node 26389299
  // nodes[40] = {5709469.453153156, 696397.2155050545}; // Node 12196664
  // nodes[41] = {5709460.002069923, 696397.567223864}; // Node 1973883128
  // nodes[42] = {5709454.374156674, 696103.9215622969}; // Node 4322519851
  // nodes[43] = {5709444.412417221, 696397.259933658}; // Node 1973882968
  // nodes[44] = {5709281.325204759, 696419.0041420648}; // Node 392512612
  // nodes[45] = {5709432.894799637, 696527.5009969231}; // Node 20947335
  // nodes[46] = {5708769.674973994, 696483.461031195}; // Node 14457894
  // nodes[47] = {5708708.673839894, 696500.9108534758}; // Node 14457897
  // nodes[48] = {5708800.075679975, 696482.0831574204}; // Node 1960925514
  // nodes[49] = {5708701.434384973, 696480.721817096}; // Node 27089782
  // nodes[50] = {5708650.455622036, 696523.2299281717}; // Node 1960954870
  // nodes[51] = {5709332.6284358045, 695952.0228566583}; // Node 20821126
  // nodes[52] = {5709338.004398434, 695992.9288725917}; // Node 1691189790
  // nodes[53] = {5709363.779561127, 695937.1995158533}; // Node 1691189798
  // nodes[54] = {5709284.592741489, 695997.6038306726}; // Node 1691189781
  // nodes[55] = {5709379.090655007, 695847.4676591664}; // Node 20947178
  // nodes[56] = {5709381.120316871, 695930.9498430908}; // Node 3764277439
  // nodes[57] = {5709215.018426711, 695786.5446752683}; // Node 29353841
  // nodes[58] = {5709330.491868323, 696538.8524985197}; // Node 306463107
  // nodes[59] = {5709434.092383795, 696554.6966499089}; // Node 306463030
  // nodes[60] = {5708687.444765689, 696200.9779529992}; // Node 20963439
  // nodes[61] = {5708739.947435228, 696194.1049815264}; // Node 6280192965
  // nodes[62] = {5708715.397339266, 696260.7281027433}; // Node 1668506746
  // nodes[63] = {5708659.3383828215, 696160.5763704968}; // Node 1360237602
  // nodes[64] = {5708641.134976796, 696527.2396858311}; // Node 21105283
  // nodes[65] = {5708673.188851461, 696542.3954772014}; // Node 96087926
  // nodes[66] = {5709340.304641033, 696021.3610122055}; // Node 25291706
  // nodes[67] = {5709236.251174985, 696033.5234972695}; // Node 25291708
  // nodes[68] = {5708611.842346598, 696115.4531074245}; // Node 25291778
  // nodes[69] = {5708721.14660294, 696088.4623191241}; // Node 5898995362
  // nodes[70] = {5708605.693762278, 696066.3763445374}; // Node 300425806
  // nodes[71] = {5708567.570969171, 696119.5121342868}; // Node 7902276163
  // nodes[72] = {5708587.471864035, 695926.8938194645}; // Node 300425785
  // nodes[73] = {5708550.090691499, 696019.1875813099}; // Node 26389361
  // nodes[74] = {5709009.212251901, 696190.0954473107}; // Node 25473589
  // nodes[75] = {5709019.8233349705, 696188.0660956096}; // Node 26389442
  // nodes[76] = {5709006.0310580395, 696323.2768147222}; // Node 25473592
  // nodes[77] = {5709029.072073105, 696311.54391214}; // Node 25473590
  // nodes[78] = {5708478.380504076, 695689.1351870648}; // Node 26389001
  // nodes[79] = {5708646.639834227, 695622.696962693}; // Node 265565434
  // nodes[80] = {5708495.618407274, 695753.8835864722}; // Node 26389051
  // nodes[81] = {5708548.217587023, 695585.9387974386}; // Node 26389004
  // nodes[82] = {5708674.89556865, 695743.3657228529}; // Node 1116949690
  // nodes[83] = {5708734.9125308655, 695520.8244467379}; // Node 26389016
  // nodes[84] = {5708758.830306789, 695663.0584611883}; // Node 57774606
  // nodes[85] = {5708819.999035907, 695499.5681548247}; // Node 1691189553
  // nodes[86] = {5708506.659011818, 695795.3426253834}; // Node 267342171
  // nodes[87] = {5708578.374259219, 696318.4176738518}; // Node 26389300
  // nodes[88] = {5708571.19105677, 696321.267102961}; // Node 1260002931
  // nodes[89] = {5708601.94124451, 696385.3722649945}; // Node 26389317
  // nodes[90] = {5708642.536580914, 696500.7315825905}; // Node 27089783
  // nodes[91] = {5708656.800930689, 696362.5728924433}; // Node 26389318
  // nodes[92] = {5708882.216333071, 696583.8812959035}; // Node 26389319
  // nodes[93] = {5708938.302181703, 696641.0853127539}; // Node 26389334
  // nodes[94] = {5708883.988785284, 696582.2071685544}; // Node 8043756354
  // nodes[95] = {5708847.8345529055, 696615.5921351353}; // Node 1960954879
  // nodes[96] = {5708817.441034672, 696518.2062604495}; // Node 26389320
  // nodes[97] = {5708736.246596246, 696569.4525747373}; // Node 26389322
  // nodes[98] = {5708796.9587542815, 696596.0298202524}; // Node 26389333
  // nodes[99] = {5708843.47813683, 696616.3863455729}; // Node 1960954878
  // nodes[100] = {5708964.794050954, 696611.5151270464}; // Node 5904372084
  // nodes[101] = {5708527.489247364, 696329.9767791595}; // Node 8091273206
  // nodes[102] = {5708527.7728225095, 696327.5191702817}; // Node 26389341
  // nodes[103] = {5708510.806158558, 696168.2531600029}; // Node 5146303884
  // nodes[104] = {5708615.183122235, 696672.5805469541}; // Node 26389359
  // nodes[105] = {5708626.995762582, 696659.8899830694}; // Node 302397746
  // nodes[106] = {5708608.304064068, 696681.841089166}; // Node 302397749
  // nodes[107] = {5708510.710240445, 696614.3686282736}; // Node 31200011
  // nodes[108] = {5708553.040368361, 696036.179205901}; // Node 5146341249
  // nodes[109] = {5708484.628014958, 696065.8931544174}; // Node 4147738097
  // nodes[110] = {5708741.458335954, 696416.0911551546}; // Node 26389363
  // nodes[111] = {5708759.969653688, 696460.8376846968}; // Node 27089784
  // nodes[112] = {5709012.7329801535, 696147.4948177767}; // Node 34519894
  // nodes[113] = {5709225.978821635, 696298.7949630031}; // Node 32618386
  // nodes[114] = {5709101.5239167195, 696453.6833576608}; // Node 26389381
  // nodes[115] = {5709144.493144636, 696444.1743869598}; // Node 387940759
  // nodes[116] = {5709113.294114814, 696503.1909305681}; // Node 392513282
  // nodes[117] = {5709058.806837071, 696308.7818828637}; // Node 26389434
  // nodes[118] = {5708954.459312898, 695774.6022412439}; // Node 26389449
  // nodes[119] = {5708872.963502869, 695788.7534463173}; // Node 26966492
  // nodes[120] = {5708963.282568445, 695829.2923988366}; // Node 26389456
  // nodes[121] = {5708952.55292382, 695762.7008228918}; // Node 3960591923
  // nodes[122] = {5709064.026648708, 695689.6964891664}; // Node 26389476
  // nodes[123] = {5709060.086602483, 695664.3144744546}; // Node 27481160
  // nodes[124] = {5709108.500183443, 695681.3453547781}; // Node 26389479
  // nodes[125] = {5709268.312660494, 695658.9518780139}; // Node 7563549239
  // nodes[126] = {5709358.472583398, 695545.9815490542}; // Node 26389510
  // nodes[127] = {5709128.568112231, 695627.6771174066}; // Node 26389509
  // nodes[128] = {5709271.770312227, 695536.8464980423}; // Node 391600713
  // nodes[129] = {5709262.434939293, 695535.8647651642}; // Node 26389511
  // nodes[130] = {5709102.061487894, 696366.545222468}; // Node 26389795
  // nodes[131] = {5709126.914879193, 696361.6573379814}; // Node 387940757
  // nodes[132] = {5708999.584588975, 696647.0104579013}; // Node 26389815
  // nodes[133] = {5708969.269596665, 696683.0881341798}; // Node 864960555
  // nodes[134] = {5709303.877965906, 696539.1043962796}; // Node 26389819
  // nodes[135] = {5709298.092474268, 696538.3623540635}; // Node 6010588330
  // nodes[136] = {5709149.265494175, 696536.992955224}; // Node 26389821
  // nodes[137] = {5709122.686454102, 696542.747740373}; // Node 387840694
  // nodes[138] = {5709267.4925242225, 696524.8135011581}; // Node 392512699
  // nodes[139] = {5708870.950451003, 695776.9741017206}; // Node 860183693
  // nodes[140] = {5708687.662264314, 695820.9266493975}; // Node 2180739564
  // nodes[141] = {5708671.0841495935, 696400.505558549}; // Node 27089790
  // nodes[142] = {5708707.409194799, 696386.1866887494}; // Node 27089791
  // nodes[143] = {5709034.867088941, 695501.880192413}; // Node 80159855
  // nodes[144] = {5708960.15488499, 695677.4019872209}; // Node 33709382
  // nodes[145] = {5709202.769146369, 695719.806495815}; // Node 860183704
  // nodes[146] = {5708849.579932537, 695644.8759800524}; // Node 31022083
  // nodes[147] = {5708904.834475318, 695635.1436454172}; // Node 80170877
  // nodes[148] = {5708859.842967963, 695710.3819226052}; // Node 1027237558
  // nodes[149] = {5708825.791795577, 695498.1914009855}; // Node 1126210963
  // nodes[150] = {5708507.232996705, 696623.4905947476}; // Node 31200012
  // nodes[151] = {5709234.168033434, 696340.9690208059}; // Node 32618385
  // nodes[152] = {5709250.378391642, 696424.3990213815}; // Node 6024047911
  // nodes[153] = {5709267.913135615, 696373.9538398493}; // Node 387940544
  // nodes[154] = {5708949.105438691, 695626.4462295353}; // Node 8154276788
  // nodes[155] = {5708954.355887827, 695625.4446925927}; // Node 33709383
  // nodes[156] = {5708749.569936095, 696192.4265331503}; // Node 6280192964
  // nodes[157] = {5708934.216732732, 695513.9496612463}; // Node 8154276789
  // nodes[158] = {5708934.723495724, 695517.8569158887}; // Node 80161627
  // nodes[159] = {5708919.289220821, 695716.3821496984}; // Node 265563778
  // nodes[160] = {5708933.957758368, 695513.0074786718}; // Node 8154276790
  // nodes[161] = {5708829.38702754, 696639.8263454554}; // Node 96086041
  // nodes[162] = {5708556.955947408, 696059.1527639969}; // Node 255697000
  // nodes[163] = {5709025.0024342295, 695697.872452572}; // Node 265563775
  // nodes[164] = {5708909.960474659, 695718.1734398978}; // Node 265563777
  // nodes[165] = {5708664.01424785, 695727.4099483389}; // Node 265565438
  // nodes[166] = {5708669.900299487, 695764.5624575751}; // Node 267342172
  // nodes[167] = {5708621.773391426, 696557.5012366002}; // Node 277313430
  // nodes[168] = {5708594.133614355, 696557.1392361086}; // Node 277313434
  // nodes[169] = {5708645.812303931, 696592.8458114329}; // Node 277313432
  // nodes[170] = {5709057.698508203, 696551.8380952755}; // Node 277474199
  // nodes[171] = {5709119.49292055, 696529.2842655771}; // Node 277474200
  // nodes[172] = {5708580.129819654, 695882.6239476504}; // Node 300425790
  // nodes[173] = {5708573.469034644, 695838.9741170865}; // Node 300425789
  // nodes[174] = {5708571.768510573, 695827.7936752201}; // Node 860183708
  // nodes[175] = {5708641.137619533, 695872.5209013498}; // Node 300425791
  // nodes[176] = {5708560.786595084, 695885.7471743808}; // Node 300425792
  // nodes[177] = {5708603.051936091, 696047.2670020652}; // Node 300425805
  // nodes[178] = {5708649.805871051, 696057.3265233573}; // Node 3417174536
  // nodes[179] = {5708657.892746568, 696055.6728602523}; // Node 300425807
  // nodes[180] = {5709341.775103858, 696564.2543953973}; // Node 6010588340
  // nodes[181] = {5709331.43412892, 696556.8642209544}; // Node 864960601
  // nodes[182] = {5709383.0494744405, 696591.2121612416}; // Node 306464184
  // nodes[183] = {5709140.885743999, 696578.5371509531}; // Node 387840814
  // nodes[184] = {5709246.692110399, 696424.9586198049}; // Node 1938018124
  // nodes[185] = {5709260.893530161, 695480.8000183472}; // Node 391600770
  // nodes[186] = {5709288.013531757, 696455.1686483962}; // Node 392512622
  // nodes[187] = {5709273.940197998, 696490.0169347443}; // Node 392512739
  // nodes[188] = {5709255.443806828, 696471.8573038887}; // Node 1938018125
  // nodes[189] = {5709262.445806333, 696470.3701446847}; // Node 392513284
  // nodes[190] = {5708676.451028778, 695811.1488049312}; // Node 2180744123
  // nodes[191] = {5708685.640292249, 695809.4947390868}; // Node 2180739560
  // nodes[192] = {5708694.259334698, 696179.3628700112}; // Node 3925964624
  // nodes[193] = {5708759.844700483, 696317.9702929279}; // Node 1960925470
  // nodes[194] = {5709054.7177141635, 696463.5980680105}; // Node 6436441932
  // nodes[195] = {5708949.411030373, 696582.3223848451}; // Node 1691189629
  // nodes[196] = {5708941.730550087, 696588.4300784941}; // Node 5904372082
  // nodes[197] = {5708971.54789427, 696603.9073201672}; // Node 1960951885
  // nodes[198] = {5709073.323720351, 696488.620850028}; // Node 1691189659
  // nodes[199] = {5708765.2185551245, 696317.2829592463}; // Node 1960925507
  // nodes[200] = {5708781.792780736, 696329.5211868151}; // Node 3818359076
  // nodes[201] = {5708846.21701122, 696484.0509293894}; // Node 3818359116
  // nodes[202] = {5709044.390385588, 696680.9010963811}; // Node 4897234610
  // nodes[203] = {5708589.033624092, 696030.5499110745}; // Node 2152475698
  // nodes[204] = {5708592.72531686, 696018.5914390611}; // Node 2152475756
  // nodes[205] = {5708593.605922426, 696036.788784133}; // Node 2152475762
  // nodes[206] = {5708678.254798091, 695822.4084393473}; // Node 2180744124
  // nodes[207] = {5708701.954468783, 695907.6933396384}; // Node 2180741853
  // nodes[208] = {5708692.981172952, 695909.1724414208}; // Node 2180741854
  // nodes[209] = {5708729.894272659, 696086.9082076679}; // Node 2360746811
  // nodes[210] = {5708646.40147123, 696040.6725091562}; // Node 3417174537
  // nodes[211] = {5709445.871834418, 696094.2428263614}; // Node 3765186615
  // nodes[212] = {5709455.41573433, 696092.5468539153}; // Node 3765209084
  // nodes[213] = {5708857.333992817, 696487.0402949713}; // Node 3818359130
  // nodes[214] = {5708832.5002054395, 696492.7205030674}; // Node 3818359102
  // nodes[215] = {5708865.878417359, 696510.7367182315}; // Node 3818359137
  // nodes[216] = {5708514.557286237, 696131.8951583181}; // Node 5146350397
  // nodes[217] = {5708561.03866724, 696121.3421995144}; // Node 7902276164
  // nodes[218] = {5709486.415730588, 696084.7753791723}; // Node 7911369886


  // Initialise the adjacency list with the edges
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

  // std::vector<float> x = {5709222.9834205909, 5709191.8782056486, 5709211.2912476128, 5709236.2511749854, 5709284.5927414894, 5709208.1398356995, 5709009.2122519007, 5709019.8233349705, 5709006.0310580395, 5709029.0720731048, 5709012.7329801535, 5709225.9788216352, 5709058.8068370707};
  // std::vector<float> y = {696108.62011866691, 696118.27943041269, 696055.84373104712, 696033.52349726949, 695997.60383067257, 696036.22818386171, 696190.09544731071, 696188.06609560957, 696323.27681472222, 696311.54391213995, 696147.49481777672, 696298.79496300314, 696308.78188286372};
  
  #endif

  int start = 73, end = 113;
  int averageOver = 1000; // number of pathfinding tasks to perform to determine average time required
  std::string message;

  JsonDocument doc;

  #if ADJ_MATX
  JsonArray parent = doc["adjMatx"].to<JsonArray>();
  #else
  JsonArray parent = doc["adjList"].to<JsonArray>();
  #endif
  JsonArray xArr = doc["x"].to<JsonArray>();
  JsonArray yArr = doc["y"].to<JsonArray>();

  #if !XY_DONTCARE
  if (SIZE != y.size()) throw "Size mismatch";
  #endif

  #if ADJ_MATX
  if (SIZE != adjMatx.size() && SIZE != adjMatx.at(0).size()) throw "Size Mismatch";
  #endif // no error case for adjList, empty list means no connections
  
  for (int i = 0; i < SIZE; i++)
  {
    #if XY_DONTCARE
    xArr.add(0);
    yArr.add(0);
    #else
    xArr.add(x.at(i));
    yArr.add(y.at(i));
    #endif
    JsonArray child = parent.add<JsonArray>();
    #if ADJ_MATX
    for (int j = 0; j < SIZE; j++)
    {
      child.add(adjMatx[i][j]);
    }
    #else
    for (int j = 0; j < adjList.at(i).size(); j++)
    {
      child.add(adjList.at(i).at(j));
    }
    #endif
  }

  doc["size"] = SIZE;
  doc["start"] = start;
  doc["end"] = end;
  doc["avgCount"] = averageOver;

  // for (int i = 0; i < BUF_LEN; i++)
  // {
  //   // TX_BUF[i] = RX_BUF[i] = '\0';
  //   // TX_BUF[i] = RX_BUF[i] = i % 26 + 'a';
  //   TX_BUF[i] = i % 26 + 'a';
  // }

  // TX_BUF[511] = '\0';

  serializeJson(doc, message);
#endif

  // Begin FPGA SPI Slave
  FPGA.begin();

  // Initiate LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  LEDState = LOW;

  offset = 0;

  // Initiate state
  #if WIFI
  state = HOST_LISTEN;
  state_change = 1;
  #else
  state = FPGA_TX;
  request = message;
  // Serial.println(message.c_str());
  #endif
}

void loop()
{

  switch (state)
  {
  case HOST_LISTEN:
  {
    #if !WIFI
    state = FPGA_TX;
    break;
    #endif
    if (state_change)
    {
      Serial.println("HOST_LISTEN");
      state_change = 0;
    }
    client = server.available(); // Listen for incoming clients

    if (client)
    {                                  // If a new client connects,
      Serial.print("New Client: IP "); // print a message out in the serial port
      Serial.print(client.localIP());
      Serial.print(" PORT ");
      Serial.println(client.localPort());
      state = HOST_RX;
      state_change = 1;
    }
    break;
  }

  case HOST_RX:
  {
    if (state_change)
    {
      Serial.println("HOST_RX");
      state_change = 0;
    }
    currentTime = millis();
    previousTime = currentTime;

    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected

      currentTime = millis();
      if (client.available())
      { // if there's bytes to read from the client,

        char c = client.read(); // read a byte, then
        //Serial.write(c);        // print it out the serial monitor
        request[offset] = c;
        offset++;
        if (c == '\n')
        {
          // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            request[offset] = '\0';
            state = FPGA_TX;
            state_change = 1;
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    break;
  }

  case FPGA_TX:
  {
    if (state_change)
    {
      Serial.println("FPGA_TX");
      state_change = 0;
    }
    
    FPGA.spi_tx_string(request, offset+1);
    //Serial.println(request);
    #if SKIP_RX
    state = RESET;
    #else
    state = FPGA_RX;
    state_change = 1;
    // state = FPGA_TX;
    #endif
    break;
  }

  case FPGA_RX:
  {
        if (state_change)
    {
      Serial.println("FPGA_RX");
      state_change = 0;
    }
    
    FPGA.spi_rx_string(response);
    #if WIFI
    state = HOST_TX;
    //Serial.println(response.c_str());
    #else
    state = FPGA_TX;
    #endif
    break;
  }

  case HOST_TX:
  {
    Serial.println("HOST_TX");
    client.println(response.c_str());
    Serial.println("SUCCESSFUL EXCHANGE! moving to RESET\n");
    state = RESET;
    break;
  }

  case RESET:
  {
    Serial.println("RESET");
    // clear request and response
    offset = 0;
    response = "";

    // Close the connection
    client.stop();
    Serial.println("Client disconnected, moving to HOST_LISTEN");
    state = HOST_LISTEN;
    state_change = 1;
    break;
  }

  default:
    Serial.println("default");
    Serial.println("ERROR! moving to RESET");
    state = RESET;
    state_change = 1;
  }
}