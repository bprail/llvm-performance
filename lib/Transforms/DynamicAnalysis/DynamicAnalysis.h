//=-------------------- llvm/Support/DynamicAnalysis.h ------======= -*- C++ -*//
//
//                     The LLVM Compiler Infrastructure
//
//  Victoria Caparros Cabezas <caparrov@inf.ethz.ch>
//===----------------------------------------------------------------------===//


#ifndef LLVM_SUPPORT_DYNAMIC_ANALYSIS_H
#define LLVM_SUPPORT_DYNAMIC_ANALYSIS_H


#include "../../../lib/ExecutionEngine/Interpreter/Interpreter.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"
#include "llvm/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallString.h"
//#include "llvm/Support/top-down-size-splay.hpp"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"

//#define INTERPRETER

#ifdef INTERPRETER
#include "llvm/Support/top-down-size-splay.hpp"
#else
#include "top-down-size-splay.hpp"
#endif

#define INT_FP_OPS

#include <iostream>
#include <map>
#include <list>
#include <stdarg.h>
#include <stdio.h>

#ifdef SOURCE_CODE_ANALYSIS
#include <unordered_map>
#endif
#include <deque>
#define ROUND_REUSE_DISTANCE
#define REDUCED_INST_TYPES
#define NORMAL_REUSE_DISTRIBUTION


//#define DEBUG_SOURCE_CODE_LINE_ANALYSIS
//#define DEBUG_MEMORY_TRACES
//#define DEBUG_REUSE_DISTANCE
//#define DEBUG_GENERIC
//#define DEBUG_DEPS_FUNCTION_CALL
//#define DEBUG_SPAN_CALCULATION
//#define DEBUG_AGU
//#define DEBUG_OOO_BUFFERS
//#define DEBUG_ISSUE_CYCLE
//#define DEBUG_PHI_NODE
//#define DEBUG_FUNCTION_CALL_STACK
//#define DEBUG_PREFETCHER

//#define ILP_DISTRIBUTION
//#define MICROSCHEDULING

#define ASSERT

#define MOO_BUFFERS

/*The YMM registers are aliased over the older 128-bit XMM registers used for
 Intel SSE, treating the XMM registers as the lower half of the corresponding
 YMM register.
 */

#define INF -1



// Can be further broken down into FPMults, FPAdds, etc..
#ifdef REDUCED_INST_TYPES
#define N_INST_TYPES 2
#else
#define N_INST_TYPES 37
#endif

#ifdef REDUCED_INST_TYPES

#ifdef INT_FP_OPS
#define INT_ADD          0
#define INT_SUB          0
#define INT_MUL          0
#define INT_DIV          0
#else
#define INT_ADD          -1
#define INT_SUB          -1
#define INT_MUL          -1
#define INT_DIV          -1
#endif

#define INT_REM         -1
#ifdef INT_FP_OPS
#define INT_LD_4_BITS    1
#define INT_LD_8_BITS    1
#define INT_LD_16_BITS   1
#define INT_LD_32_BITS   1
#define INT_LD_64_BITS   1
#define INT_LD_80_BITS   1
#define INT_LD_128_BITS  1
#define INT_ST_4_BITS    1
#define INT_ST_8_BITS    1
#define INT_ST_16_BITS   1
#define INT_ST_32_BITS   1
#define INT_ST_64_BITS   1
#define INT_ST_80_BITS   1
#define INT_ST_128_BITS  1

#else
#define INT_LD_4_BITS    -1
#define INT_LD_8_BITS    -1
#define INT_LD_16_BITS   -1
#define INT_LD_32_BITS   -1
#define INT_LD_64_BITS   -1
#define INT_LD_80_BITS   -1
#define INT_LD_128_BITS  -1
#define INT_ST_4_BITS    -1
#define INT_ST_8_BITS    -1
#define INT_ST_16_BITS   -1
#define INT_ST_32_BITS   -1
#define INT_ST_64_BITS   -1
#define INT_ST_80_BITS   -1
#define INT_ST_128_BITS  -1

#endif

#define FP_ADD           0
#define FP_SUB           0
#define FP_MUL           0
#define FP_DIV           0
#define FP_REM          -1
#define FP_LD_16_BITS    1
#define FP_LD_32_BITS    1
#define FP_LD_64_BITS    1
#define FP_LD_80_BITS    1
#define FP_LD_128_BITS   1
#define FP_ST_16_BITS    1
#define FP_ST_32_BITS    1
#define FP_ST_64_BITS    1
#define FP_ST_80_BITS    1
#define FP_ST_128_BITS   1
#define MISC_MEM        -1
#define CTRL            -1
#define VECTOR_SHUFFLE   0
#define MISC            -1
#else
#define INT_ADD         0
#define INT_SUB         1
#define INT_MUL         2
#define INT_DIV         3
#define INT_REM         4
#define INT_LD_4_BITS   5
#define INT_LD_8_BITS   6
#define INT_LD_16_BITS  7
#define INT_LD_32_BITS  8
#define INT_LD_64_BITS  9
#define INT_LD_80_BITS  10
#define INT_LD_128_BITS 11
#define INT_ST_4_BITS   12
#define INT_ST_8_BITS   13
#define INT_ST_16_BITS  14
#define INT_ST_32_BITS  15
#define INT_ST_64_BITS  16
#define INT_ST_80_BITS  17
#define INT_ST_128_BITS 18
#define FP_ADD          19
#define FP_SUB          20
#define FP_MUL          21
#define FP_DIV          22
#define FP_REM          23
#define FP_LD_16_BITS   24
#define FP_LD_32_BITS   25
#define FP_LD_64_BITS   26
#define FP_LD_80_BITS   27
#define FP_LD_128_BITS  28
#define FP_ST_16_BITS   29
#define FP_ST_32_BITS   30
#define FP_ST_64_BITS   31
#define FP_ST_80_BITS   32
#define FP_ST_128_BITS  33
#define MISC_MEM        34
#define CTRL            35
#define MISC            36
#endif






// =================== Atom config ================================//


#define ATOM_EXECUTION_UNITS 6
#define ATOM_DISPATCH_PORTS 2
#define ATOM_BUFFERS 5
#define ATOM_AGUS 0
#define ATOM_LOAD_AGUS 0
#define ATOM_STORE_AGUS 0


#define ATOM_N_COMP_EXECUTION_UNITS 4
#define ATOM_N_MEM_EXECUTION_UNITS 5
#define ATOM_N_AGUS 2

#define ATOM_FP_ADDER  0
#define ATOM_FP_MULTIPLIER 1
#define ATOM_FP_DIVIDER  2
#define ATOM_FP_SHUFFLE  3
#define ATOM_L1_LOAD_CHANNEL 4
#define ATOM_L1_STORE_CHANNEL  5
#define ATOM_L2_LOAD_CHANNEL 6
#define ATOM_L2_STORE_CHANNEL  7
#define ATOM_L3_LOAD_CHANNEL 8
#define ATOM_L3_STORE_CHANNEL  9
#define ATOM_MEM_LOAD_CHANNEL 10
#define ATOM_MEM_STORE_CHANNEL  11
#define ATOM_ADDRESS_GENERATION_UNIT 12
#define ATOM_STORE_ADDRESS_GENERATION_UNIT 13
#define ATOM_LOAD_ADDRESS_GENERATION_UNIT 14



// =================== Generic config ================================//



#define GENERIC_AGUS 2
#define GENERIC_LOAD_AGUS 0
#define GENERIC_STORE_AGUS 1


// Define execution units
#define GENERIC_N_MEM_EXECUTION_UNITS 5
#define GENERIC_N_COMP_EXECUTION_UNITS 4
#define GENERIC_N_AGUS 2

#define GENERIC_FP_ADDER  0
#define GENERIC_FP_MULTIPLIER 1
#define GENERIC_FP_DIVIDER  2
#define GENERIC_FP_SHUFFLE  3
#define GENERIC_L1_LOAD_CHANNEL 4
#define GENERIC_L1_STORE_CHANNEL  5
#define GENERIC_L2_LOAD_CHANNEL 6
#define GENERIC_L2_STORE_CHANNEL  7
#define GENERIC_L3_LOAD_CHANNEL 8
#define GENERIC_L3_STORE_CHANNEL  9
#define GENERIC_MEM_LOAD_CHANNEL 10
#define GENERIC_MEM_STORE_CHANNEL  11
#define GENERIC_ADDRESS_GENERATION_UNIT 12
#define GENERIC_STORE_ADDRESS_GENERATION_UNIT 13
#define GENERIC_LOAD_ADDRESS_GENERATION_UNIT 14




// =================== Sandy Bridge config ================================//


// These are execution units, o resources -> there is an available and
// and full occupancy tree from them
#ifdef INT_FP_OPS
#define SANDY_BRIDGE_EXECUTION_UNITS 13 //9
#define SANDY_BRIDGE_NODES 28 //23
#define SANDY_BRIDGE_COMP_EXECUTION_UNITS 8 //4
#else
#define SANDY_BRIDGE_EXECUTION_UNITS 9
#define SANDY_BRIDGE_NODES 23
#define SANDY_BRIDGE_COMP_EXECUTION_UNITS 4
#endif

#define SANDY_BRIDGE_DISPATCH_PORTS 5
#define SANDY_BRIDGE_BUFFERS 5
#define SANDY_BRIDGE_AGUS 2
#define SANDY_BRIDGE_LOAD_AGUS 0
#define SANDY_BRIDGE_STORE_AGUS 0
#define SANDY_BRIDGE_PREFETCH_NODES 3

#define SANDY_BRIDGE_MEM_EXECUTION_UNITS 5
#define SANDY_BRIDGE_AGU 1


#ifdef INT_FP_OPS
#define INT_ADDER 0
#define INT_MULTIPLIER 1
#define INT_DIVIDER 2
#define INT_SHUFFLE 3
#define FP_ADDER  4 //0
#define FP_MULTIPLIER 5 //1
#define FP_DIVIDER  6 //2
#define FP_SHUFFLE  7 //3
#define L1_LOAD_CHANNEL 8 //4
#define L1_STORE_CHANNEL  9 //5
#define L2_LOAD_CHANNEL 10 //6
#define L2_STORE_CHANNEL  10 //6
#define L3_LOAD_CHANNEL 11 //7
#define L3_STORE_CHANNEL  11 //7
#define MEM_LOAD_CHANNEL 12 //8
#define MEM_STORE_CHANNEL  12 //8
#define ADDRESS_GENERATION_UNIT 13 //9
#define STORE_ADDRESS_GENERATION_UNIT -1
#define LOAD_ADDRESS_GENERATION_UNIT -1
#define PORT_0  14 //10
#define PORT_1  15 //11
#define PORT_2  16 //12
#define PORT_3  17 //13
#define PORT_4  18 //14
#define RS_STALL  19 //15
#define ROB_STALL 20 //16
#define LB_STALL 21 //17
#define SB_STALL 22 //18
#define LFB_STALL 23 //19

#else
#define FP_ADDER  0
#define FP_MULTIPLIER 1
#define FP_DIVIDER  2
#define FP_SHUFFLE  3
#define L1_LOAD_CHANNEL 4
#define L1_STORE_CHANNEL  5
#define L2_LOAD_CHANNEL 6
#define L2_STORE_CHANNEL  6
#define L3_LOAD_CHANNEL 7
#define L3_STORE_CHANNEL  7
#define MEM_LOAD_CHANNEL 8
#define MEM_STORE_CHANNEL  8
#define ADDRESS_GENERATION_UNIT 9
#define STORE_ADDRESS_GENERATION_UNIT -1
#define LOAD_ADDRESS_GENERATION_UNIT -1
#define PORT_0  10
#define PORT_1  11
#define PORT_2  12
#define PORT_3  13
#define PORT_4  14
#define RS_STALL  15
#define ROB_STALL 16
#define LB_STALL 17
#define SB_STALL 18
#define LFB_STALL 19

#endif



// Nodes for SANDY BRIDGE. Set to -1 those that
// do not exists

#ifdef INT_FP_OPS
#define SANDY_BRIDGE_COMP_NODES 8 //4
#define N_COMP_NODES_START 0
#define N_COMP_NODES_END 7 //3

#define INT_ADD_NODE      0
#define INT_MUL_NODE      1
#define INT_DIV_NODE      2
#define INT_SHUFFLE_NODE 3
#define FP_ADD_NODE      4 //0
#define FP_MUL_NODE      5 //1
#define FP_DIV_NODE      6 //2
#define VECTOR_SHUFFLE_NODE 7 //3


#define SANDY_BRIDGE_MEM_NODES 8
#define L1_LOAD_NODE     8 //4
#define L1_STORE_NODE      9 //5
#define L2_LOAD_NODE     10 //6
#define L2_STORE_NODE      11 //7
#define L3_LOAD_NODE     12 //8
#define L3_STORE_NODE      13 //9
#define MEM_LOAD_NODE    14 //10
#define MEM_STORE_NODE     15 //11

#define N_MEM_NODES_START 8 //4
#define N_MEM_NODES_END 15 //11


#define N_MISC_RESOURCES 7
#define N_AGU_NODES     1
#define N_PORT_NODES  5
#define AGU_NODE           16 //12
#define STORE_AGU_NODE     -1
#define LOAD_AGU_NODE      -1
#define PORT_0_NODE        17 //13
#define PORT_1_NODE        18 //14
#define PORT_2_NODE        19 //15
#define PORT_3_NODE        20 //16
#define PORT_4_NODE        21 //17
#define N_MISC_RESOURCES_START 16 //12
#define N_MISC_RESOURCES_END 21 //17


#define N_BUFFER_NODES 5
#define RS_STALL_NODE  22 //21
#define ROB_STALL_NODE 23 //22
#define LB_STALL_NODE 24 //23
#define SB_STALL_NODE 25 //24
#define LFB_STALL_NODE 26 //25
#define N_BUFFER_NODES_START 22 //21
#define N_BUFFER_NODES_END 26 //25

#define N_PREFETCH_RESOURCES 6
#define FIRST_PREFETCH_LEVEL L2_LOAD_NODE
#define L2_LOAD_PREFETCH_NODE 27 //26
#define L2_STORE_PREFETCH_NODE 28 //27
#define L3_LOAD_PREFETCH_NODE 29 //28
#define L3_STORE_PREFETCH_NODE 30 //29
#define MEM_LOAD_PREFETCH_NODE 31 //30
#define MEM_STORE_PREFETCH_NODE 32 //31
#define N_PREFETCH_RESOURCES_START 33 //26
#define N_PREFETCH_RESOURCES_END 34 //31


#else

#define SANDY_BRIDGE_COMP_NODES 4
#define N_COMP_NODES_START 0
#define N_COMP_NODES_END 3

#define FP_ADD_NODE      0
#define FP_MUL_NODE      1
#define FP_DIV_NODE      2
#define VECTOR_SHUFFLE_NODE 3


#define SANDY_BRIDGE_MEM_NODES 8
#define L1_LOAD_NODE     4
#define L1_STORE_NODE      5
#define L2_LOAD_NODE     6
#define L2_STORE_NODE      7
#define L3_LOAD_NODE     8
#define L3_STORE_NODE      9
#define MEM_LOAD_NODE    10
#define MEM_STORE_NODE     11

#define N_MEM_NODES_START 4
#define N_MEM_NODES_END 11


#define N_MISC_RESOURCES 7
#define N_AGU_NODES     1
#define N_PORT_NODES  5
#define AGU_NODE           12
#define STORE_AGU_NODE     -1
#define LOAD_AGU_NODE      -1
#define PORT_0_NODE        13
#define PORT_1_NODE        14
#define PORT_2_NODE        15
#define PORT_3_NODE        16
#define PORT_4_NODE        17
#define N_MISC_RESOURCES_START 12
#define N_MISC_RESOURCES_END 17


#define N_BUFFER_NODES 5
#define RS_STALL_NODE  21
#define ROB_STALL_NODE 22
#define LB_STALL_NODE 23
#define SB_STALL_NODE 24
#define LFB_STALL_NODE 25
#define N_BUFFER_NODES_START 21
#define N_BUFFER_NODES_END 25

#define N_PREFETCH_RESOURCES 6
#define FIRST_PREFETCH_LEVEL L2_LOAD_NODE
#define L2_LOAD_PREFETCH_NODE  26
#define L2_STORE_PREFETCH_NODE 27
#define L3_LOAD_PREFETCH_NODE 28
#define L3_STORE_PREFETCH_NODE 29
#define MEM_LOAD_PREFETCH_NODE 30
#define MEM_STORE_PREFETCH_NODE 31
#define N_PREFETCH_RESOURCES_START 26
#define N_PREFETCH_RESOURCES_END 31




#endif

// ====================================================================//




#define N_TOTAL_NODES N_COMP_NODES+N_MEM_NODES+N_BUFFER_NODES+N_PREFETCH_RESOURCES+N_MISC_RESOURCES


//Define Microarchitectures
#define SANDY_BRIDGE 0
#define ATOM 1



using namespace llvm;
using namespace std;
using namespace SplayTree;
using namespace SplayTreeBoolean;
using namespace SimpleSplayTree;
using namespace ComplexSplayTree;

  // For FullOccupancyCyles, the vector has a different meaning that for AvailableCycles.
  // Each element of the vector contains the elements of the tree in a corresponding
  // rage.
  static const int SplitTreeRange = 131072;

struct CacheLineInfo{
  uint64_t IssueCycle;
  uint64_t LastAccess;
 // uint64_t ReuseDistance;
};


struct InstructionDispatchInfo{
  uint64_t IssueCycle;
  uint64_t CompletionCycle;
};


struct LessThanOrEqualValuePred
{
  uint64_t CompareValue;
  
  bool operator()(const uint64_t Value) const
  {
    return Value <= CompareValue;
    }
};
    
    struct StructMemberLessThanOrEqualThanValuePred
    {
      const uint64_t CompareValue;
      
      bool operator()(const InstructionDispatchInfo& v) const
      {
        return v.CompletionCycle <= CompareValue;
      }
    };
    

  
class TBV {
    class TBV_node {
        public:
        TBV_node():BitVector(20) {
            
        }
        //vector<bool> BitVector;
        dynamic_bitset<> BitVector; // from boost
    };
    
    private:
        vector<TBV_node> tbv_map;
        bool e;
    
  public:
    TBV();
    bool get_node(uint64_t key, unsigned bitPosition);
    bool get_node_nb(uint64_t key, unsigned bitPosition);
    void insert_node(uint64_t key, unsigned bitPosition);
    void delete_node(uint64_t key, unsigned bitPosition);
    bool empty();
};

struct ACTNode {
  public:
    uint64_t key;
    int32_t issueOccupancy;
    int32_t widthOccupancy;
    int32_t occupancyPrefetch;
    uint64_t address;
};
  
class ACT {
	private:
		//list<ACTNode*> act_map;
		//list<ACTNode*>::iterator currNode;
        vector< TBV> act_vec;
    
	public:
        //bool empty;
	//ACT(uint);// Ideally, we would resize knowing the size of the original spanning tree
	bool get_node(uint64_t, unsigned);
	void push_back(ACTNode*, unsigned);
    void DebugACT();
    size_t size();
};

uint64_t BitScan(vector< TBV> &FullOccupancyCyclesTree, uint64_t key, unsigned bitPosition);
    
class DynamicAnalysis {
  
public:
  
  unsigned TotalResources;
  unsigned nExecutionUnits;
  unsigned nCompExecutionUnits;
  unsigned nMemExecutionUnits;
  unsigned nPorts;
  unsigned nBuffers;
  unsigned nAGUs;
  unsigned nLoadAGUs;
  unsigned nStoreAGUs;
  unsigned nNodes;
  unsigned nPrefetchNodes;
  unsigned nCompNodes;
  unsigned nMemNodes;
  unsigned MemoryWordSize;
  unsigned CacheLineSize;
  
  //Cache sizes are specified in number of cache lines of size CacheLineSize
  unsigned L1CacheSize;
  unsigned L2CacheSize;
  unsigned LLCCacheSize;
  
  unsigned BitsPerCacheLine;
  
  unsigned VectorWidth;
  
  //TODO: COmment out
   vector<uint64_t> FlopLatencies;
   vector<uint64_t> MemLatencies;

  //For every node, the execution unit in which it executes.
  vector<unsigned> ExecutionUnit;
  vector<unsigned> ExecutionPort;
  vector<unsigned> PortNodes;
  vector<vector<unsigned> > DispatchPort;
  
  vector<unsigned> ExecutionUnitsLatency;
  vector<float> ExecutionUnitsThroughput;
  vector<int> ExecutionUnitsParallelIssue;
  vector<unsigned> PortsWidth;
  
  vector<unsigned> IssueCycleGranularities;
  vector<unsigned> AccessWidths;
  
  vector<float> Throughputs;
  vector<float> PortsWidths;
  // Port-Node type that can be executed on that port.
  vector<vector<unsigned> > PortsBindings;
  
  
  // vector<float> FlopThroughputs;
  // vector<float> MemThroughputs;
  
  // TODO: COmment out
  vector<float> IssueThroughputs;
  vector<float> IssueWidths;
  
  
  vector<unsigned > AccessGranularities;
  vector<string> ResourcesNames;
  vector<string> NodesNames;

  

 
  bool LimitILP;
  bool LimitMLP;
  
  uint64_t BasicBlockBarrier;
  uint64_t BasicBlockLookAhead;
  int64_t RemainingInstructionsFetch;
  uint64_t InstructionFetchCycle;
  uint64_t LoadDispatchCycle;
  uint64_t StoreDispatchCycle;
  vector<uint64_t> ReservationStationIssueCycles;
  deque<uint64_t> ReorderBufferCompletionCycles;
  vector<uint64_t> LoadBufferCompletionCycles;
  SimpleTree<uint64_t> *LoadBufferCompletionCyclesTree;
  vector<uint64_t> StoreBufferCompletionCycles;
  vector<uint64_t> LineFillBufferCompletionCycles;
  vector<InstructionDispatchInfo> DispatchToLoadBufferQueue;
  ComplexTree<uint64_t> *DispatchToLoadBufferQueueTree;

  vector<InstructionDispatchInfo> DispatchToStoreBufferQueue;
  vector<InstructionDispatchInfo> DispatchToLineFillBufferQueue;
  
  bool RARDependences;
  bool WarmCache;
  bool x86MemoryModel;
  bool SpatialPrefetcher;
  bool ConstraintPorts;
  bool ConstraintAGUs;
  unsigned PrefetchLevel;
  unsigned PrefetchDispatch;
  unsigned PrefetchTarget;
  unsigned PrefetchDestination;

  bool InOrderExecution;
  bool ReportOnlyPerformance;
  
  unsigned ReservationStationSize;
  int AddressGenerationUnits;
  int InstructionFetchBandwidth;
  unsigned ReorderBufferSize;
  unsigned LoadBufferSize;
  unsigned StoreBufferSize;
  unsigned LineFillBufferSize;
  
  string TargetFunction;
  uint8_t FunctionCallStack;
  
  
  uint8_t uarch;
  
  unsigned SourceCodeLine;
  
  int rep;
 
#ifdef INT_FP_OPS
// Begin new
  // Number of bytes transferred from/to L1, L2, LLC, Mem, Total respectively
  // Depends on program
  vector<unsigned> Q;

  // Throughput (bytes/second) for L1, L2, LLC, Mem respectively
  vector<float> Beta;

  // Computation throughput bound (ops/second)
  float Pi;
// End new
#endif
 
  // Variables to track instructions count
  uint64_t TotalInstructions;
  uint64_t TotalSpan;
  uint64_t NFirstAccesses;
  vector<uint64_t> InstructionsCount;
  vector<uint64_t> InstructionsCountExtended;
  vector<uint64_t> InstructionsSpan;
  vector<uint64_t> InstructionsLastIssueCycle;
  vector<uint64_t> IssueSpan;
  vector<uint64_t> SpanGaps;
  vector<uint64_t> FirstNonEmptyLevel;
  vector<uint64_t> BuffersOccupancy;
  vector<uint64_t> LastIssueCycleVector;
  
  vector<unsigned> MaxOccupancy;
  vector<bool> FirstIssue;
  
  deque<uint64_t> RegisterFile;
  
  uint64_t LastLoadIssueCycle;
  uint64_t LastStoreIssueCycle;
  
  
  vector< vector< unsigned > > DAGLevelsOccupancy;
  /*
  vector< Tree<uint64_t> * > AvailableCyclesTree;
  vector< Tree<uint64_t> * > FullOccupancyCyclesTree;
  
  vector< TreeBitVector<uint64_t> * > AvailableCyclesTreeBitVector;
  vector< TreeBitVector<uint64_t> * > FullOccupancyCyclesTreeTreeBitVector;
  */
  
  vector< Tree<uint64_t> * > AvailableCyclesTree;
  ACT ACTFinal;
  
  vector< TBV> FullOccupancyCyclesTree;
  
  
  vector <Tree<uint64_t> * > StallCycles;
  vector <uint64_t> NInstructionsStalled;
  
  uint64_t MinLoadBuffer;
  uint64_t MaxDispatchToLoadBufferQueueTree;
vector<ComplexTree<uint64_t> *> PointersToRemove;
  
  //Statistics
  double AverageILP;
  
  
  map <Value*, uint64_t> InstructionValueIssueCycleMap;
  map <Value*, uint64_t> InstructionValueUseCycleMap;
   map <uint64_t , CacheLineInfo> CacheLineIssueCycleMap;
   map <uint64_t , uint64_t> MemoryAddressIssueCycleMap;
  Tree<uint64_t> * ReuseTree;
  Tree<uint64_t> * PrefetchReuseTree;
  uint64_t PrefetchReuseTreeSize;
  double ErrorApproximationReuse;
  
  vector< vector<unsigned> > ParallelismDistribution;
  map<int,int> ReuseDistanceDistribution;
  map<int,map<uint64_t,uint> > ReuseDistanceDistributionExtended;
  
#ifdef SOURCE_CODE_ANALYSIS
  unordered_map<uint64_t,set<uint64_t> > SourceCodeLineOperations;
  unordered_map<uint64_t,set<uint64_t> > SourceCodeLineInfo;
  unordered_map<uint64_t,vector<uint64_t> > SourceCodeLineInfoBreakdown;
#endif
  
  //Constructor
  DynamicAnalysis(string TargetFunction,
                  string Microarchitecture,
                  unsigned MemoryWordSize,
                  unsigned CacheLineSize,
                  unsigned L1CacheSize,
                  unsigned L2CacheSize,
                  unsigned LLCCacheSize,
                  vector<float> ExecutionUnitsLatency,
                  vector<float> ExecutionUnitsThroughput,
                  vector<int> ExecutionUnitsParallelIssue,
                  vector<unsigned>  MemAccessGranularity,
                  int AddressGenerationUnits,
                  int InstructionFetchBandwidth,
                  int ReservationStationSize,
                  int ReorderBufferSize,
                  int LoadBufferSize,
                  int StoreBufferSize,
                  int LineFillBufferSize,
                  bool WarmCache,
                  bool x86MemoryModel,
                  bool SpatialPrefetcher,
                  bool ConstraintPorts,
                  bool ConstraintAGUs,
                  int rep,
                  bool InOrderExecution,
                  bool ReportOnlyPerformance,
                  unsigned PrefetchLevel,
                  unsigned PrefetchDispatch,
                  unsigned PrefetchTarget);

  
  
  vector<Instruction*> instructionPool;
  
  
  void analyze();
#ifdef INTERPRETER
void analyzeInstruction(Instruction &I, ExecutionContext &SF,  GenericValue * visitResult);
#else
void analyzeInstruction (Instruction &I, uint64_t addr);
#endif  
  
  void insertInstructionValueIssueCycle(Value* v,uint64_t InstructionIssueCycle, bool isPHINode = 0 );
  void insertCacheLineLastAccess(uint64_t v,uint64_t LastAccess );
  void insertCacheLineInfo(uint64_t v,CacheLineInfo Info );
  void insertMemoryAddressIssueCycle(uint64_t v,uint64_t Cycle );
  
  
  uint64_t getInstructionValueIssueCycle(Value* v);
  uint64_t getCacheLineLastAccess(uint64_t v);
  CacheLineInfo getCacheLineInfo(uint64_t v);
  uint64_t getMemoryAddressIssueCycle(uint64_t v);
  
  unsigned  GetInstructionTypeFromPrefetchType(unsigned PrefetchType);

  
  uint64_t GetLastIssueCycle(unsigned ExecutionResource, bool WithPrefetch = false);
    
  uint64_t GetTreeChunk(uint64_t i);
  
  //Returns the DAG level occupancy after the insertion
  unsigned FindNextAvailableIssueCycle(unsigned OriginalCycle, unsigned ExecutionResource, uint64_t ExtendedInstructionType,  uint8_t NElementsVector = 1, bool TargetLevel = true);
  unsigned FindNextAvailableIssueCyclePortAndThroughtput(unsigned InstructionIssueCycle, unsigned ExtendedInstructionType, unsigned NElementsVector=1);
  
  bool ThereIsAvailableBandwidth(unsigned NextAvailableCycle, unsigned ExecutionResource, bool& FoundInFullOccupancyCyclesTree, bool TargetLevel);
  
  uint64_t FindNextAvailableIssueCycleUntilNotInFullOrEnoughBandwidth(unsigned NextCycle, unsigned ExecutionResource , bool& FoundInFullOccupancyCyclesTree, bool& EnoughBandwidth);
  
  bool InsertNextAvailableIssueCycle(uint64_t NextAvailableCycle, unsigned ExecutionResource, uint64_t ExtendedInstructionType, unsigned NElementsVector = 1, bool isPrefetch=0);
  
  void IncreaseInstructionFetchCycle(bool EmptyBuffers = false);
  
  unsigned CalculateIssueCycleGranularity(unsigned ExecutionResource, unsigned NElementsVector=1);
  
  uint64_t CalculateSpan(int ResourceType);
  //unsigned CalculateGroupSpan(int NResources, ...);
  unsigned CalculateGroupSpan(vector<int> & ResourcesVector, bool WithPrefetch = false, bool ForceUnitLatency = false);
  
  
  unsigned CalculateIssueSpan(vector<int> & ResourcesVector);
  
  // NEW FINAL VERSIONS
  uint64_t CalculateSpanFinal(int ResourceType);
  //unsigned CalculateGroupSpan(int NResources, ...);
  unsigned CalculateGroupSpanFinal(vector<int> & ResourcesVector, bool WithPrefetch = false, bool ForceUnitLatency = false);
  
  unsigned CalculateIssueSpanFinal(vector<int> & ResourcesVector);
  bool IsEmptyLevelFinal(unsigned ExecutionResource, uint64_t Level, bool& IsInAvailableCyclesTree,
                    bool& IsInFullOccupancyCyclesTree , bool WithPrefetch = false);
  
  unsigned CalculateGroupSpanUnitLatency(vector<int> & ResourcesVector, bool ForceUnitLatency = false);

  unsigned CalculateResourceStallSpan(int resource, int stall);
  void CalculateResourceStallOverlapCycles(Tree<uint64_t> * n, int resource, uint64_t & OverlapCycles);

  
  void CollectSourceCodeLineStatistics(uint64_t ResourceType, uint64_t Cycle,  uint64_t MaxLatencyLevel, uint64_t SpanIncrease, bool IsInFullOccupancyCyclesTree, bool IsInAvailableCyclesTree);
  
  bool IsEmptyLevel(unsigned ExecutionResource, uint64_t Level, bool& IsInAvailableCyclesTree,
                    bool& IsInFullOccupancyCyclesTree , bool WithPrefetch = false);
  uint64_t FindNextNonEmptyLevel(unsigned ExecutionResource, uint64_t Level);
  bool isStallCycle(int ResourceType, uint64_t Level);


  unsigned GetMemoryInstructionType(int ReuseDistance, uint64_t MemoryAddress,bool isLoad=true);
    unsigned GetExtendedInstructionType(int OpCode, int ReuseDistance=0);
  unsigned GetPositionSourceCodeLineInfoVector(uint64_t Resource);
      
  uint64_t GetMinIssueCycleReservationStation();
  uint64_t GetMinCompletionCycleLoadBuffer();
  uint64_t GetMinCompletionCycleLoadBufferTree();

  uint64_t GetMinCompletionCycleStoreBuffer();
   uint64_t GetMinCompletionCycleLineFillBuffer();
  
  void RemoveFromReservationStation(uint64_t Cycle);
  void RemoveFromReorderBuffer(uint64_t Cycle);
  void RemoveFromLoadBuffer(uint64_t Cycle);
  void RemoveFromLoadBufferTree(uint64_t Cycle);

  void RemoveFromStoreBuffer(uint64_t Cycle);
  void RemoveFromLineFillBuffer(uint64_t Cycle);
  
  void RemoveFromDispatchToLoadBufferQueue(uint64_t Cycle);
    void RemoveFromDispatchToLoadBufferQueueTree(uint64_t Cycle);
  void RemoveFromDispatchToStoreBufferQueue(uint64_t Cycle);
  void RemoveFromDispatchToLineFillBufferQueue(uint64_t Cycle);
  
  ComplexTree<uint64_t> * RemoveFromDispatchAndInsertIntoLoad(uint64_t i, ComplexTree<uint64_t> * t);
  void inOrder(uint64_t i, ComplexTree<uint64_t> * n);
  
  void DispatchToLoadBuffer(uint64_t Cycle);
  void DispatchToLoadBufferTree(uint64_t Cycle);

  void DispatchToStoreBuffer(uint64_t Cycle);
  void DispatchToLineFillBuffer(uint64_t Cycle);
  
  uint64_t FindIssueCycleWhenLoadBufferIsFull();
  uint64_t FindIssueCycleWhenLoadBufferTreeIsFull();

  uint64_t FindIssueCycleWhenStoreBufferIsFull();
  uint64_t FindIssueCycleWhenLineFillBufferIsFull();
  
  void PrintReorderBuffer();
  void PrintReservationStation();
  void PrintLoadBuffer();
  void PrintLoadBufferTreeRecursive(SimpleTree<uint64_t> * p);
  void PrintDispatchToLoadBufferTreeRecursive(ComplexTree<uint64_t> * p, bool key);
  void PrintDispatchToLoadBufferTree();

  void PrintLoadBufferTree();
  void PrintStoreBuffer();
  void PrintLineFillBuffer();
  void PrintDispatchToStoreBuffer();
  void PrintDispatchToLoadBuffer();
  void PrintDispatchToLineFillBuffer();
  
  int ReuseDistance(uint64_t Last, uint64_t Current, uint64_t address, bool FromPrefetchReuseTree = false);
  int ReuseTreeSearchDelete(uint64_t Current, uint64_t address,  bool FromPrefetchReuseTree = false);
  //int ReuseTreeSearchDelete(uint64_t Last, bool FromPrefetchReuseTree = false);
  void updateReuseDistanceDistribution(int Distance, uint64_t InstructionIssueCycle);
  unsigned int roundNextPowerOfTwo(unsigned int v);
  unsigned int roundNextMultiple(uint64_t num, int multiple);
  unsigned int roundNextMultipleOf2(uint64_t num);
unsigned int DivisionRoundUp(float a, float b);
  void finishAnalysis();
  void printHeaderStat(string Header);
  
  int getInstructionType(Instruction &I);
  
  void ComputeAvailableTreeFinalHelper(uint p, Tree<uint64_t>* t, uint d);
  void ComputeAvailableTreeFinal(bool WithPrefetch = false);
  void DebugACT(uint p);
};
#endif
