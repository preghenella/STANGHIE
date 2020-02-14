#include "Fun4All_G4_Tracker.C"

void runAna(int pid, double pipe, double field, double eta, std::string tagname, int nevents)
{
  if (nevents <= 0) return;

  GENERATOR_pid = pid;
  GENERATOR_etamin = eta;
  GENERATOR_etamax = eta;
  GENERATOR_phimin = 0.;
  GENERATOR_phimax = 2. * M_PI;
  MAGNET_field = field;
  PIPE_radius = pipe;
  
  Fun4All_G4_Tracker();
  Fun4AllServer *se = Fun4AllServer::instance(); 
  auto ana = (TrackerAnalysis *)se->getSubsysReco("TrackerAnalysis");
  ana->SetPid(pid);
  ana->SetFileName(tagname + ".trackerAnalysis.root");
  auto gen = (PHG4ParticleGenerator *) se->getSubsysReco("Generator");

  double logmom = -2.0;
  double logmomstep = 0.1; // 0.01;
  while (true) {
    double mom = pow(10., logmom);
    logmom += logmomstep;
    if (mom < 0.03) continue;
    if (mom > 30.) break;
    std::cout << " --- running with momentum " << mom << " GeV/c " << std::endl;
    gen->set_mom_range(mom, mom);
    se->run(nevents);
  }
  
  // finish job - close and save output files
  se->End();
  delete se;
  gSystem->Exit(0);

}
