#include "DynamicAnalysis.h"



/* An alternative to optimize calculateSpan could be merging the
 AvailableCyclesTree and FullOccupancyCyclesTree and doing and
 ca inorder/ postoder travesal */
uint64_t
DynamicAnalysis::CalculateSpanFinal(int ResourceType){
  
  uint64_t Span = 0;
  bool IsInAvailableCyclesTree = false;
  bool IsInFullOccupancyCyclesTree = false;
  
  //If there are instructions of this type....
  if (InstructionsCountExtended[ResourceType]>0) {
    
    uint64_t Latency = ExecutionUnitsLatency[ResourceType];
    uint64_t First= FirstNonEmptyLevel[ResourceType];
    uint64_t DominantLevel = First;
    uint64_t LastCycle = LastIssueCycleVector[ResourceType];
    
#ifdef DEBUG_SPAN_CALCULATION
    DEBUG(dbgs() << "First  " << First << "\n");
    DEBUG(dbgs() << "Latency  " << Latency << "\n");
    DEBUG(dbgs() << "LastCycle  " << LastCycle << "\n");
    DEBUG(dbgs() << "Increasing span to  " << Latency << "\n");
#endif
    
    Span+= Latency;
    
    //Start from next level to first non-emtpy level
    //  for(unsigned i=First+IssueCycleGranularity; i <= LastCycle; i+=IssueCycleGranularity){
    for(unsigned i=First+1; i <= LastCycle; i+=1){
      //Check whether there is instruction scheduled in this cycle!
      IsInAvailableCyclesTree = false;
      IsInFullOccupancyCyclesTree = false;
      if (IsEmptyLevelFinal( ResourceType,i, IsInAvailableCyclesTree, IsInFullOccupancyCyclesTree)==false) {
        if ( DominantLevel+Latency!= 0 && i <= DominantLevel+Latency-1){
          if (i+Latency > DominantLevel+Latency && Latency!=0) {
#ifdef DEBUG_SPAN_CALCULATION
            DEBUG(dbgs() << "Increasing Span by the difference " << ((i+Latency)-max((DominantLevel+Latency),(uint64_t)1)) << "\n");
#endif
            Span+=((i+Latency)-max((DominantLevel+Latency),(uint64_t)1));
            DominantLevel = i;
          }
        }else{
#ifdef DEBUG_SPAN_CALCULATION
          DEBUG(dbgs() << "Increasing Span by " << Latency << "\n");
#endif
          Span+=Latency;
          DominantLevel = i;
        }
      }
    }
  }
  return Span;
}

bool
DynamicAnalysis::IsEmptyLevelFinal(unsigned ExecutionResource, uint64_t Level, bool& IsInAvailableCyclesTree,
                              bool& IsInFullOccupancyCyclesTree, bool WithPrefetch){
  
  // bool IsInAvailableCyclesTree = false;
  // bool IsInFullOccupancyCyclesTree = false;
  
  if (ExecutionResource <= nExecutionUnits) {
    /*ACTNode* n = ACTFinal[ExecutionResource].get_node(Level);
    if (n != NULL &&
        (n->issueOccupancy != 0 ||
        (WithPrefetch && n->occupancyPrefetch != 0)))*/
    if (ACTFinal.get_node(Level, ExecutionResource))
    {
      IsInAvailableCyclesTree = true;
      return false;
    }
    /*
    if (AvailableCyclesTree[ExecutionResource]!= NULL) {
      AvailableCyclesTree[ExecutionResource] = splay(Level, AvailableCyclesTree[ExecutionResource] );
      if (Level == AvailableCyclesTree[ExecutionResource]->key){
        if( AvailableCyclesTree[ExecutionResource]->issueOccupancy != 0 || 
           (WithPrefetch && AvailableCyclesTree[ExecutionResource]->occupancyPrefetch != 0)){
          IsInAvailableCyclesTree = true;
          return false;
        }
      }
    }*/
  }else
    IsInAvailableCyclesTree = false;

  int TreeChunk = Level/SplitTreeRange;
  IsInFullOccupancyCyclesTree = FullOccupancyCyclesTree[TreeChunk].get_node(Level, ExecutionResource);
  
  if (IsInFullOccupancyCyclesTree == false && IsInAvailableCyclesTree == false) {
    return true;
  }else
    return false;
}

unsigned
DynamicAnalysis::CalculateGroupSpanFinal(vector<int> & ResourcesVector, bool WithPrefetch, bool ForceUnitLatency){
  
  unsigned Span = 0;
  unsigned MaxLatency = 0;
  uint64_t First = 0;
  bool EmptyLevel = true;
  bool IsGap = false;
  int NResources = ResourcesVector.size();
  uint64_t LastCycle = 0;
  uint64_t ResourceLastCycle = 0;
  unsigned MaxLatencyLevel = 0;
  unsigned ResourceType = 0;
  unsigned AccessWidth = 0;
  bool IsInAvailableCyclesTree = false;
  bool IsInFullOccupancyCyclesTree = false;
  unsigned SpanIncrease = 0;
  
  // vector<uint64_t> NextNonEmptyLevelVector;
  // uint64_t NextNonEmptyLevel;
  
#ifdef DEBUG_SPAN_CALCULATION
  DEBUG(dbgs() << "Resources that contribute to Span:\n");
  for (int j= 0; j< NResources; j++) {
    DEBUG(dbgs() << ResourcesVector[j] << "\n");
  }
#endif
  
  //Determine first non-empty level and LastCycle
  for (int j= 0; j< NResources; j++) {
    
    ResourceType = ResourcesVector[j];
    
    if (InstructionsCountExtended[ResourceType]>0) {
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "There are instructions of type "<< ResourceType<<"\n");
#endif
      AccessWidth = AccessWidths[ResourceType];
      
      if (EmptyLevel == true) { // This will be only executed the first time of a non-empty level
        EmptyLevel = false;
        First = FirstNonEmptyLevel[ResourceType];
        
        if (ExecutionUnitsThroughput[ResourceType] == INF) {
          MaxLatency = ExecutionUnitsLatency[ResourceType];
        }else
          MaxLatency = max(ExecutionUnitsLatency[ResourceType],(unsigned)ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]));
      }else{
        if (First == FirstNonEmptyLevel[ResourceType]){
          if (ExecutionUnitsThroughput[ResourceType] == INF) {
            MaxLatency = max(MaxLatency,ExecutionUnitsLatency[ResourceType]);
          }else
            MaxLatency = max(MaxLatency,max(ExecutionUnitsLatency[ResourceType],(unsigned)ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType])));
        }
        else{
          First = min(First,FirstNonEmptyLevel[ResourceType]);{
            if (First == FirstNonEmptyLevel[ResourceType]){
              if (ExecutionUnitsThroughput[ResourceType] == INF) {
                MaxLatency =ExecutionUnitsLatency[ResourceType];
              }else
                MaxLatency = max(ExecutionUnitsLatency[ResourceType],(unsigned)ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]));
            }
          }
        }
      }
      if (ForceUnitLatency == true)
        MaxLatency = 1;
      
      ResourceLastCycle = LastIssueCycleVector[ResourceType];
      // NextNonEmptyLevelVector.push_back(FirstNonEmptyLevel[ResourceType]);
      
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "Calling GetLastIssueCycle with args "<< ResourceType << "  " << WithPrefetch<<"\n");
      DEBUG(dbgs() << "Last cycle returned from ResourceLastCycle "<< ResourceLastCycle<<"\n");
      DEBUG(dbgs() << "First non-empty level  " << First << "\n");
      
#endif
      LastCycle = max(LastCycle, ResourceLastCycle);
      
#ifdef SOURCE_CODE_ANALYSIS
      if(NResources == 1 && ResourceType < nExecutionUnits ){
        unsigned TreeChunk = GetTreeChunk(First);
        FullOccupancyCyclesTree[TreeChunk] = splay(First,  FullOccupancyCyclesTree[TreeChunk]);
        AvailableCyclesTree[ResourceType] = splay(First, AvailableCyclesTree[ResourceType]);
        CollectSourceCodeLineStatistics(ResourceType, First, MaxLatency, MaxLatency-1, true,true);
      }
      
      
      if(NResources == 1 && ResourceType >= RS_STALL){
        unsigned TreeChunk = GetTreeChunk(First);
        FullOccupancyCyclesTree[TreeChunk] = splay(First,  FullOccupancyCyclesTree[TreeChunk]);
        CollectSourceCodeLineStatistics(ResourceType, First, MaxLatency, MaxLatency-1, true,false);
      }
      
      
#endif
      
    }
  }
  
#ifdef DEBUG_SPAN_CALCULATION
  DEBUG(dbgs() << "First non-empty level  " << First << "\n");
  DEBUG(dbgs() << "MaxLatency  " << MaxLatency << "\n");
  DEBUG(dbgs() << "LastCycle  " << LastCycle << "\n");
#endif
  unsigned DominantLevel = First;
  
  
  
  
  
  if (EmptyLevel == false) {
    Span+= MaxLatency;
    
    
    for(unsigned i=First+1; i<= LastCycle; i++){
      // For sure there is at least resource for which this level is not empty.
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "i =   " << i << "\n");
#endif
      //Determine MaxLatency of Level
      MaxLatencyLevel = 0;
      for(int j=0; j< NResources; j++){
        ResourceType = ResourcesVector[j];
        
        if (i <= LastIssueCycleVector[ResourceType]/*GetLastIssueCycle(ResourceType, WithPrefetch)*/ ) {
          IsInAvailableCyclesTree = false;
          IsInFullOccupancyCyclesTree = false;
          if (IsEmptyLevelFinal(ResourceType, i, IsInAvailableCyclesTree, IsInFullOccupancyCyclesTree, WithPrefetch) == false) {
            IsGap = false;
            // MaxLatencyLevel = max(MaxLatencyLevel, GetInstructionLatency(ResourcesVector[j]));
            if (ForceUnitLatency ==true) {
              MaxLatencyLevel = 1;
            }else{
              AccessWidth =AccessWidths[ResourceType];
              
              if (ExecutionUnitsThroughput[ResourceType] == INF) {
                MaxLatencyLevel = max(MaxLatencyLevel, ExecutionUnitsLatency[ResourceType]);
                
                
              }else{
                MaxLatencyLevel = max(MaxLatencyLevel, max(ExecutionUnitsLatency[ResourceType],(unsigned)ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType])));
              }
            }
          }
        }
      }
      
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "MaxLatencyLevel  " << MaxLatencyLevel << "\n");
#endif
      
      //That is, only if there are instructions scheduled in this cycle
      if(MaxLatencyLevel !=0){
        
        // Add the first condition because if Latency=0 is allowed, it can happen
        // that DominantLevel+MaxLatency-1 is a negative number, so the loop
        // is entered incorrectly.
        if ( DominantLevel+MaxLatency!= 0 &&  i <= DominantLevel+MaxLatency-1){
          
          if (i+MaxLatencyLevel > DominantLevel+MaxLatency && MaxLatencyLevel!=0) {
            
            SpanIncrease = ((i+MaxLatencyLevel)-max((DominantLevel+MaxLatency),(unsigned)1)) ;
            
            
            
#ifdef DEBUG_SPAN_CALCULATION
            DEBUG(dbgs() << "Increasing Span by the difference " << SpanIncrease<< "\n");
#endif
            
            Span+=SpanIncrease;
            DominantLevel = i;
            MaxLatency = MaxLatencyLevel;
          }
        }else{
          
#ifdef DEBUG_SPAN_CALCULATION
          DEBUG(dbgs() << "Increasing Span by " << MaxLatencyLevel << "\n");
#endif
          SpanIncrease = MaxLatencyLevel-1;
          Span+=MaxLatencyLevel;
          DominantLevel = i;
          MaxLatency = MaxLatencyLevel;
        }
#ifdef SOURCE_CODE_ANALYSIS
        if(NResources == 1  && (ResourceType < nExecutionUnits || ResourceType >= RS_STALL)){
          CollectSourceCodeLineStatistics(ResourceType, i, MaxLatencyLevel, SpanIncrease, IsInFullOccupancyCyclesTree,IsInAvailableCyclesTree);
        }
        
#endif
      }else{
        if ( i > DominantLevel+MaxLatency-1){
          if (NResources==1 && IsGap == false) {
            SpanGaps[ResourceType]++;
#ifdef DEBUG_SPAN_CALCULATION
            DEBUG(dbgs() << "Increasing span gaps for resource " << ResourcesNames[ResourceType] << "\n");
#endif
            IsGap = true;
          }
        }
      }
      
    }
  }
  
  return Span;
}

unsigned
DynamicAnalysis::CalculateIssueSpanFinal(vector<int> & ResourcesVector){
  
  unsigned Span = 0;
  unsigned MaxLatency = 0;
  uint64_t First = 0;
  bool EmptyLevel = true;
  int NResources = ResourcesVector.size();
  uint64_t LastCycle = 0;
  uint64_t ResourceLastCycle = 0;
  unsigned MaxLatencyLevel = 0;
  unsigned ResourceType = 0;
  unsigned AccessWidth = 0;
  unsigned TmpLatency = 0;
  bool IsInAvailableCyclesTree = false;
  bool IsInFullOccupancyCyclesTree = false;
  
  
#ifdef DEBUG_SPAN_CALCULATION
  DEBUG(dbgs() << "Resources that contribute to Span:\n");
  for (int j= 0; j< NResources; j++) {
    DEBUG(dbgs() << ResourcesVector[j] << "\n");
  }
#endif
  
  //Determine first non-empty level and LastCycle
  for (int j= 0; j< NResources; j++) {
    
    ResourceType = ResourcesVector[j];
    
    if (InstructionsCountExtended[ResourceType]>0) {
      
      AccessWidth = AccessWidths[ResourceType];
      if (ExecutionUnitsThroughput[ResourceType]==INF)
        TmpLatency = 1;
      else
        TmpLatency = ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]);
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "AccessWidth "<< AccessWidth<<"\n");
      DEBUG(dbgs() << "There are instructions of type "<< ResourceType<<"\n");
#endif
      if (EmptyLevel == true) { // This will be only executed the first time of a non-empty level
        EmptyLevel = false;
        First = FirstNonEmptyLevel[ResourceType];
        //    MaxLatency = ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]);
        MaxLatency = TmpLatency;
      }else{
        if (First == FirstNonEmptyLevel[ResourceType])
          // MaxLatency = max(MaxLatency,(unsigned)ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]));
          MaxLatency = max(MaxLatency,TmpLatency);
        else{
          First = min(First,FirstNonEmptyLevel[ResourceType]);{
            if (First == FirstNonEmptyLevel[ResourceType])
              //MaxLatency = ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]);
              MaxLatency = TmpLatency;
          }
        }
      }
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "ResourceType "<< ResourceType<<"\n");
      DEBUG(dbgs() << "LastIssueCycleVector size "<< LastIssueCycleVector.size()<<"\n");
      DEBUG(dbgs() << "LastIssueCycleVector[ResourceType] "<< LastIssueCycleVector[ResourceType]<<"\n");
#endif
      ResourceLastCycle = LastIssueCycleVector[ResourceType];
      
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "Last cycle returned from ResourceLastCycle "<< ResourceLastCycle<<"\n");
#endif
      LastCycle = max(LastCycle, ResourceLastCycle);
    }
  }
  
  
#ifdef DEBUG_SPAN_CALCULATION
  DEBUG(dbgs() << "First non-empty level  " << First << "\n");
  DEBUG(dbgs() << "MaxLatency  " << MaxLatency << "\n");
  DEBUG(dbgs() << "LastCycle  " << LastCycle << "\n");
#endif
  unsigned DominantLevel = First;
  //  if (First >= 0) {
  if (EmptyLevel == false) {
    Span+= MaxLatency;
    
    //Start from next level to first non-emtpy level
    for(unsigned i=First+1; i<= LastCycle; i++){
      //Determine MaxLatency of Level
      MaxLatencyLevel = 0;
      for(int j=0; j< NResources; j++){
        ResourceType = ResourcesVector[j];
        
        if (i <= LastIssueCycleVector[ResourceType]/*GetLastIssueCycle(ResourceType, 0)*/) {
          IsInAvailableCyclesTree = false;
          IsInFullOccupancyCyclesTree = false;
          if (IsEmptyLevelFinal(ResourceType, i, IsInAvailableCyclesTree, IsInFullOccupancyCyclesTree, false) == false) {
            AccessWidth = AccessWidths[ResourceType];
            if (ExecutionUnitsThroughput[ResourceType]==INF)
              TmpLatency = 1;
            else
              TmpLatency = ceil(AccessWidth/ExecutionUnitsThroughput[ResourceType]);
            MaxLatencyLevel = max(MaxLatencyLevel, TmpLatency);
          }
        }
      }
      
#ifdef DEBUG_SPAN_CALCULATION
      DEBUG(dbgs() << "i =   " << i << "\n");
      DEBUG(dbgs() << "MaxLatencyLevel  " << MaxLatencyLevel << "\n");
#endif
      
      //That is, only if there are instructions scheduled in this cycle
      if(MaxLatencyLevel !=0){
        if ( i <= DominantLevel+MaxLatency-1){
          
          
          if (i+MaxLatencyLevel > DominantLevel+MaxLatency && MaxLatencyLevel!=0) {
#ifdef DEBUG_SPAN_CALCULATION
            DEBUG(dbgs() << "Increasing Span by the difference " << ((i+MaxLatencyLevel)-max((DominantLevel+MaxLatency),(unsigned)1)) << "\n");
#endif
            
            Span+=((i+MaxLatencyLevel)-max((DominantLevel+MaxLatency),(unsigned)1));
            DominantLevel = i;
            MaxLatency = MaxLatencyLevel;
          }
        }else{
          
#ifdef DEBUG_SPAN_CALCULATION
          DEBUG(dbgs() << "Increasing Span by " << MaxLatencyLevel << "\n");
#endif
          
          Span+=MaxLatencyLevel;
          DominantLevel = i;
          MaxLatency = MaxLatencyLevel;
        }
      }
    }
  }
#ifdef DEBUG_SPAN_CALCULATION
  DEBUG(dbgs() << "Span = " << Span << "\n");
#endif
  return Span;
}

/*void DynamicAnalysis::ComputeAvailableTreeFinalHelper(uint p, Tree<uint64_t>* t, uint d)
{
    if (t == NULL) return;
    ComputeAvailableTreeFinalHelper(p, t->left, d + 1);
    ComputeAvailableTreeFinalHelper(p, t->right, d + 1);
    {
      ACTNode* n = new ACTNode;
      assert(n != NULL);
      assert(t != NULL);
      n->key = t->key;
      n->issueOccupancy = t->issueOccupancy;
      n->widthOccupancy = t->widthOccupancy;
      n->occupancyPrefetch = t->occupancyPrefetch;
      n->address = t->address;
      ACTFinal[p].push_back(n);
    }
    // Cannot delete old tree until we know that it isn't needed.
    delete t;
} */

// Tree is unbalanced, switch from recursive to iterative method
void DynamicAnalysis::ComputeAvailableTreeFinalHelper(uint p, Tree<uint64_t>* t, uint d)
{
    bool done = false;
    bool rightUp = false; // Was prev from the right branch?
    Tree<uint64_t>* prev = t->prev;
    uint64_t lastKey = 0;
    
    while (!done)
    {
        if (t->left != NULL)
        {
            t->left->prev = t;
            t = t->left;
            continue;
        }
        // insert element
        if (t->key >= lastKey)
        {
            ACTNode* n = new ACTNode;
          
            n->key = t->key;
            n->issueOccupancy = t->issueOccupancy;
            n->widthOccupancy = t->widthOccupancy;
            n->occupancyPrefetch = t->occupancyPrefetch;
            n->address = t->address;
            ACTFinal.push_back(n, p);
            
            lastKey = t->key;
        }
        
        if (t->right != NULL)
        {
            t->right->prev = t;
            t = t->right;
            continue;
        }
        
        if (t->prev != NULL)
        {
            Tree<uint64_t>* old = t;
            t = t->prev;
            if (t->left == old) t->left = NULL;
            if (t->right == old) t->right = NULL;
            delete old;
            continue;
        }
        else
        {
            done = true;
        }
    }  
}

void DynamicAnalysis::ComputeAvailableTreeFinal(bool WithPrefetch)
{
    uint p = 0;
    //ACTFinal.resize(AvailableCyclesTree.size());
    for (auto it = AvailableCyclesTree.begin(), et = AvailableCyclesTree.end(); it != et; ++it)
    {
        //ACTFinal[p].empty = true;
        if ((*it) != NULL)
        {
            errs() << "ACT convert on " << p << "\t" << (*it)->size << "\t";
            ComputeAvailableTreeFinalHelper(p, *it, 0);
            *it = NULL;
            //errs() << ACTFinal[p].size() << "\n";
            //ACTFinal[p].empty = false;
        }
        p++;
    }
}

void ACT::push_back(ACTNode* n, unsigned BitPosition)
{
    uint64_t i = n->key;
    uint64_t TreeChunk = i/SplitTreeRange;
    if (TreeChunk >= act_vec.size()) {
        act_vec.resize(TreeChunk+1);
    }
    
    bool cond = (n->issueOccupancy != 0); // Add optional prefetch conditional
    if (cond)
        act_vec[TreeChunk].insert_node(n->key, BitPosition);
    
    delete n;
}

bool ACT::get_node(uint64_t key, unsigned BitPosition)
{
    uint64_t TreeChunk = key / SplitTreeRange;
    if (TreeChunk >= act_vec.size())
    {
        return false;
    }
    
    return act_vec[TreeChunk].get_node(key, BitPosition);
}

size_t ACT::size()
{
    //return act_map.size();
    return 0;
}

/*void ACT::push_back(ACTNode* n)
{
    if (n == NULL) return;
    assert(act_map.empty() || ((act_map.back())->key < n->key));
    act_map.push_back(n);
    currNode = act_map.begin();
}*/

void DynamicAnalysis::DebugACT(uint p)
{
    /*errs() << "P: " << p << "\t";
    ACTFinal[p].DebugACT();*/
}

void ACT::DebugACT()
{
    /*errs() << act_map.size() << "\n";
    errs() << "currNode: " << &currNode ;
    if (currNode != act_map.end()) 
    {
        errs() << "\t" << *currNode << "\n";
        if (*currNode != NULL)
        {
            errs() << "key: " << (*currNode)->key << "\n";
        }
    }
    else {errs() << "\n";}*/
}

/*ACTNode* ACT::get_node(uint64_t key)
{
    auto it = currNode;
    ACTNode* ret = NULL;
    if (act_map.empty()) return NULL;
    if (it == act_map.end() ||
        (*it)->key < key) it = act_map.begin();
    while (it != act_map.end())
    {
        if ((*it)->key == key) {ret = *it; break;}
        if ((*it)->key > key) {break;}
        ++it;
    }
    if (ret != NULL) currNode = it;
    
    return ret;
}*/
