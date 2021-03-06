AliXiStarPbPb *AddTaskXiStarPbPb(bool AODcase=kFALSE, bool MCcase=kFALSE, int CutList=0) {
 
  //===========================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    ::Error("AddTaskXiStar", "No analysis manager to connect to.");
    return NULL;
  }
  
    if (AODcase == kTRUE) {
        Printf("INFO! You are using AODs!");}

  //____________________________________________//
  // Create tasks
  AliXiStarPbPb *XiStarTask = new AliXiStarPbPb("XiStarTask", AODcase, MCcase, CutList);
  if(!XiStarTask) exit(-1);
    
  mgr->AddTask(XiStarTask);
    
   
  // Create ONLY the output containers for the data produced by the task.
  // Get and connect other common input/output containers via the manager as below
  //==============================================================================
  TString outputFileName = AliAnalysisManager::GetCommonFileName();
  outputFileName += ":PWGLF.outputXiStarAnalysis.root";
  AliAnalysisDataContainer *coutXiStar = mgr->CreateContainer("XiStarOutput", TList::Class(),AliAnalysisManager::kOutputContainer,outputFileName.Data());
  mgr->ConnectInput(XiStarTask, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(XiStarTask, 1, coutXiStar);
  
  
  // Return the task pointer
  return XiStarTask;
}
