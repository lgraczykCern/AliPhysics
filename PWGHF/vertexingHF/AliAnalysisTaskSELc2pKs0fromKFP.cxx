/**************************************************************************
 * Copyright(c) 1998-2019, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/////////////////////////////////////////////////////////////
// Author: Jianhui Zhu (1,2)
// (1) Central China Normal University
// (2) GSI Helmholtz Centre for Heavy Ion Research
// E-mail: zjh@mail.ccnu.edu.cn
/////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <TDatabasePDG.h>
#include <vector>
#include <TVector3.h>
#include "TChain.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLine.h"
#include "TList.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliESDtrack.h"
#include "AliCentrality.h"
#include "AliNormalizationCounter.h"
#include "AliAnalysisTaskSELc2pKs0fromKFP.h"
#include "AliPIDResponse.h"
#include "AliAODHandler.h"
#include "AliAODMCHeader.h"
#include "AliAnalysisVertexingHF.h"

#include "AliAODMCParticle.h"

// includes added to play with KFParticle
#ifndef HomogeneousField
#define HomogeneousField 
#endif

class AliAnalysisTaskSELc2pKs0fromKFP;    // your analysis class

ClassImp(AliAnalysisTaskSELc2pKs0fromKFP) // classimp: necessary for root

AliAnalysisTaskSELc2pKs0fromKFP::AliAnalysisTaskSELc2pKs0fromKFP() :
  AliAnalysisTaskSE(),
  fIsMC(kFALSE),
  fPID(0),
  fAnaCuts(0),
  fpVtx(0),
  fMCEvent(0),
  fBzkG(0),
  fCentrality(0),
  fAodTrackInd(0),
  fOutputList(0),
  fOutputWeight(0),
  fListCuts(0),
  fTree_Event(0),
  fVar_Event(0),
  fTree_Lc(0),
  fVar_Lc(0),
  fTree_Lc_QA(0),
  fVar_Lc_QA(0),
  fTree_LcMCGen(0),
  fVar_LcMCGen(0),
  fCounter(0),
  fHistEvents(0),
  fHTrigger(0),
  fWriteLcTree(kFALSE),
  fWriteLcMCGenTree(kFALSE),
  fWriteLcQATree(kFALSE),
  fWeight(0),
  fHistMCGen_LcPt_weight(0),
  f2DHistMCRec_LcPt_weight(0)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
AliAnalysisTaskSELc2pKs0fromKFP::AliAnalysisTaskSELc2pKs0fromKFP(const char* name, AliRDHFCutsKFP* cuts) :
  AliAnalysisTaskSE(name),
  fIsMC(kFALSE),
  fPID(0),
  fAnaCuts(cuts),
  fpVtx(0),
  fMCEvent(0),
  fBzkG(0),
  fCentrality(0),
  fAodTrackInd(0),
  fOutputList(0),
  fOutputWeight(0),
  fListCuts(0),
  fTree_Event(0),
  fVar_Event(0),
  fTree_Lc(0),
  fVar_Lc(0),
  fTree_Lc_QA(0),
  fVar_Lc_QA(0),
  fTree_LcMCGen(0),
  fVar_LcMCGen(0),
  fCounter(0),
  fHistEvents(0),
  fHTrigger(0),
  fWriteLcTree(kFALSE),
  fWriteLcMCGenTree(kFALSE),
  fWriteLcQATree(kFALSE),
  fWeight(0),
  fHistMCGen_LcPt_weight(0),
  f2DHistMCRec_LcPt_weight(0)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
  DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
  DefineOutput(2, AliNormalizationCounter::Class());
  DefineOutput(3, TTree::Class()); // event
  DefineOutput(4, TTree::Class()); // Lc
  DefineOutput(5, TTree::Class()); // Lc MCGen
  DefineOutput(6, TList::Class()); // Lc weight of MC pt shape
  DefineOutput(7, TTree::Class()); // Lc QA

}
//_____________________________________________________________________________
AliAnalysisTaskSELc2pKs0fromKFP::~AliAnalysisTaskSELc2pKs0fromKFP()
{
    // destructor
    if (fOutputList) {
      delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
      fOutputList = 0;
    }

    if (fOutputWeight) {
      delete fOutputWeight;     // at the end of your task, it is deleted from memory by calling this function
      fOutputWeight = 0;
    }

    if (fListCuts) {
      delete fListCuts;
      fListCuts = 0;
    }

    if (fAnaCuts) {
      delete fAnaCuts;
      fAnaCuts = 0;
    }

    if (fTree_Event) {
      delete fTree_Event;
      fTree_Event = 0;
    }

    if (fVar_Event) {
      delete fVar_Event;
      fVar_Event = 0;
    }

    if (fTree_Lc) {
      delete fTree_Lc;
      fTree_Lc = 0;
    }

    if (fVar_Lc) {
      delete fVar_Lc;
      fVar_Lc = 0;
    }

    if (fTree_Lc_QA) {
      delete fTree_Lc_QA;
      fTree_Lc_QA = 0;
    }

    if (fVar_Lc_QA) {
      delete fVar_Lc_QA;
      fVar_Lc_QA = 0;
    }

    if (fTree_LcMCGen) {
      delete fTree_LcMCGen;
      fTree_LcMCGen = 0;
    }

    if (fVar_LcMCGen) {
      delete fVar_LcMCGen;
      fVar_LcMCGen = 0;
    }

    if (fCounter) {
      delete fCounter;
      fCounter = 0;
    }


}
//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::Init()
{
  // Initialization

  if (fDebug > 1) AliInfo("Init");

  fListCuts = new TList();
  fListCuts->SetOwner();
  fListCuts->SetName("ListCuts");
  fListCuts->Add(new AliRDHFCutsKFP(*fAnaCuts));
  PostData(1, fListCuts);

  return;

}
//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use 
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
  fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
  fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

  DefineAnaHist(); // define analysis histograms

    // example of a histogram
  fHistEvents = new TH1F("fHistEvents", "fHistEvents", 18, 0.5, 18.5);
  fHistEvents->GetXaxis()->SetBinLabel(1,"Analyzed events");
  fHistEvents->GetXaxis()->SetBinLabel(2,"AliAODVertex exists");
  fHistEvents->GetXaxis()->SetBinLabel(3,"TriggerOK");
  fHistEvents->GetXaxis()->SetBinLabel(4,"IsEventSelected");
  fHistEvents->GetXaxis()->SetBinLabel(5,"V0 exists");
  fHistEvents->GetXaxis()->SetBinLabel(6,"Cascade exists");

  fHistEvents->GetXaxis()->SetBinLabel(7,"MCarray exists");
  fHistEvents->GetXaxis()->SetBinLabel(8,"MCheader exists");
  fHistEvents->GetXaxis()->SetBinLabel(9,"triggerClass!=CINT1");
  fHistEvents->GetXaxis()->SetBinLabel(10,"triggerMask!=kAnyINT");
  fHistEvents->GetXaxis()->SetBinLabel(11,"triggerMask!=kAny");
  fHistEvents->GetXaxis()->SetBinLabel(12,"vtxTitle.Contains(Z)");
  fHistEvents->GetXaxis()->SetBinLabel(13,"vtxTitle.Contains(3D)");
  fHistEvents->GetXaxis()->SetBinLabel(14,"vtxTitle.Doesn'tContain(Z-3D)");
  fHistEvents->GetXaxis()->SetBinLabel(15,Form("zVtx<=%2.0fcm", fAnaCuts->GetMaxVtxZ()));
  fHistEvents->GetXaxis()->SetBinLabel(16,"!IsEventSelected");
  fHistEvents->GetXaxis()->SetBinLabel(17,"triggerMask!=kAnyINT || triggerClass!=CINT1");
  fHistEvents->GetXaxis()->SetBinLabel(18,Form("zVtxMC<=%2.0fcm",fAnaCuts->GetMaxVtxZ()));
  fHistEvents->GetYaxis()->SetTitle("counts");

  fHTrigger = new TH1F("fHTrigger", "counter", 18, -0.5, 17.5);                                      
  fHTrigger->SetStats(kTRUE);
  fHTrigger->GetXaxis()->SetBinLabel(1,"X1");
  fHTrigger->GetXaxis()->SetBinLabel(2,"kMB");
  fHTrigger->GetXaxis()->SetBinLabel(3,"kSemiCentral");
  fHTrigger->GetXaxis()->SetBinLabel(4,"kCentral");
  fHTrigger->GetXaxis()->SetBinLabel(5,"kINT7");
  fHTrigger->GetXaxis()->SetBinLabel(6,"kEMC7");
  //fHTrigger->GetXaxis()->SetBinLabel(7,"Space");
  fHTrigger->GetXaxis()->SetBinLabel(8,"kMB|kSemiCentral|kCentral");
  fHTrigger->GetXaxis()->SetBinLabel(9,"kINT7|kEMC7");
  fHTrigger->GetXaxis()->SetBinLabel(11,"kMB&kSemiCentral");
  fHTrigger->GetXaxis()->SetBinLabel(12,"kMB&kCentral");
  fHTrigger->GetXaxis()->SetBinLabel(13,"kINT7&kEMC7");

  fOutputList->Add(fHistEvents); // don't forget to add it to the list! the list will be written to file, so if you want
  fOutputList->Add(fHTrigger);

  // Counter for Normalization
  TString normName="NormalizationCounter";
  AliAnalysisDataContainer *cont = GetOutputSlot(2)->GetContainer();
  if(cont) normName = (TString)cont->GetName();
  fCounter = new AliNormalizationCounter(normName.Data());
  fCounter->Init();
  PostData(2, fCounter);
  DefineEvent();
  PostData(3, fTree_Event);  // postdata will notify the analysis manager of changes / updates to the 

  DefineTreeLc_Rec();
  PostData(4, fTree_Lc);

  DefineTreeLc_Gen();
  PostData(5, fTree_LcMCGen);

  fOutputWeight = new TList();
  fOutputWeight->SetOwner(kTRUE);
  fHistMCGen_LcPt_weight = new TH1D("fHistMCGen_LcPt_weight", "", 11, 1., 12.);
  f2DHistMCRec_LcPt_weight = new TH2D("f2DHistMCRec_LcPt_weight", "", 11, 1., 12., 495, 0.5, 50);
  fOutputWeight->Add(fHistMCGen_LcPt_weight);
  fOutputWeight->Add(f2DHistMCRec_LcPt_weight);
  PostData(6, fOutputWeight);

  DefineTreeLc_Rec_QA();
  PostData(7, fTree_Lc_QA);

  return;
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::UserExec(Option_t *)
{
  // user exec
  // this function is called once for each event
  // the manager will take care of reading the events from file, and with the static function InputEvent() you 
  // have access to the current event. 
  // once you return from the UserExec function, the manager will retrieve the next event from the chain

  if (!fInputEvent) { // if the event is empty (getting it failed) skip this event
    AliError("NO EVENT FOUND!");
    return;
  }
  AliAODEvent* aodEvent = dynamic_cast<AliAODEvent*>(fInputEvent);    // get an event (called AODEvent) from the input file
                                                        // there's another event format (ESD) which works in a similar way
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's

  fHistEvents->Fill(1);

  
  TClonesArray *arrayLc2pKs0 = NULL;
  if (!aodEvent && AODEvent() && IsStandardAOD()) {
    // In case there is an AOD handler writing a standard AOD, use the AOD
    // event in memory rather than the input (ESD) event.
    aodEvent = dynamic_cast<AliAODEvent*> (AODEvent());
    // in this case the braches in the deltaAOD (AliAOD.VertexingHF.root)
    // have to taken from the AOD event hold by the AliAODExtension
    AliAODHandler *aodHandler = (AliAODHandler*)((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler());
    if (aodHandler->GetExtensions()) {
      AliAODExtension *ext = (AliAODExtension*)aodHandler->GetExtensions()->FindObject("AliAOD.VertexingHF.root");
      AliAODEvent *aodFromExt = ext->GetAOD();
      arrayLc2pKs0 = (TClonesArray*)aodFromExt->GetList()->FindObject("CascadesHF");
    }
  } else {
    arrayLc2pKs0 = (TClonesArray*)aodEvent->GetList()->FindObject("CascadesHF");
  }


  //--------------------------------------------------------------
  // First check if the event has magnetic field and proper vertex
  //--------------------------------------------------------------

  fBzkG = (Double_t)aodEvent->GetMagneticField();
  if (TMath::Abs(fBzkG)<0.001) return;
  // Setting magnetic field for KF vertexing
  KFParticle::SetField(fBzkG);

  fpVtx = (AliAODVertex*)aodEvent->GetPrimaryVertex();
  if (!fpVtx) return;
  fHistEvents->Fill(2);

  fCounter->StoreEvent(aodEvent,fAnaCuts,fIsMC);

  //------------------------------------------------
  // MC analysis setting                                                                    
  //------------------------------------------------

  TClonesArray *mcArray = 0;
  AliAODMCHeader *mcHeader = 0;

  if (fIsMC) {
    fMCEvent = MCEvent(); // get the corresponding MC event fMCEvent
    if (!fMCEvent) {
      Printf("ERROR: Could not retrieve MC event");
      return;
    }

    // MC array need for maching
    mcArray = dynamic_cast<TClonesArray*>(aodEvent->FindListObject(AliAODMCParticle::StdBranchName()));
    if ( !mcArray ) {
      AliError("Could not find Monte-Carlo in AOD");
      return;
    }
    fHistEvents->Fill(7); // number of MC array exist

    // load MC header
    mcHeader = (AliAODMCHeader*)aodEvent->GetList()->FindObject(AliAODMCHeader::StdBranchName());
    if ( !mcHeader ) {
      AliError("AliAnalysisTaskSELc2pKs0fromKFP::UserExec: MC header branch not found!\n");
      return;
    }
    fHistEvents->Fill(8); // number of MC header exist

    Double_t zMCvtx = mcHeader->GetVtxZ();
    if ( TMath::Abs(zMCvtx) > fAnaCuts->GetMaxVtxZ() ) {
      AliDebug(2,Form("Event rejected: fabs(zVtxMC)=%f > fAnaCuts->GetMaxVtxZ()=%f", zMCvtx, fAnaCuts->GetMaxVtxZ()));
      return;
    } else {
      fHistEvents->Fill(18);
    }
    if ((TMath::Abs(zMCvtx) < fAnaCuts->GetMaxVtxZ()) && (!fAnaCuts->IsEventRejectedDuePhysicsSelection()) && (!fAnaCuts->IsEventRejectedDueToTrigger())) {
      Bool_t selevt = MakeMCAnalysis(mcArray);
      if(!selevt) return;
    }
  }


  //------------------------------------------------
  // Event selection
  //------------------------------------------------
  Bool_t IsTriggerNotOK = fAnaCuts->IsEventRejectedDueToTrigger();
  Bool_t IsPhysSelNotOK = fAnaCuts->IsEventRejectedDuePhysicsSelection();
  Bool_t IsNoVertex = fAnaCuts->IsEventRejectedDueToNotRecoVertex();
  if( !IsTriggerNotOK && !IsPhysSelNotOK && !IsNoVertex && fabs(fpVtx->GetZ())<fAnaCuts->GetMaxVtxZ() ) fHistEvents->Fill(3);

  Bool_t IsEventSelected = fAnaCuts->IsEventSelected(aodEvent);
  if(!IsEventSelected) {
//    cout<<"Why: "<<fAnaCuts->GetWhyRejection()<<endl;
    return;
  }
  fHistEvents->Fill(4);


  Bool_t IsMB = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected()&AliVEvent::kMB)==(AliVEvent::kMB);
  Bool_t IsSemi = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected()&AliVEvent::kSemiCentral)==(AliVEvent::kSemiCentral);
  Bool_t IsCent = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected()&AliVEvent::kCentral)==(AliVEvent::kCentral);
  Bool_t IsINT7 = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected()&AliVEvent::kINT7)==(AliVEvent::kINT7);
  Bool_t IsEMC7 = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected()&AliVEvent::kEMC7)==(AliVEvent::kEMC7);
  if(IsMB) fHTrigger->Fill(1);
  if(IsSemi) fHTrigger->Fill(2);
  if(IsCent) fHTrigger->Fill(3);
  if(IsINT7) fHTrigger->Fill(4);
  if(IsEMC7) fHTrigger->Fill(5);
  if(IsMB||IsSemi||IsCent) fHTrigger->Fill(7);
  if(IsINT7||IsEMC7) fHTrigger->Fill(8);
  if(IsMB&&IsSemi) fHTrigger->Fill(10);
  if(IsMB&&IsCent) fHTrigger->Fill(11);
  if(IsINT7&&IsEMC7) fHTrigger->Fill(12);

//  AliCentrality *cent = aodEvent->GetCentrality();
//  Float_t Centrality = cent->GetCentralityPercentile("V0M");

  //------------------------------------------------
  // Check if the event has v0 candidate
  //------------------------------------------------
  Int_t num_v0 = aodEvent->GetNumberOfV0s();
  if (num_v0>0) fHistEvents->Fill(5);

  //------------------------------------------------
  // Check if the event has cascade candidate
  //------------------------------------------------
  Int_t num_casc = aodEvent->GetNumberOfCascades();
  if (num_casc<=0) return;
  fHistEvents->Fill(6);

  // set primary vertex
  KFPVertex pVertex;
  Double_t pos[3],cov[6];
  fpVtx->GetXYZ(pos);
  if ( fabs(pos[2])>10. ) return; // vertex cut on z-axis direction
  fpVtx->GetCovarianceMatrix(cov);
//  if ( !AliVertexingHFUtils::CheckAODvertexCov(fpVtx) ) cout << "Vertex Cov. is wrong!!!" << endl;
  pVertex.SetXYZ((Float_t)pos[0], (Float_t)pos[1], (Float_t)pos[2]);
  Float_t covF[6];
  for (Int_t i=0; i<6; i++) { covF[i] = (Float_t)cov[i]; }
  pVertex.SetCovarianceMatrix(covF);
  pVertex.SetChi2(fpVtx->GetChi2());
  pVertex.SetNDF(fpVtx->GetNDF());
  pVertex.SetNContributors(fpVtx->GetNContributors());

  KFParticle PV(pVertex);

  if(!fAnaCuts) return;

  FillEventROOTObjects();

//------------------------------------------------
// Main analysis done in this function
//------------------------------------------------
  
  fPID = fInputHandler->GetPIDResponse();
  MakeAnaLcFromCascadeHF(arrayLc2pKs0, aodEvent, mcArray, PV);

  PostData(2, fCounter);
  PostData(3, fTree_Event);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
  PostData(4, fTree_Lc);
  PostData(5, fTree_LcMCGen);

  return;
}
//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
/*
    TCanvas *c1 = new TCanvas();
    fHistDecayLLambda->Draw();
    TLine *lLPDG = new TLine(7.89, 0, 7.89, 1e10);
    lLPDG->SetLineColor(2);
    lLPDG->Draw();

    TCanvas *c2 = new TCanvas();
    fHistDecayLXiMinus->Draw();
    TLine *lXiPDG = new TLine(4.91, 0, 4.91, 1e10);
    lXiPDG->SetLineColor(2);
    lXiPDG->Draw();
*/
    return;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskSELc2pKs0fromKFP::MakeMCAnalysis(TClonesArray *mcArray)
{

  // method to fill MC histo: how many Lc --> Ks0 + p are there at MC level
  for (Int_t iPart=0; iPart<mcArray->GetEntriesFast(); iPart++) {
    AliAODMCParticle *mcPart = dynamic_cast<AliAODMCParticle*>(mcArray->At(iPart));
    if (!mcPart) {
      AliError("Failed casting particle from MC array!, Skipping particle");
      continue;
    }
    Int_t pdg = mcPart->GetPdgCode();
    if ( TMath::Abs(pdg)!=4122 ) {
      AliDebug(2, Form("MC particle %d is not a Lc: its pdg code is %d", iPart, pdg));
      continue;
    }
    AliDebug(2, Form("Step 0 ok: MC particle %d is a Lc: its pdg code is %d", iPart, pdg));
    Int_t labeldaugh0 = mcPart->GetDaughterLabel(0);
    Int_t labeldaugh1 = mcPart->GetDaughterLabel(1);
    if (labeldaugh0 <= 0 || labeldaugh1 <= 0){
      AliDebug(2, Form("The MC particle doesn't have correct daughters, skipping!!"));
      continue;
    }
    else if ( (labeldaugh1 - labeldaugh0) == 1 ) {
      AliDebug(2, Form("Step 1 ok: The MC particle has correct daughters!!"));
      AliAODMCParticle* daugh0 = dynamic_cast<AliAODMCParticle*>(mcArray->At(labeldaugh0));
      AliAODMCParticle* daugh1 = dynamic_cast<AliAODMCParticle*>(mcArray->At(labeldaugh1));
      if ( !daugh0 || !daugh1 ) {
        AliDebug(2, "Particle daughters not properly retrieved!");
        continue;
      }
      Int_t pdgCodeDaugh0 = daugh0->GetPdgCode();
      Int_t pdgCodeDaugh1 = daugh1->GetPdgCode();
      AliAODMCParticle* bachelorMC = daugh0;
      AliAODMCParticle* v0MC = daugh1;
      if ( TMath::Abs(pdgCodeDaugh0)==2212 || TMath::Abs(pdgCodeDaugh1)==2212 ) {
        AliDebug(1, Form("pdgCodeDaugh0 = %d, pdgCodeDaugh1 = %d", pdgCodeDaugh0, pdgCodeDaugh1));
      }
      if ( (TMath::Abs(pdgCodeDaugh0)==311 && TMath::Abs(pdgCodeDaugh1)==2212) || (TMath::Abs(pdgCodeDaugh0)==2212 && TMath::Abs(pdgCodeDaugh1)==311) ) {
        if ( TMath::Abs(pdgCodeDaugh0)==311 ) {
          bachelorMC = daugh1;
          v0MC = daugh0;
        }
        if ( v0MC->GetNDaughters()!=1 ) {
          AliDebug(2, "The K0 does not decay in 1 body only! Impossible... Continuing...");
          continue;
        }
        AliDebug(2, "Step 2 ok: The K0 does decay in 1 body only! ");
        Int_t labelK0daugh = v0MC->GetDaughterLabel(0);
        AliAODMCParticle* partK0S = dynamic_cast<AliAODMCParticle*>(mcArray->At(labelK0daugh));
        if ( !partK0S ) {
          AliError("Error while casting particle! returning a NULL array");
          continue;
        }
//        cout << "=== Lc" << pdg << endl;
//        cout << "=== pdgCodeDaugh0 " << pdgCodeDaugh0 << endl;
//        cout << "=== pdgCodeDaugh1 " << pdgCodeDaugh1 << endl;
//        cout << "=== K0 " << partK0S->GetPdgCode() << endl;
        if ( TMath::Abs(partK0S->GetPdgCode()) != 310 ) {
          AliDebug(2, "The K0 daughter is not a K0S");
          continue;
        }
        //=== check K0S decay ===
        if ( partK0S->GetNDaughters() != 2 ) {
          AliDebug(2, "The K0S does not decay in 2 bodies");
          continue;
        }
        AliDebug(2, "Step 3 ok: The K0 daughter is a K0S and does decay in 2 bodies");
        Int_t labelK0Sdaugh0 = partK0S->GetDaughterLabel(0);
        Int_t labelK0Sdaugh1 = partK0S->GetDaughterLabel(1);
        AliAODMCParticle* daughK0S0 = dynamic_cast<AliAODMCParticle*>(mcArray->At(labelK0Sdaugh0));
        AliAODMCParticle* daughK0S1 = dynamic_cast<AliAODMCParticle*>(mcArray->At(labelK0Sdaugh1));
        if ( !daughK0S0 || !daughK0S1 ) {
          AliDebug(2, "Could not access K0S daughters, continuing...");
          continue;
        }
        AliDebug(2, "Step 4 ok: Could access K0S daughters, continuing...");
        Int_t pdgK0Sdaugh0 = daughK0S0->GetPdgCode();
        Int_t pdgK0Sdaugh1 = daughK0S1->GetPdgCode();
        if ( TMath::Abs(pdgK0Sdaugh0)!=211 || TMath::Abs(pdgK0Sdaugh1)!=211 ) {
          AliDebug(2, "The K0S does not decay in pi+pi-, continuing");
          continue;
        }
        if ( TMath::Abs(mcPart->Y()) < 0.8 ) {
          Int_t CheckOrigin = AliVertexingHFUtils::CheckOrigin(mcArray,mcPart,kTRUE);
          FillTreeGenLc(mcPart, CheckOrigin);
        }
      }
    }
  }

  return kTRUE;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::FillTreeGenLc(AliAODMCParticle *mcpart, Int_t CheckOrigin)
{
  // Fill histograms or tree depending

  if(!mcpart) return;

  for(Int_t i=0;i<4;i++){
    fVar_LcMCGen[i] = -9999.;
  }

  fVar_LcMCGen[ 0] = fCentrality;
//  if (mcpart->IsPrimary() && (!mcpart->IsPhysicalPrimary())) fVar_LcMCGen[1] = 1;
//  if (mcpart->IsPhysicalPrimary()) fVar_LcMCGen[1] = 2;
//  if (mcpart->IsSecondaryFromWeakDecay()) fVar_LcMCGen[1] = 3;
//  if (mcpart->IsSecondaryFromMaterial()) fVar_LcMCGen[1] = 4;
//  if (mcpart->IsFromSubsidiaryEvent()) fVar_LcMCGen[1] = 5;
  fVar_LcMCGen[ 1] = mcpart->Y();
  fVar_LcMCGen[ 2] = mcpart->Pt();
  fVar_LcMCGen[ 3] = CheckOrigin;

  if (fWriteLcMCGenTree) fTree_LcMCGen->Fill();

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::MakeAnaLcFromCascadeHF(TClonesArray *arrayLc2pKs0, AliAODEvent *aodEvent, TClonesArray *mcArray, KFParticle PV)
{
  // Main analysis called from "UserExec"

//  std::cout.setf(std::ios::fixed);
//  std::cout.setf(std::ios::showpoint);
//  std::cout.precision(3);

  UInt_t nCasc = arrayLc2pKs0->GetEntriesFast();
  AliAnalysisVertexingHF *vHF = new AliAnalysisVertexingHF();
  Double_t covP[21], covN[21], covB[21];
  const Int_t NDaughters = 2;
  const Float_t massKs0_PDG = TDatabasePDG::Instance()->GetParticle(310)->Mass();
  const Float_t massLc_PDG  = TDatabasePDG::Instance()->GetParticle(4122)->Mass();
  const Float_t massLam_PDG = TDatabasePDG::Instance()->GetParticle(3122)->Mass();

  for (UInt_t iCasc=0; iCasc<nCasc; iCasc++) {
    // Lc candidates
    AliAODRecoCascadeHF *Lc2pKs0 = dynamic_cast<AliAODRecoCascadeHF*>(arrayLc2pKs0->At(iCasc));
    if (!Lc2pKs0) {
      AliDebug(2,Form("Cascade %d doens't exist, skipping",iCasc));
      continue;
    }

    // --- Needed for newer samples ---
    if (!vHF->FillRecoCasc(aodEvent, Lc2pKs0, kFALSE)) { // Fill data members of candidate if not done
      AliDebug(2,Form("Cascade %d not refilled, skipping",iCasc));
      continue;
    }
    // --------------------------------

    if (!(Lc2pKs0->CheckCascadeFlags())) {
      AliDebug(2,Form("Cascade %d is not flagged as Lc candidate",iCasc));
      continue;
    }

    if (Lc2pKs0->GetNDaughters()!=2) {
      AliDebug(2,Form("Cascade %d does not have 2 daughters (nDaughters=%d)",iCasc,Lc2pKs0->GetNDaughters()));
      continue;
    }

    AliAODv0 *v0part = dynamic_cast<AliAODv0*>(Lc2pKs0->Getv0());
    AliAODTrack *bachPart = dynamic_cast<AliAODTrack*>(Lc2pKs0->GetBachelor());
    if (!v0part || !bachPart) {
      AliDebug(2,Form("Cascade %d has no V0 or no bachelor object",iCasc));
      continue;
    }

    // primary track cuts
//    if ( !fAnaCuts->PassedTrackQualityCuts_Primary(bachPart) ) continue;

    if (!v0part->GetSecondaryVtx()) {
      AliDebug(2,Form("No secondary vertex for V0 by cascade %d",iCasc));
      continue;
    }

    if (v0part->GetNDaughters()!=2) {
      AliDebug(2,Form("current V0 does not have 2 daughters (onTheFly=%d, nDaughters=%d)",v0part->GetOnFlyStatus(),v0part->GetNDaughters()));
      continue;
    }

    AliAODTrack * v0Pos = dynamic_cast<AliAODTrack*>(Lc2pKs0->Getv0PositiveTrack());
    AliAODTrack * v0Neg = dynamic_cast<AliAODTrack*>(Lc2pKs0->Getv0NegativeTrack());
    if (!v0Pos || !v0Neg) {
      AliDebug(2,Form("V0 by cascade %d has no V0positive or V0negative object",iCasc));
      continue;
    }

    // check charge of v0
    Int_t v0charge = v0Pos->Charge() + v0Neg->Charge();
    if (v0charge!=0) {
      AliDebug(2,Form("V0 by cascade %d has charge: IMPOSSIBLE!",iCasc));
      continue;
    }
    
    // check charge of the first daughter, if negative, define it as the second one
    if (v0Pos->Charge()<0) {
      v0Pos = (AliAODTrack*) (Lc2pKs0->Getv0NegativeTrack());
      v0Neg = (AliAODTrack*) (Lc2pKs0->Getv0PositiveTrack());
    }

    // === pre-selection for Lc ===
    if ( !fAnaCuts->PreSelForLc2pKs0(Lc2pKs0) ) continue;

    if ( fabs(v0part->MassLambda()-massLam_PDG) <= fAnaCuts->GetProdMassTolLambda() ) continue;
    if ( fabs(v0part->MassAntiLambda()-massLam_PDG) <= fAnaCuts->GetProdMassTolLambda() ) continue;
//    if ( v0part->InvMass2Prongs(0,1,11,11) <= fAnaCuts->GetMassCutEplusEminus() ) continue; // InvMass of e+e-
    if ( v0part->InvMass2Prongs(0,1,11,11) <= 0.1 ) continue; // InvMass of e+e-
    // ============================

    if ( !bachPart->GetCovarianceXYZPxPyPz(covB) || !v0Pos->GetCovarianceXYZPxPyPz(covP) || !v0Neg->GetCovarianceXYZPxPyPz(covN) ) continue;
    if ( !AliVertexingHFUtils::CheckAODtrackCov(bachPart) || !AliVertexingHFUtils::CheckAODtrackCov(v0Pos) || !AliVertexingHFUtils::CheckAODtrackCov(v0Neg) ) continue;
    
    KFParticle kfpProton;
    if (bachPart->Charge()>0) kfpProton = AliVertexingHFUtils::CreateKFParticleFromAODtrack(bachPart, 2212);
    if (bachPart->Charge()<0) kfpProton = AliVertexingHFUtils::CreateKFParticleFromAODtrack(bachPart, -2212);

    KFParticle kfpPionPlus   = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Pos, 211);
    KFParticle kfpPionMinus  = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Neg, -211);

    KFParticle kfpKs0;
    const KFParticle *Ks0Daughters[2] = {&kfpPionPlus, &kfpPionMinus};
    kfpKs0.Construct(Ks0Daughters, NDaughters);
    Float_t massKs0_rec=0., err_massKs0_rec=0.;
    kfpKs0.GetMass(massKs0_rec, err_massKs0_rec);

    // check rapidity of Ks0
    if ( TMath::Abs(kfpKs0.GetE())<=TMath::Abs(kfpKs0.GetPz()) ) continue;

    // chi2>0 && NDF>0 for selecting Ks0
    if ( (kfpKs0.GetNDF()<=1.e-10 || kfpKs0.GetChi2()<=1.e-10) ) continue;

    // check cov. of Ks0
    if ( !AliVertexingHFUtils::CheckKFParticleCov(kfpKs0) ) continue;

    // err_mass(Ks0) > 0
    if ( err_massKs0_rec<=1.e-10 ) continue;

    // Chi2geo cut of Ks0
    if ( (kfpKs0.GetChi2()/kfpKs0.GetNDF()) >= fAnaCuts->GetKFPKs0_Chi2geoMax() ) continue;

    // ================ calculate l/Δl for Ks0 ===============
//    Double_t dx_Ks0 = PV.GetX()-kfpKs0.GetX();
//    Double_t dy_Ks0 = PV.GetY()-kfpKs0.GetY();
//    Double_t dz_Ks0 = PV.GetZ()-kfpKs0.GetZ();
//    Double_t l_Ks0 = TMath::Sqrt(dx_Ks0*dx_Ks0 + dy_Ks0*dy_Ks0 + dz_Ks0*dz_Ks0);
//    Double_t dl_Ks0 = (PV.GetCovariance(0)+kfpKs0.GetCovariance(0))*dx_Ks0*dx_Ks0 + (PV.GetCovariance(2)+kfpKs0.GetCovariance(2))*dy_Ks0*dy_Ks0 + (PV.GetCovariance(5)+kfpKs0.GetCovariance(5))*dz_Ks0*dz_Ks0 + 2*( (PV.GetCovariance(1)+kfpKs0.GetCovariance(1))*dx_Ks0*dy_Ks0 + (PV.GetCovariance(3)+kfpKs0.GetCovariance(3))*dx_Ks0*dz_Ks0 + (PV.GetCovariance(4)+kfpKs0.GetCovariance(4))*dy_Ks0*dz_Ks0 );
//    if ( fabs(l_Ks0)<1.e-8f ) l_Ks0 = 1.e-8f;
//    dl_Ks0 = dl_Ks0<0. ? 1.e8f : sqrt(dl_Ks0)/l_Ks0;
//    Double_t ldl_Ks0 = l_Ks0/dl_Ks0;
    Double_t ldl_Ks0 = AliVertexingHFUtils::ldlFromKF(kfpKs0, PV);
    // =======================================================

    // l/Deltal cut of Ks0
    if ( ldl_Ks0 <= fAnaCuts->GetKFPKs0_lDeltalMin() ) continue;

    // mass window cut of Ks0
    if ( TMath::Abs(massKs0_rec-massKs0_PDG) > (fAnaCuts->GetProdMassTolKs0()) ) continue;
    
    // mass constraint for Ks0
    KFParticle kfpKs0_massConstraint = kfpKs0;
    kfpKs0_massConstraint.SetNonlinearMassConstraint(massKs0_PDG);

    // QA check after mass constraint
    if ( !AliVertexingHFUtils::CheckKFParticleCov(kfpKs0_massConstraint) || TMath::Abs(kfpKs0_massConstraint.GetE()) <= TMath::Abs(kfpKs0_massConstraint.GetPz()) ) continue;

    // reconstruct Lc with mass constraint
    KFParticle kfpLc;
    const KFParticle *LcDaughters[2] = {&kfpProton, &kfpKs0_massConstraint};
    kfpLc.Construct(LcDaughters, NDaughters);

    // reconstruct Lc without mass constraint
    KFParticle kfpLc_woKs0MassConst;
    const KFParticle *LcDaughters_woKs0MassConst[2] = {&kfpProton, &kfpKs0};
    kfpLc_woKs0MassConst.Construct(LcDaughters_woKs0MassConst, NDaughters);

    // === for Lc with mass constraint ===
    // check rapidity of Lc
    if ( TMath::Abs(kfpLc.GetE())<=TMath::Abs(kfpLc.GetPz()) ) continue;

    // chi2>0 && NDF>0
    if ( kfpLc.GetNDF()<=1.e-10 || kfpLc.GetChi2()<=1.e-10 ) continue;

    // check covariance matrix
    if ( !AliVertexingHFUtils::CheckKFParticleCov(kfpLc) ) continue;

    // Prefilter
    if ( kfpLc.GetChi2()/kfpLc.GetNDF() >= fAnaCuts->GetKFPLc_Chi2geoMax() ) continue;
    if ( kfpLc.GetPt() < fAnaCuts->GetPtMinLc() ) continue;
    
    // err_mass(Lc) > 0
    Float_t massLc_rec=0., err_massLc_rec=0.;
    kfpLc.GetMass(massLc_rec, err_massLc_rec);
    if (err_massLc_rec <= 1.e-10 ) continue;
    // ===================================

    // === for Lc without mass constraint ===
    // check rapidity of Lc
    if ( TMath::Abs(kfpLc_woKs0MassConst.GetE())<=TMath::Abs(kfpLc_woKs0MassConst.GetPz()) ) continue;

    // chi2>0 && NDF>0
    if ( kfpLc_woKs0MassConst.GetNDF()<=1.e-10 || kfpLc_woKs0MassConst.GetChi2()<=1.e-10 ) continue;

    // check covariance matrix
    if ( !AliVertexingHFUtils::CheckKFParticleCov(kfpLc_woKs0MassConst) ) continue;

    // Prefilter
    if ( kfpLc_woKs0MassConst.GetChi2()/kfpLc_woKs0MassConst.GetNDF() >= fAnaCuts->GetKFPLc_Chi2geoMax() ) continue;
    if ( kfpLc_woKs0MassConst.GetPt() < fAnaCuts->GetPtMinLc() ) continue;
    
    // err_mass(Lc) > 0
    Float_t massLc_woKs0MassConst_rec=0., err_massLc_woKs0MassConst_rec=0.;
    kfpLc_woKs0MassConst.GetMass(massLc_woKs0MassConst_rec, err_massLc_woKs0MassConst_rec);
    if (err_massLc_woKs0MassConst_rec <= 1.e-10 ) continue;
    // ===================================


    if (fWriteLcTree) {
      Int_t lab_Lc  = -9999;
      Int_t lab_Ks0 = -9999;
      if ( fIsMC ) {
        lab_Ks0 = MatchToMCKs0(v0Pos, v0Neg, mcArray);
        lab_Lc  = MatchToMCLc(v0Pos, v0Neg, bachPart, mcArray);
      }
      FillTreeRecLcFromCascadeHF(Lc2pKs0, kfpLc, bachPart, kfpProton, kfpKs0, kfpKs0_massConstraint, v0Pos, v0Neg, PV, mcArray, lab_Ks0, lab_Lc, kfpLc_woKs0MassConst);
    }

    kfpLc.Clear();
    kfpKs0_massConstraint.Clear();
    kfpKs0.Clear();
    kfpPionMinus.Clear();
    kfpPionPlus.Clear();
    kfpProton.Clear();
  }

  return;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::SelectTrack(AliVEvent *event, Int_t trkEntries, Int_t &nSeleTrks, Bool_t *seleFlags)
{
  // Select good tracks using fAnaCuts (AliRDHFCuts object)
  if(trkEntries==0) return;

  nSeleTrks=0;                                                                                                 
  for(Int_t i=0; i<trkEntries; i++) {
    seleFlags[i] = kFALSE;
    
    AliVTrack *track;
    track = (AliVTrack*)event->GetTrack(i);
    
//    if(track->GetID()<0) continue;
    Double_t covtest[21];
    if(!track->GetCovarianceXYZPxPyPz(covtest)) continue;
    
//    AliAODTrack *aodt = (AliAODTrack*)track;

/*
    if(!fAnaCuts) continue;
    if(fAnaCuts->SingleTrkCuts(aodt)){
      seleFlags[i]=kTRUE;
      nSeleTrks++;
//      fHistoPiPtRef->Fill(aodt->Pt());
    }
*/
  } // end loop on tracks
}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::DefineEvent()
{
  // This is to define tree variables

  const char* nameoutput = GetOutputSlot(3)->GetContainer()->GetName();
  fTree_Event = new TTree(nameoutput, "Event");
  Int_t nVar = 7;
  fVar_Event = new Float_t[nVar];
  TString *fVarNames = new TString[nVar];

  fVarNames[0]  = "centrality";
  fVarNames[1]  = "z_vtx_reco";
  fVarNames[2]  = "n_vtx_contributors";
  fVarNames[3]  = "n_tracks";
  fVarNames[4]  = "is_ev_rej";
  fVarNames[5]  = "run_number";
  fVarNames[6]  = "ev_id";

  for (Int_t ivar=0; ivar<nVar; ivar++) {
    fTree_Event->Branch(fVarNames[ivar].Data(), &fVar_Event[ivar], Form("%s/F", fVarNames[ivar].Data()));
  }

  return;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::DefineTreeLc_Rec()
{
  // This is to define tree variables

  const char* nameoutput = GetOutputSlot(4)->GetContainer()->GetName();
  fTree_Lc = new TTree(nameoutput, "Lc variables tree");
  Int_t nVar = 29;
  fVar_Lc = new Float_t[nVar];
  TString *fVarNames = new TString[nVar];

  fVarNames[0]  = "nSigmaTPC_PiPlus"; //TPC nsigma for pion+ coming from K0s
  fVarNames[1]  = "nSigmaTPC_PiMinus"; //TPC nsigma for pion- coming from K0s
  fVarNames[2]  = "nSigmaTPC_Pr"; //TPC nsigma for proton
  fVarNames[3]  = "nSigmaTOF_PiPlus"; //TOF nsigma for pion+ coming from K0s
  fVarNames[4]  = "nSigmaTOF_PiMinus"; //TOF nsigma for pion coming from K0s
  fVarNames[5]  = "nSigmaTOF_Pr"; //TOF nsigma for proton

  fVarNames[6]  = "DCA_Ks0Dau"; //Distance between pions coming from K0s (calculated from AOD v0)

  fVarNames[7]  = "chi2geo_Ks0"; //chi2_geometry of K0s (without mass constraint)
  fVarNames[8]  = "chi2topo_Ks0_PV"; //chi2_topological of K0s (with mass constraint) to PV
  fVarNames[9]  = "ldl_Ks0"; //l/dl of K0s

  fVarNames[10] = "chi2geo_Lc"; //chi2_geometry of Lc (with mass const. of Ks0)
  fVarNames[11] = "chi2topo_Lc"; //chi2_topological of Lc (with mass const. of Ks0) to PV
  fVarNames[12] = "ldl_Lc"; //l/dl of Lc (with mass const. of Ks0)

  fVarNames[13] = "DecayLxy_Ks0"; //decay length of K0s in x-y plane
  fVarNames[14] = "ct_Ks0"; // life time of Ks0
  fVarNames[15] = "DecayLxy_Lc"; //decay length of Lc in x-y plane
  fVarNames[16] = "PA_Ks0"; //pointing angle of Ks0 (pointing back to Lc)
  fVarNames[17] = "PA_Lc"; //pointing angle of Lc (pointing back to PV)

  fVarNames[18] = "pt_Ks0"; //pt of Ks0 (without mass const.)
  fVarNames[19] = "mass_Ks0"; //mass of Ks0 (without mass const.)
  fVarNames[20] = "pt_Lc"; //pt of Lc (with mass const. of Ks0 and with PV const.)
  fVarNames[21] = "rap_Lc"; //rapidity of Lc (with mass const. of Ks0 and with PV const.)
  fVarNames[22] = "mass_Lc"; //mass of Lc (with mass const. of Ks0 and with PV const.)
  fVarNames[23] = "pt_Pr"; //pt of proton
  fVarNames[24] = "pt_PiPlus"; //pt of pion+
  fVarNames[25] = "pt_PiMinus"; //pt of pion-
  fVarNames[26] = "d0_PrToPV"; //rphi impact params of proton w.r.t. Primary Vtx [cm]
  fVarNames[27] = "d0_Ks0ToPV"; //rphi impact params of Ks0 w.r.t. Primary Vtx [cm]
  fVarNames[28] = "Source_Lc"; //flag for Lc MC truth (“>=0” signal, “<0” background)

//  fVarNames[]  = "chi2geo_Ks0_wMassConst"; //chi2_geometry of K0s (with mass constraint)
//  fVarNames[] = "DecayL_Ks0"; //decay length of K0s in 3D
//  fVarNames[] = "DecayL_Lc"; //decay length of Lc in 3D
//  fVarNames[] = "Source_Ks0"; //flag for Ks0 MC truth (“>=0” signal, “<0” background)
//  fVarNames[] = "Source_Lc"; //flag for Lc MC truth (“>=0” signal, “<0” background)
//  fVarNames[] = "DCA_PrToPV_KF"; //DCA of proton to PV from KF in 3D
//  fVarNames[] = "ct_Lc"; // life time of Lc

//  fVarNames[] = "mass_Lam"; //mass of Lambda
//  fVarNames[] = "mass_AntiLam"; //mass of Anti-Lambda
//  fVarNames[] = "mass_Gamma"; //mass of e+e-

//  fVarNames[] = "NtrkCorr"; //number of tracks after correction

  for (Int_t ivar=0; ivar<nVar; ivar++) {
    fTree_Lc->Branch(fVarNames[ivar].Data(), &fVar_Lc[ivar], Form("%s/F", fVarNames[ivar].Data()));
  }

  return;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::DefineTreeLc_Rec_QA()
{

  const char* nameoutput = GetOutputSlot(7)->GetContainer()->GetName();
  fTree_Lc_QA = new TTree(nameoutput, "QA of Lc variables tree");
  Int_t nVar = 24;
  fVar_Lc_QA = new Float_t[nVar];
  TString *fVarNames = new TString[nVar];

  // check pt, rapidity and mass with and without mass constraint
  // pt and mass without mass constraint in analysis tree
  fVarNames[0] = "rap_Ks0_woMassConst"; //rapidity of Ks0 (without mass const.)
  fVarNames[1] = "pt_Ks0_wMassConst"; //pt of Ks0 (with mass const.)
  fVarNames[2] = "rap_Ks0_wMassConst"; //rapidity of Ks0 (with mass const.)
  fVarNames[3] = "mass_Ks0_wMassConst"; //mass of Ks0 (with mass const.)
  fVarNames[4] = "chi2mass_Ks0"; //chi2_MassConst. of Ks0

  // check chi2_topo of Ks0 pointing to PV without Ks0 mass const.
  fVarNames[5] = "chi2topo_Ks0woMassConst_PV"; //chi2_topo of Ks0 (without mass const.) to PV

  // check Lc reconstruction without Ks0 mass const.
  fVarNames[6] = "chi2geo_Lc_woKs0MassConst"; //chi2_geometry of Lc (without mass constraint of Ks0)
  fVarNames[7] = "pt_Lc_woKs0MassConst"; //pt of Lc reconstructed without Ks0 mass const.
  fVarNames[8] = "rap_Lc_woKs0MassConst"; //rap of Lc reconstructed without Ks0 mass const.
  fVarNames[9] = "mass_Lc_woKs0MassConst"; //mass of Lc reconstructed without Ks0 mass const. (case 1 requested by Silvia)

  // check Lc reconstruction with Ks0 mass const. and without PV const.
  fVarNames[10] = "pt_Lc_woPV"; //pt of Lc (with mass const. of Ks0 and without PV const.)
  fVarNames[11] = "rap_Lc_woPV"; //rap of Lc (with mass const. of Ks0 and without PV const.)
  fVarNames[12] = "mass_Lc_woPV"; //mass of Lc (with mass const. of Ks0 and without PV const.) (case 2 requested by Silvia)

  // --- reconstruction logic 2 (set production vertex of Ks0 to PV) ---
  fVarNames[13] = "pt_Ks0_PV"; //pt of Ks0 (with mass and PV const.)
  fVarNames[14] = "rap_Ks0_PV"; //rapidity of Ks0 (with mass and PV const.)
  fVarNames[15] = "mass_Ks0_PV"; //mass of Ks0 (with mass and PV const.)

  fVarNames[16] = "chi2geo_Lc_wKs0MassPVConst"; //chi2_geo of Lc (with mass and PV const. of Ks0)
  fVarNames[17] = "pt_Lc_wKs0MassPVConst"; //pt of Lc (with mass and PV const. of Ks0)
  fVarNames[18] = "rap_Lc_wKs0MassPVConst"; //rapidity of Lc (with mass and PV const. of Ks0)
  fVarNames[19] = "mass_Lc_wKs0MassPVConst"; //mass of Lc (with mass and PV const. of Ks0)
  fVarNames[20] = "ldl_Lc_wKs0MassPVConst"; //l/dl of Lc (with mass and PV const. of Ks0)

  fVarNames[21] = "chi2topo_Lc_wKs0MassPVConst"; //chi2_topo of Lc (with mass and PV const. of Ks0)
  // -------------------------------------------------------------------

  // flags for signal and background
  fVarNames[22] = "Source_Ks0"; //flag for Ks0 MC truth (“>=0” signal, “<0” background)
  fVarNames[23] = "Source_Lc"; //flag for Lc MC truth (“>=0” signal, “<0” background)

//  fVarNames[] = "CosThetaStar_Pr"; //cosine angle between the proton momentum in the Lc rest frame and the boost direction

  for (Int_t ivar=0; ivar<nVar; ivar++) {
    fTree_Lc_QA->Branch(fVarNames[ivar].Data(), &fVar_Lc_QA[ivar], Form("%s/F", fVarNames[ivar].Data()));
  }

  return;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::DefineTreeLc_Gen()
{
  const char* nameoutput = GetOutputSlot(5)->GetContainer()->GetName();
  fTree_LcMCGen = new TTree(nameoutput,"Lc MC variables tree");
  Int_t nVar = 4;
  fVar_LcMCGen = new Float_t[nVar];
  TString *fVarNames = new TString[nVar];

  fVarNames[ 0]="Centrality";
  fVarNames[ 1]="LcY";
  fVarNames[ 2]="LcPt";
  fVarNames[ 3]="LcSource";

  for (Int_t ivar=0; ivar<nVar; ivar++) {
    fTree_LcMCGen->Branch(fVarNames[ivar].Data(),&fVar_LcMCGen[ivar],Form("%s/F",fVarNames[ivar].Data()));
  }

  return;
}

//_____________________________________________________________________________
Double_t AliAnalysisTaskSELc2pKs0fromKFP::InvMassV0atPV(AliAODTrack *trk1, AliAODTrack *trk2, Int_t pdg1, Int_t pdg2)
{
  
  Double_t mass1 = TDatabasePDG::Instance()->GetParticle(pdg1)->Mass();
  Double_t mass2 = TDatabasePDG::Instance()->GetParticle(pdg2)->Mass();
  Double_t E1 = TMath::Sqrt(mass1*mass1 + trk1->P()*trk1->P());
  Double_t E2 = TMath::Sqrt(mass2*mass2 + trk2->P()*trk2->P());
  Double_t mass = TMath::Sqrt( (E1+E2)*(E1+E2) - (trk1->Px()+trk2->Px())*(trk1->Px()+trk2->Px()) - (trk1->Py()+trk2->Py())*(trk1->Py()+trk2->Py()) - (trk1->Pz()+trk2->Pz())*(trk1->Pz()+trk2->Pz()) );

  return mass;
}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::DefineAnaHist()
{
  // Define analysis histograms
}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::FillEventROOTObjects()
{

  for (Int_t i=0; i<7; i++) {
    fVar_Event[i] = 0.;
  }

  Double_t pos[3];
  fpVtx->GetXYZ(pos);

  fVar_Event[1] = pos[2];

  fTree_Event->Fill();

  return;

}

//_____________________________________________________________________________
void AliAnalysisTaskSELc2pKs0fromKFP::FillTreeRecLcFromCascadeHF(AliAODRecoCascadeHF *Lc2pKs0, KFParticle kfpLc, AliAODTrack *trackPr, KFParticle kfpPr, KFParticle kfpKs0, KFParticle kfpKs0_massConstraint, AliAODTrack *v0Pos, AliAODTrack *v0Neg, KFParticle PV, TClonesArray *mcArray, Int_t lab_Ks0, Int_t lab_Lc, KFParticle kfpLc_woKs0MassConst)
{
  
  for (Int_t i=0; i<29; i++) {
    fVar_Lc[i] = -9999.;
  }

  for (Int_t i=0; i<24; i++) {
    fVar_Lc_QA[i] = -9999.;
  }

  KFParticle kfpLc_PV = kfpLc;
  kfpLc_PV.SetProductionVertex(PV);

  // pt cut for Lc
  fVar_Lc[20] = kfpLc_PV.GetPt(); //pt of Lc (with mass const. of Ks0 and with PV const.)
  if ( fVar_Lc[20] <= fAnaCuts->GetPtMinLc() ) return;

  // mass window cut for Lc
  const Float_t massLc_PDG = TDatabasePDG::Instance()->GetParticle(4122)->Mass();
  Float_t massLc_rec=0., err_massLc_rec=0.;
  kfpLc_PV.GetMass(massLc_rec, err_massLc_rec);
  if ( fabs(massLc_rec-massLc_PDG) > fAnaCuts->GetProdMassTolLc() ) return;
  fVar_Lc[22] = massLc_rec; //mass of Lc (with mass const. of Ks0 and with PV const.)

  Float_t nSigmaTPC_PiPlus  = fPID->NumberOfSigmasTPC(v0Pos, AliPID::kPion);
  Float_t nSigmaTPC_PiMinus = fPID->NumberOfSigmasTPC(v0Neg, AliPID::kPion);
  Float_t nSigmaTPC_Pr      = fPID->NumberOfSigmasTPC(trackPr, AliPID::kProton);

  Float_t nSigmaTOF_PiPlus  = fPID->NumberOfSigmasTOF(v0Pos, AliPID::kPion);
  Float_t nSigmaTOF_PiMinus = fPID->NumberOfSigmasTOF(v0Neg, AliPID::kPion);
  Float_t nSigmaTOF_Pr      = fPID->NumberOfSigmasTOF(trackPr, AliPID::kProton);

  if ( fabs(nSigmaTPC_Pr)>=4. || fabs(nSigmaTPC_PiPlus)>=4. || fabs(nSigmaTPC_PiMinus)>=4. ) return;

  fVar_Lc[0]  = nSigmaTPC_PiPlus;
  fVar_Lc[1]  = nSigmaTPC_PiMinus;
  fVar_Lc[2]  = nSigmaTPC_Pr;
  fVar_Lc[3]  = nSigmaTOF_PiPlus;
  fVar_Lc[4]  = nSigmaTOF_PiMinus;
  fVar_Lc[5]  = nSigmaTOF_Pr;
  
  KFParticle kfpKs0_Lc = kfpKs0_massConstraint;
  kfpKs0_Lc.SetProductionVertex(kfpLc);
//  if ( kfpKs0_Lc.GetChi2()/kfpKs0_Lc.GetNDF() >= fAnaCuts->GetKFPKs0_Chi2topoMax() ) return;

  KFParticle kfpKs0_PV = kfpKs0_massConstraint;
  kfpKs0_PV.SetProductionVertex(PV);
//  if ( kfpKs0_PV.GetChi2()/kfpKs0_PV.GetNDF() >= fAnaCuts->GetKFPKs0_Chi2topoMax() ) return;

  /*
  // === mass of Lam ===
  KFParticle kfpProton     = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Pos, 2212);
  KFParticle kfpPionMinus  = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Neg, -211);
  KFParticle kfpLam;
  const KFParticle *vLamDaughters[2] = {&kfpProton, &kfpPionMinus};
  kfpLam.Construct(vLamDaughters, 2);
  Float_t massLam_rec, err_massLam;
  kfpLam.GetMass(massLam_rec, err_massLam);

  // === mass of AntiLam ===
  KFParticle kfpAntiProton = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Neg, -2212);
  KFParticle kfpPionPlus   = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Pos, 211);
  KFParticle kfpAntiLam;
  const KFParticle *vAntiLamDaughters[2] = {&kfpAntiProton, &kfpPionPlus};
  kfpAntiLam.Construct(vAntiLamDaughters, 2);
  Float_t massAntiLam_rec, err_massAntiLam;
  kfpAntiLam.GetMass(massAntiLam_rec, err_massAntiLam);

  // === mass of Gamma ===
  KFParticle kfpElePlus    = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Pos, -11);
  KFParticle kfpEleMinus   = AliVertexingHFUtils::CreateKFParticleFromAODtrack(v0Neg, 11);
  KFParticle kfpGamma;
  const KFParticle *vGammaDaughters[2] = {&kfpElePlus, &kfpEleMinus};
  kfpGamma.Construct(vGammaDaughters, 2);
  Float_t massGamma_rec, err_massGamma;
  kfpGamma.GetMass(massGamma_rec, err_massGamma);
  */

  KFParticle kfpPr_Lc = kfpPr;
  kfpPr_Lc.SetProductionVertex(kfpLc);


  // calculate CosPointingAngle
  Double_t cosPA_Ks0 = AliVertexingHFUtils::CosPointingAngleFromKF(kfpKs0_massConstraint, kfpLc);
  Double_t cosPA_Lc  = AliVertexingHFUtils::CosPointingAngleFromKF(kfpLc, PV);

  AliAODv0 *v0 = dynamic_cast<AliAODv0*>(Lc2pKs0->Getv0());
  fVar_Lc[6]  = v0->GetDCA(); // DCA_Ks0Dau
  fVar_Lc[7]  = kfpKs0.GetChi2()/kfpKs0.GetNDF(); //chi2_geometry of K0s (without mass constraint)
  fVar_Lc[8]  = kfpKs0_PV.GetChi2()/kfpKs0_PV.GetNDF(); //chi2_topological of K0s (with mass constraint) to PV
  fVar_Lc[9]  = AliVertexingHFUtils::ldlFromKF(kfpKs0, PV); // ldl_Ks0

  fVar_Lc[10] = kfpLc.GetChi2()/kfpLc.GetNDF(); // chi2geo_Lc
  fVar_Lc[11] = kfpLc_PV.GetChi2()/kfpLc_PV.GetNDF(); // chi2topo_Lc
  fVar_Lc[12] = AliVertexingHFUtils::ldlFromKF(kfpLc, PV); // ldl_Lc

//  Float_t DecayL_Ks0=0., err_DecayL_Ks0=0.;
//  kfpKs0_Lc.GetDecayLength(DecayL_Ks0, err_DecayL_Ks0);
//  fVar_Lc[13] = DecayL_Ks0;
  Float_t DecayLxy_Ks0=0., err_DecayLxy_Ks0=0.;
  kfpKs0_Lc.GetDecayLengthXY(DecayLxy_Ks0, err_DecayLxy_Ks0);
  fVar_Lc[13] = DecayLxy_Ks0;
  Float_t ct_Ks0=0., err_ct_Ks0=0.;
  kfpKs0_Lc.GetLifeTime(ct_Ks0, err_ct_Ks0);
  fVar_Lc[14] = ct_Ks0;

//  Float_t DecayL_Lc=0., err_DecayL_Lc=0.;
//  kfpLc_PV.GetDecayLength(DecayL_Lc, err_DecayL_Lc);
//  fVar_Lc[15] = DecayL_Lc;
  Float_t DecayLxy_Lc=0., err_DecayLxy_Lc=0.;
  kfpLc_PV.GetDecayLengthXY(DecayLxy_Lc, err_DecayLxy_Lc);
  fVar_Lc[15] = DecayLxy_Lc;

  fVar_Lc[16] = TMath::ACos(cosPA_Ks0); // PA_Ks0
  fVar_Lc[17] = TMath::ACos(cosPA_Lc);  // PA_Lc

  fVar_Lc[18] = kfpKs0.GetPt();
  Float_t mass_Ks0_rec=0., err_mass_Ks0_rec=0.;
  kfpKs0.GetMass(mass_Ks0_rec, err_mass_Ks0_rec);
  fVar_Lc[19] = mass_Ks0_rec;

  if ( TMath::Abs(kfpLc_PV.GetE())>TMath::Abs(kfpLc_PV.GetPz()) ) {
    fVar_Lc[21] = kfpLc_PV.GetRapidity();
  }

  fVar_Lc[23] = kfpPr.GetPt();

  fVar_Lc[24] = v0Pos->Pt();
  fVar_Lc[25] = v0Neg->Pt();
  fVar_Lc[26] = Lc2pKs0->Getd0Prong(0); //rphi impact params of proton w.r.t. Primary Vtx [cm]
  fVar_Lc[27] = Lc2pKs0->Getd0Prong(1); ////rphi impact params of Ks0 w.r.t. Primary Vtx [cm]
  fVar_Lc[28] = lab_Lc;

//  fVar_Lc[] = AliVertexingHFUtils::CosThetaStarFromKF(0, 4122, 2212, 310, kfpLc, kfpPr_Lc, kfpKs0_Lc);
//  fVar_Lc[] = kfpPr.GetDistanceFromVertex(PV); //DCA of proton to PV from KF in 3D


  fVar_Lc_QA[0]  = kfpKs0.GetRapidity(); //rapidity of Ks0 (without mass const.)
  fVar_Lc_QA[1]  = kfpKs0_massConstraint.GetPt(); //pt of Ks0 (with mass const.)
  fVar_Lc_QA[2]  = kfpKs0_massConstraint.GetRapidity(); //rapidity of Ks0 (with mass const.)
  Float_t mass_Ks0_massConst_rec=0., err_mass_Ks0_massConst_rec=0.;
  kfpKs0_massConstraint.GetMass(mass_Ks0_massConst_rec, err_mass_Ks0_massConst_rec);
  fVar_Lc_QA[3]  = mass_Ks0_massConst_rec; //mass of Ks0 (with mass const.)
  fVar_Lc_QA[4]  = kfpKs0_massConstraint.GetChi2()/kfpKs0_massConstraint.GetNDF(); //chi2_MassConst. of Ks0

  KFParticle kfpKs0_woMassConst_PV = kfpKs0;
  kfpKs0_woMassConst_PV.SetProductionVertex(PV);
  fVar_Lc_QA[5]  = kfpKs0_woMassConst_PV.GetChi2()/kfpKs0_woMassConst_PV.GetNDF();

  fVar_Lc_QA[6]  = kfpLc_woKs0MassConst.GetChi2()/kfpLc_woKs0MassConst.GetNDF();
  fVar_Lc_QA[7]  = kfpLc_woKs0MassConst.GetPt();
  fVar_Lc_QA[8]  = kfpLc_woKs0MassConst.GetRapidity();
  Float_t mass_Lc_woKs0MassConst_rec=0., err_mass_Lc_woKs0MassConst_rec=0.;
  kfpLc_woKs0MassConst.GetMass(mass_Lc_woKs0MassConst_rec, err_mass_Lc_woKs0MassConst_rec);
  fVar_Lc_QA[9]  = mass_Lc_woKs0MassConst_rec;

  fVar_Lc_QA[10] = kfpLc.GetPt();
  fVar_Lc_QA[11] = kfpLc.GetRapidity();
  Float_t mass_Lc_woPV_rec=0., err_mass_Lc_woPV_rec=0.;
  kfpLc.GetMass(mass_Lc_woPV_rec, err_mass_Lc_woPV_rec);
  fVar_Lc_QA[12] = mass_Lc_woPV_rec;

  // --- reconstruction logic 2 (set production vertex of Ks0 to PV) ---
  //
  // reconstruct Lc (with mass and PV const. of Ks0)
  fVar_Lc_QA[13] = kfpKs0_PV.GetPt();
  fVar_Lc_QA[14] = kfpKs0_PV.GetRapidity();
  Float_t mass_Ks0_wMassPVConst_rec=0., err_mass_Ks0_wMassPVConst_rec=0.;
  kfpKs0_PV.GetMass(mass_Ks0_wMassPVConst_rec, err_mass_Ks0_wMassPVConst_rec);
  fVar_Lc_QA[15] = mass_Ks0_wMassPVConst_rec;

  KFParticle kfpLc_wKs0PV;
  const KFParticle *LcDaughters_wKs0PV[2] = {&kfpPr, &kfpKs0_PV};
  kfpLc_wKs0PV.Construct(LcDaughters_wKs0PV, 2);
  fVar_Lc_QA[16] = kfpLc_wKs0PV.GetChi2()/kfpLc_wKs0PV.GetNDF();
  fVar_Lc_QA[17] = kfpLc_wKs0PV.GetPt();
  fVar_Lc_QA[18] = kfpLc_wKs0PV.GetRapidity();
  Float_t mass_Lc_wKs0PV_rec=0., err_mass_Lc_wKs0PV_rec=0.;
  kfpLc_wKs0PV.GetMass(mass_Lc_wKs0PV_rec, err_mass_Lc_wKs0PV_rec);
  fVar_Lc_QA[19] = mass_Lc_wKs0PV_rec;
  fVar_Lc_QA[20] = AliVertexingHFUtils::ldlFromKF(kfpLc_wKs0PV, PV);

  KFParticle kfpLc_PV_wKs0PV = kfpLc_wKs0PV;
  kfpLc_PV_wKs0PV.SetProductionVertex(PV);
  fVar_Lc_QA[21] = kfpLc_PV_wKs0PV.GetChi2()/kfpLc_PV_wKs0PV.GetNDF();

  fVar_Lc_QA[22] = lab_Ks0;
  fVar_Lc_QA[23] = lab_Lc;

  /*
  //Method to get tracklet multiplicity from event
  Int_t countTreta1corr = 0;
  AliAODEvent* aodEvent = dynamic_cast<AliAODEvent*>(fInputEvent);
  TProfile *estimatorAvg = GetEstimatorHistogram(aodEvent);
  if (estimatorAvg) {
    countTreta1corr = static_cast<Int_t>(AliVertexingHFUtils::GetCorrectedNtracklets(estimatorAvg,fNTracklets,fVtx1->GetZ(),fRefMult));
  }
  fVar_Lc[43] = countTreta1corr; // NtrkCorr
  */

  if (fIsMC && lab_Lc>=0) fTree_Lc->Fill();
  if (!fIsMC) fTree_Lc->Fill();

  if (fWriteLcQATree) {fTree_Lc_QA->Fill();}

  /*
  cout << "==========" << endl;
  cout << "kfpKs0: " << kfpKs0 << endl;
  cout << "kfpKs0_massConstraint: " << kfpKs0_massConstraint << endl;
  cout << "kfpKs0_PV: " << kfpKs0_PV << endl;
  cout << "PV: " << PV << endl;
  cout << "==========" << endl;
  */

  return;

}

//_____________________________________________________________________________
Int_t AliAnalysisTaskSELc2pKs0fromKFP::MatchToMCLc(AliAODTrack *v0Pos, AliAODTrack *v0Neg, AliAODTrack *bachPart, TClonesArray *mcArray)
{
  // Check if all of the tracks is matched to a MC signal
  // If no, return -1;
  // If yes, return label (>=0) of the AliAODMCParticle

  Int_t labelPionPlus = fabs(v0Pos->GetLabel());
  AliAODMCParticle *mcPionPlus = static_cast<AliAODMCParticle*>(mcArray->At(labelPionPlus));

  Int_t labelPionMinus = fabs(v0Neg->GetLabel());
  AliAODMCParticle *mcPionMinus = static_cast<AliAODMCParticle*>(mcArray->At(labelPionMinus));

  Int_t labelProton = fabs(bachPart->GetLabel());
  AliAODMCParticle *mcProton = static_cast<AliAODMCParticle*>(mcArray->At(labelProton));

  if ( mcPionPlus->GetPdgCode()!=211 || mcPionMinus->GetPdgCode()!=-211 || fabs(mcProton->GetPdgCode())!=2212 ) return -1;

  Int_t IndexMother[2] = {-9999, -9999};

  IndexMother[0] = mcPionPlus->GetMother();
  IndexMother[1] = mcPionMinus->GetMother();
  if ( IndexMother[0]<0 || IndexMother[1]<0 ) return -1; // check mother exist
  if ( IndexMother[0] != IndexMother[1] ) return -1; // check the same mother
  AliAODMCParticle* mcMother = static_cast<AliAODMCParticle*>(mcArray->At(IndexMother[0]));
  if ( fabs(mcMother->GetPdgCode()) != 310 || mcMother->GetNDaughters()!=2 ) return -1; // check mother is Ks0 and have two daughters

  IndexMother[0] = mcMother->GetMother(); // mother of Ks0
  if ( IndexMother[0]<0 ) return -1; // check mother exist
  mcMother = static_cast<AliAODMCParticle*>(mcArray->At(IndexMother[0]));
  if ( fabs(mcMother->GetPdgCode()) != 311 || mcMother->GetNDaughters()!=1 ) return -1; // check mother is K0 and have only one daughter

  IndexMother[0] = mcMother->GetMother(); // mother of K0
  IndexMother[1] = mcProton->GetMother(); // mother of proton
  if ( IndexMother[0]<0 || IndexMother[1]<0 ) return -1; // check mother exist
  if ( IndexMother[0] != IndexMother[1] ) return -1; // check the same mother
  mcMother = static_cast<AliAODMCParticle*>(mcArray->At(IndexMother[0]));
  if ( fabs(mcMother->GetPdgCode()) != 4122 || mcMother->GetNDaughters()!=2 ) return -1; // check mother is Lc and have two daughters

//  if ( mcMother->IsPrimary() ) return 1;
//  if ( mcMother->IsPhysicalPrimary() ) return 2;
//  if ( mcMother->IsSecondaryFromWeakDecay() ) return 3;
//  if ( mcMother->IsSecondaryFromMaterial() ) return 4;
//  if ( mcMother->IsFromSubsidiaryEvent() ) return 5;

  Int_t CheckOrigin = AliVertexingHFUtils::CheckOrigin(mcArray,mcMother,kTRUE);
  return CheckOrigin;

}

//_____________________________________________________________________________
Int_t AliAnalysisTaskSELc2pKs0fromKFP::MatchToMCKs0(AliAODTrack *v0Pos, AliAODTrack *v0Neg, TClonesArray *mcArray)
{
  // Check if all of the tracks is matched to a MC signal
  // If no, return -1;
  // If yes, return label (>=0) of the AliAODMCParticle

  Int_t labelPionPlus = fabs(v0Pos->GetLabel());
  AliAODMCParticle *mcPionPlus = static_cast<AliAODMCParticle*>(mcArray->At(labelPionPlus));

  Int_t labelPionMinus = fabs(v0Neg->GetLabel());
  AliAODMCParticle *mcPionMinus = static_cast<AliAODMCParticle*>(mcArray->At(labelPionMinus));

  if ( mcPionPlus->GetPdgCode()!=211 || mcPionMinus->GetPdgCode()!=-211 ) return -1;

  Int_t IndexMother[2] = {-9999, -9999};

  IndexMother[0] = mcPionPlus->GetMother();
  IndexMother[1] = mcPionMinus->GetMother();

  if ( IndexMother[0]<0 || IndexMother[1]<0 ) return -1; // check mother exist
  if ( IndexMother[0] != IndexMother[1] ) return -1; // check the same mother
  AliAODMCParticle* mcMother = static_cast<AliAODMCParticle*>(mcArray->At(IndexMother[0]));
  if ( fabs(mcMother->GetPdgCode()) != 310 || mcMother->GetNDaughters()!=2 ) return -1; // check mother is Ks0 and have two daughters

  return 1;

}
