#include "io.C"

void
conversion(std::string fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  auto hGen = new TH1F("hGen", "", 601, -3.005, 3.005);
  auto hCon = new TH1F("hCon", "", 601, -3.005, 3.005);
  
  auto hPt = new TH2F("hPt", "", 61, -3.05, 3.05, 100, 0., 1.);
  auto hEta = new TH2F("hEta", "", 61, -3.05, 3.05, 1000, -5., 5.);

  auto hPpar = new TH1F("hPpar", "", 100, 0., 1.);
  auto hPper = new TH1F("hPper", "", 1000, 0., 0.1);

  
  auto h2 = new TH2F("h2", "", 100, 0., 1., 1000, 0., 10.);
  
  /** loop over events **/
  TLorentzVector lv;
  TVector3 vgamma, dgamma, vel;
  for (int iev = 0; iev < nevents; ++iev) {
    
    io.event(iev);

    auto egamma = io.tracks.e[0];

    vgamma.SetXYZ(io.tracks.px[0], io.tracks.py[0], io.tracks.pz[0]);
    double pgamma = vgamma.Mag();
    dgamma.SetXYZ(io.tracks.px[0] / pgamma, io.tracks.py[0] / pgamma, io.tracks.pz[0] / pgamma);
    
    hGen->Fill(log10(egamma));

    /** select conversion events **/
    if (io.tracks.n != 3) continue;


    hCon->Fill(log10(egamma));
    
    if (io.tracks.pdg[1] != 11 || io.tracks.pdg[2] != -11) {
      std::cout << "not a conversion event" << std::endl;
      continue;
    }
    auto epx = io.tracks.px[1];
    auto epy = io.tracks.py[1];
    auto epz = io.tracks.pz[1];
    auto ept = hypot(epx, epy);
    auto eeta = -log(tan(atan2(ept, epz) * 0.5));
    
    hPt->Fill(log10(egamma), ept / egamma);
    hEta->Fill(log10(egamma), eeta);

    h2->Fill(ept / egamma, hypot(epy, epz));

    vel.SetXYZ(epx, epy, epz);

    hPpar->Fill(vel.Dot(dgamma) / pgamma);
    hPper->Fill(vel.Perp(dgamma));
      
    
  }

  hPt->Sumw2();
  hPt->Scale(1. / nevents);

  hPpar->Sumw2();
  hPpar->Scale(1. / nevents);
  
  //  hPper->Sumw2();
  //  hPper->Scale(1. / nevents);
  
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  c->cd(1);
  hPt->Draw("colz");
  c->cd(2);
  hEta->Draw("colz");
  c->cd(3);

  new TCanvas("cc");
  hCon->Sumw2();
  hGen->Sumw2();
  hCon->Divide(hCon, hGen, 1., 1., "B");
  hCon->Draw();

  new TCanvas("c2");
  h2->Draw("colz");

  new TCanvas("cpar");
  hPpar->Draw();
  new TCanvas("cper");
  hPper->DrawNormalized();
}


