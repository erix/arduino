#define myNodeID 30          //node ID of Rx (range 0-30) 
#define network     210      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module

typedef struct { int power, pulse, misc1, misc2; } PayloadTX;      // create structure - a neat way of packaging data for RF comms
PayloadTX emontx;  

const int emonTx_NodeID=10;            //emonTx node ID

