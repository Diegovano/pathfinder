// Number of nodes we're working with
`define DEFAULT_MAX_NODES 16 //number of nodes to support
`define DEFAULT_INDEX_WIDTH 4 //number of bits used to represent a node index

// Width of the edge value
// Max edge value is 2**VALUE_WIDTH-1
`define DEFAULT_VALUE_WIDTH 32 //width of the edge value

// RAM width
`define DEFAULT_MADDR_WIDTH 32
`define DEFAULT_MDATA_WIDTH 16

`define INFINITY 32'h7F800000 //floating point infinite
`define NO_PREVIOUS_NODE ((2**INDEX_WIDTH)-1)

`define UNVISITED 0
`define VISITED 1

