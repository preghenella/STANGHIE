#pragma once
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <g4detectors/PHG4CylinderSubsystem.h>
#include <g4trackfastsim/PHG4TrackFastSim.h>
#include <g4trackfastsim/PHG4TrackFastSimEval.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4main/PHG4Reco.h>
#include <phool/recoConsts.h>
#include <g4eval/PHG4DSTReader.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4testbench.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4trackfastsim.so)
R__LOAD_LIBRARY(libg4eval.so)

#include "TrackerParameters.C"
#include "TrackerAnalysis.C"

int Fun4All_G4_Tracker(const int nEvents = 1000, int pid = 11, std::string tagname = "NextGen")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4trackfastsim.so");

  const bool whether_to_sim_calorimeter = false;

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();
  //  rc->set_IntFlag("RANDOMSEED", 12345); // if you want to use a fixed seed
  // PHG4ParticleGenerator generates particle
  // distributions in eta/phi/mom range
  PHG4ParticleGenerator *gen = new PHG4ParticleGenerator("Generator");
  gen->set_pid(GENERATOR_pid);
  gen->set_vtx(0., 0., 0.);
  gen->set_z_range(0., 0.);
  gen->set_eta_range(GENERATOR_etamin, GENERATOR_etamax);
  gen->set_mom_range(GENERATOR_pmin, GENERATOR_pmax);
  gen->set_phi_range(GENERATOR_phimin, GENERATOR_phimax);
  se->registerSubsystem(gen);

  PHG4Reco *g4Reco = new PHG4Reco();
  g4Reco->SetWorldMaterial("G4_AIR");
  g4Reco->set_field(MAGNET_field);  // solenoidal field
  g4Reco->set_rapidity_coverage(1.0);
  
  /** beam PIPE **/
  auto pipe = new PHG4CylinderSubsystem("PIPE", 0);
  pipe->set_int_param("lengthviarapidity", 0);
  pipe->set_double_param("radius", PIPE_radius);
  pipe->set_double_param("length", PIPE_length);
  pipe->set_double_param("thickness", PIPE_thickness);
  pipe->set_string_param("material", "G4_Be");
  pipe->SuperDetector("PIPE");
  g4Reco->registerSubsystem(pipe);

  /** INNER tracker **/
  for (int ilayer = 0; ilayer < INNER_layers; ++ilayer) {
    auto inner = new PHG4CylinderSubsystem("INNER", ilayer);
    inner->set_int_param("lengthviarapidity", 1);
    inner->set_double_param("radius", INNER_radius[ilayer]);
    inner->set_double_param("length", INNER_length[ilayer]);
    inner->set_double_param("thickness", INNER_thickness);
    inner->set_string_param("material", "G4_Si");
    inner->SetActive();
    inner->SuperDetector("INNER");
    g4Reco->registerSubsystem(inner);
  }

  /** OUTER tracker **/
  for (int ilayer = 0; ilayer < OUTER_layers; ++ilayer) {
    auto outer = new PHG4CylinderSubsystem("OUTER", ilayer);
    outer->set_int_param("lengthviarapidity", 1);
    outer->set_double_param("radius", OUTER_radius[ilayer]);
    outer->set_double_param("length", OUTER_length[ilayer]);
    outer->set_double_param("thickness", OUTER_thickness);
    outer->set_string_param("material", "G4_Si");
    outer->SetActive();
    outer->SuperDetector("OUTER");
    g4Reco->registerSubsystem(outer);
  }

  /** TOF tracker **/
  auto tof = new PHG4CylinderSubsystem("TOF", 0);
  tof->set_int_param("lengthviarapidity", 1);
  tof->set_double_param("radius", TOF_radius);
  tof->set_double_param("length", TOF_length);
  tof->set_double_param("thickness", TOF_thickness);
  tof->set_string_param("material", "G4_Si");
  tof->SetActive();
  tof->SuperDetector("TOF");
  g4Reco->registerSubsystem(tof);
  
  // Black hole swallows everything - prevent loopers from returning
  // to inner detectors
  auto black = new PHG4CylinderSubsystem("BlackHole", 0);
  black->set_int_param("lengthviarapidity", 0);
  black->set_double_param("radius", 120.);        // 80 cm
  black->set_double_param("length", 1000.);
  black->set_double_param("thickness", 0.1); // does not matter (but > 0)
  black->SetActive();
  black->BlackHole(); // eats everything
  g4Reco->registerSubsystem(black);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);

  //---------------------------
  // fast pattern recognition and full Kalman filter
  // output evaluation file for truth track and reco tracks are PHG4TruthInfoContainer
  //---------------------------
  PHG4TrackFastSim *kalman = new PHG4TrackFastSim("PHG4TrackFastSim");
  kalman->set_primary_assumption_pid(pid);
  kalman->set_use_vertex_in_fitting(false);
  kalman->set_vertex_xy_resolution(0.e-4);
  kalman->set_vertex_z_resolution(0.e-4);
  kalman->set_sub_top_node_name("SVTX");
  kalman->set_trackmap_out_name("SvtxTrackMap");

  //  add INNER hits
  kalman->add_phg4hits(
      "G4HIT_INNER",               //      const std::string& phg4hitsNames,
      PHG4TrackFastSim::Cylinder,  //      const DETECTOR_TYPE phg4dettype,
      INNER_resolution_r,          //      radial-resolution [cm]
      INNER_resolution_rphi,       //      azimuthal-resolution [cm]
      INNER_resolution_z,          //      z-resolution [cm]
      1,                           //      efficiency,
      0                            //      noise hits
  );

  //  add OUTER hits
  kalman->add_phg4hits(
      "G4HIT_OUTER",               //      const std::string& phg4hitsNames,
      PHG4TrackFastSim::Cylinder,  //      const DETECTOR_TYPE phg4dettype,
      OUTER_resolution_r,          //      radial-resolution [cm]
      OUTER_resolution_rphi,       //      azimuthal-resolution [cm]
      OUTER_resolution_z,          //      z-resolution [cm]
      1,                           //      efficiency,
      0                            //      noise hits
  );

  //  add TOF hits
  kalman->add_phg4hits(
      "G4HIT_TOF",                 //      const std::string& phg4hitsNames,
      PHG4TrackFastSim::Cylinder,  //      const DETECTOR_TYPE phg4dettype,
      TOF_resolution_r,            //      radial-resolution [cm]
      TOF_resolution_rphi,         //      azimuthal-resolution [cm]
      TOF_resolution_z,            //      z-resolution [cm]
      1,                           //      efficiency,
      0                            //      noise hits
  );

  se->registerSubsystem(kalman);
  
  auto trackerAnalysis = new TrackerAnalysis();
  se->registerSubsystem(trackerAnalysis);
    
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);

  return 0;
}

