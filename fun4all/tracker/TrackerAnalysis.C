#pragma once
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TRandom.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4trackfastsim.so)
R__LOAD_LIBRARY(libphool.so)

class TrackerAnalysis : public SubsysReco
{
public:
  TrackerAnalysis() : SubsysReco("TrackerAnalysis") {};
  ~TrackerAnalysis() {};

  int Init(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);

  void SetFileName(std::string val) { mFileName = val; };
  void SetPid(int val) { mPid = val; };
  
private:

  int mPid = 211;
  std::string mFileName = "trackerAnalysis.root";
  TH1 *mHistoGenPt;
  TH1 *mHistoHitPt;
  TH1 *mHistoRecPt;
  TH2 *mHistoResPt;
  TH2 *mHistoDcaPt;
  TH2 *mHistoZPt;
  TH2 *mHistoMassPt;

};

int
TrackerAnalysis::Init(PHCompositeNode *topNode) {
  
  mHistoGenPt  = new TH1F("mHistoGenPt" , "", 30001, -0.00005, 30.0 + 0.00005);
  mHistoHitPt  = new TH1F("mHistoHitPt" , "", 30001, -0.00005, 30.0 + 0.00005);    
  mHistoRecPt  = new TH1F("mHistoRecPt" , "", 30001, -0.00005, 30.0 + 0.00005);    
  mHistoResPt  = new TH2F("mHistoResPt" , "", 30001, -0.00005, 30.0 + 0.00005, 2000, -1.0, 1.0);    
  mHistoDcaPt  = new TH2F("mHistoDcaPt" , "", 30001, -0.00005, 30.0 + 0.00005, 2000, -0.1, 0.1);    
  mHistoZPt    = new TH2F("mHistoZPt"   , "", 30001, -0.00005, 30.0 + 0.00005, 2000, -0.1, 0.1);    
  mHistoMassPt = new TH2F("mHistoMassPt", "", 30001, -0.00005, 30.0 + 0.00005, 2000, -0.2, 1.8);    
  return Fun4AllReturnCodes::EVENT_OK;
};

int
TrackerAnalysis::End(PHCompositeNode *topNode) {
  auto fout = TFile::Open(mFileName.c_str(), "RECREATE");
  mHistoGenPt->Write();
  mHistoHitPt->Write();
  mHistoRecPt->Write();
  mHistoResPt->Write();
  mHistoDcaPt->Write();
  mHistoZPt->Write();
  mHistoMassPt->Write();
  fout->Close();
  return Fun4AllReturnCodes::EVENT_OK;
};

int
TrackerAnalysis::process_event(PHCompositeNode *topNode) {

  /** get track map **/
  auto tracks = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!tracks) {
    std::cout << "Cannot find SvtxTrackMap" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /** get truth info **/
  auto truth = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truth) {
    std::cout << "Cannot find G4TruthInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  /** get INNER hits **/
  auto INNER_hits = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_INNER");
  if (!INNER_hits) {
    std::cout << "Cannot find G4HIT_INNER" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  /** get OUTER hits **/
  auto OUTER_hits = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_OUTER");
  if (!OUTER_hits) {
    std::cout << "Cannot find G4HIT_OUTER" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  /** get TOF hits **/
  auto TOF_hits = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_TOF");
  if (!TOF_hits) {
    std::cout << "Cannot find G4HIT_TOF" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  TVector3 pos_h;
  TVector3 pos_i, pos_r, pos_f;
  TVector3 mom_i, mom_r, mom_f;
  TMatrixDSym cov_i(6), cov_r(6), cov_f(6);
  std::unique_ptr<genfit::AbsTrackRep> rep(nullptr);
  std::unique_ptr<genfit::MeasuredStateOnPlane> sop(nullptr);
  
  /** loop over particles **/
  auto primaries =  truth->GetPrimaryParticleRange();
  for (auto iprimary = primaries.first; iprimary != primaries.second; ++iprimary) {
    auto particle = iprimary->second;
    if (!particle) continue;
    auto px  = particle->get_px();
    auto py  = particle->get_py();
    auto pz  = particle->get_pz();
    auto p   = sqrt(px * px + py * py + pz * pz);
    auto pt  = hypot(px, py);

    /*
    auto x   = particle->get_vx();
    auto y   = particle->get_vy();
    auto z   = particle->get_vz();
    auto dca = hypot(x, y);
    */    

    mHistoGenPt->Fill(pt);

    SvtxTrack_FastSim *recoTrack = nullptr;
    PHG4Hit *recoHit = nullptr;

    /** loop over TOF hits **/
    for (auto ihit = TOF_hits->getHits().first; ihit != TOF_hits->getHits().second; ++ihit) {
      auto hit = dynamic_cast<PHG4Hit *>(ihit->second);
      if (!hit) continue;
      if (hit->get_trkid() == particle->get_track_id()) {
	recoHit = hit;
	break;
      }
    } /** end of loop over hits **/
      
    if (!recoHit) continue;

    mHistoHitPt->Fill(pt);
      
    /** loop over tracks **/
    for (auto itrack = tracks->begin(); itrack != tracks->end(); ++itrack) {
      auto track = dynamic_cast<SvtxTrack_FastSim *>(itrack->second);
      if (!track) continue;

      if (track->get_truth_track_id() == particle->get_track_id()) {
	recoTrack = track;
	break;
      }
    }

    if (!recoTrack) continue;
    
    mHistoRecPt->Fill(pt);
    
    auto px_rec = recoTrack->get_px();
    auto py_rec = recoTrack->get_py();
    auto pz_rec = recoTrack->get_pz();
    auto p_rec  = sqrt(px_rec * px_rec + py_rec * py_rec + pz_rec * pz_rec);
    mHistoResPt->Fill(pt, (p_rec - p) / p);
    
    auto x_rec   = recoTrack->get_x();
    auto y_rec   = recoTrack->get_y();
    auto z_rec   = recoTrack->get_z();
    auto dca_rec = recoTrack->get_dca2d();//hypot(x_rec, y_rec);
    mHistoDcaPt->Fill(pt, dca_rec);
    mHistoZPt->Fill(pt, z_rec);


#if 0
    /** extrapolate track to TOF hit point **/

    /** get last track state **/
    auto istate = --recoTrack->end_states();
    auto state = istate->second; 
    if (!state)  continue;
      
    /** get state info **/      
    auto pidguess = recoTrack->get_charge() * mPid;
    auto length_i = state->get_pathlength();
    pos_i.SetXYZ(state->get_x(), state->get_y(), state->get_z());
    mom_i.SetXYZ(state->get_px(), state->get_py(), state->get_pz());
    for (int i = 0; i < 6; ++i)
      for (int j = 0; j < 6; ++j)
	cov_i[i][j] = state->get_error(i, j);

    /** extrapolate **/
    rep.reset(new genfit::RKTrackRep(pidguess));
    sop.reset(new genfit::MeasuredStateOnPlane(rep.get()));
    sop->setPosMomCov(pos_i, mom_i, cov_i);
    pos_h.SetXYZ(recoHit->get_x(0), recoHit->get_y(0), recoHit->get_z(0));
    auto length = length_i;
    try {
      length += rep->extrapolateToCylinder(*sop, 100., TVector3(0., 0., 0.), TVector3(0., 0., 1.));
    }
    catch(...) {
      continue;
    }
    sop->getPosMomCov(pos_f, mom_f, cov_f);
    
    auto distance = (pos_f - pos_h).Mag();
    std::cout << " WAIT A SECOND, THE DISTANCE IS LARGE " << distance << std::endl;
    pos_f.Print();
    pos_h.Print();
    if (distance > 10.) continue;
    
    auto time = gRandom->Gaus(recoHit->get_t(0), 0.020);
    auto beta = length / time / 29.979246;
    auto mass = p_rec * p_rec * (1. / (beta * beta) - 1.);

    mHistoMassPt->Fill(pt, mass);

#endif
    
  } /** end of loop over particles **/

  return Fun4AllReturnCodes::EVENT_OK;
};
  
