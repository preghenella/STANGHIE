#include "io.C"
#include "style.C"

const double c_light = TMath::C() * 1.e2 / 1.e9;

const double p_reso = 0.01;    // relative
const double time_reso = 0.02; // ps

std::map<std::string, TH1 *>
tof(std::string fname, std::vector<int> tof_layers = {4, 9})
{
  io.open(fname);
  auto nevents = io.nevents();
  
  auto hBetaP = new TH2F("hBetaP", ";log_{10}(#it{p} / GeV/#it{c});v/#it{c};",
			 400, -2., 2., 250, 0., 1.25);
  
  auto hSigEl = new TH2F("hSigEl", ";log_{10}(#it{p} / GeV/#it{c});signal;",
			 400, -2., 2., 5000, -250., 250.);
  
  auto hGenEl = new TH1F("hGenEl", "", 80, -2., 2.);
  auto hGenPi = new TH1F("hGenPi", "", 80, -2., 2.);
  auto hGenKa = new TH1F("hGenKa", "", 80, -2., 2.);
  auto hGenPr = new TH1F("hGenPr", "", 80, -2., 2.);

  auto hRecEl = new TH1F("hRecEl", "", 80, -2., 2.);
  auto hRecPi = new TH1F("hRecPi", "", 80, -2., 2.);
  auto hRecKa = new TH1F("hRecKa", "", 80, -2., 2.);
  auto hRecPr = new TH1F("hRecPr", "", 80, -2., 2.);

  TVector3 vec; 
  /** loop over events **/
  for (int iev = 0; iev < nevents; ++iev) {
    
    io.event(iev);

    /** loop over primary tracks **/
    int n_tof_tracks = 0;
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {      
      if (io.tracks.parent[itrk] > 0) continue;
      
      auto px = io.tracks.px[itrk];
      auto py = io.tracks.py[itrk];
      auto pz = io.tracks.pz[itrk];
      vec.SetXYZ(px, py, pz);
      auto pt  = vec.Pt();
      auto p   = vec.Mag();
      auto eta = vec.Eta();

      //      p = gRandom->Gaus(p, p * 0.1);
      
      /** check track at mid-rapidity **/
      if (fabs(eta) > 0.8) continue;

      /** fill efficieny histograms **/
      switch (abs(io.tracks.pdg[itrk])) {
      case 11:
	hGenEl->Fill(log10(pt));
	break;
      case 211:
	hGenPi->Fill(log10(pt));
	break;
      case 321:
	hGenKa->Fill(log10(pt));
	break;
      case 2212:
	hGenPr->Fill(log10(pt));
	break;
      }
      
      /** search for hits on TOF layers **/
      std::vector<std::pair<float, float>> tof_hits;
      for (auto tof_layer : tof_layers) {
	for (int ihit = 0; ihit < io.hits.n; ++ihit) { 
	  if (io.hits.trkid[ihit] != io.tracks.id[itrk]) continue; // check the hit belongs to this track
	  if (io.hits.lyrid[ihit] != tof_layer)          continue; // check the hit belongs to this layer
	  auto time = gRandom->Gaus(io.hits.t[ihit], 0.020);       // time smearing
	  auto length = gRandom->Gaus(io.hits.trklen[ihit], 0.);   // length smearing
	  tof_hits.push_back( {time, length} );
	  break;
	}
      }
      if (tof_hits.size() <= 0) continue;
      n_tof_tracks++;

      /** sort hits by track length, longest first **/
      std::sort(tof_hits.begin(), tof_hits.end(),
		[](const pair<float, float> &l, const pair<float, float> &r) { return r.second < l.second; });

      /** require at least 90 cm length for p > 200 MeV/c **/
      if (p > 0.2 && tof_hits[0].second < 90.)
	continue;

      /** use the longest track for particle-identification **/
      auto time    = tof_hits[0].first;
      auto length  = tof_hits[0].second;
      auto beta    = length / (time * c_light);	
      hBetaP->Fill(log10(p), beta);
      auto mass2   = p * p * (1. / (beta * beta) - 1.);
      auto texp_el = length / c_light / p * TMath::Sqrt(5.110e-04 * 5.110e-04 + p * p);
      hSigEl->Fill(log10(pt), (time - texp_el) / 0.020);

      /** fill efficieny histograms **/
      switch (abs(io.tracks.pdg[itrk])) {
      case 11:
	hRecEl->Fill(log10(pt));
	break;
      case 211:
	hRecPi->Fill(log10(pt));
	break;
      case 321:
	hRecKa->Fill(log10(pt));
	break;
      case 2212:
	hRecPr->Fill(log10(pt));
	break;
      }

    }
  }

  auto hEffEl = (TH1 *)hRecEl->Clone("hEffEl");
  hEffEl->Sumw2();
  hEffEl->SetTitle(";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  hEffEl->SetMarkerStyle(20);
  hEffEl->SetMarkerColor(kGray+2);
  hEffEl->SetLineColor(kGray+2);
  hEffEl->Divide(hEffEl, hGenEl, 1., 1., "B");
  
  
  auto hEffPi = (TH1 *)hRecPi->Clone("hEffPi");
  hEffPi->Sumw2();
  hEffPi->SetTitle(";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  hEffPi->SetMarkerStyle(20);
  hEffPi->SetMarkerColor(kAzure-3);
  hEffPi->SetLineColor(kAzure-3);
  hEffPi->Divide(hEffPi, hGenPi, 1., 1., "B");
  
  auto hEffKa = (TH1 *)hRecKa->Clone("hEffKa");
  hEffKa->Sumw2();
  hEffKa->SetTitle(";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  hEffKa->SetMarkerStyle(20);
  hEffKa->SetMarkerColor(kGreen+2);
  hEffKa->SetLineColor(kGreen+2);
  hEffKa->Divide(hEffKa, hGenKa, 1., 1., "B");
  
  auto hEffPr = (TH1 *)hRecPr->Clone("hEffPr");
  hEffPr->Sumw2();
  hEffPr->SetTitle(";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  hEffPr->SetMarkerStyle(20);
  hEffPr->SetMarkerColor(kRed+1);
  hEffPr->SetLineColor(kRed+1);
  hEffPr->Divide(hEffPr, hGenPr, 1., 1., "B");
  
  std::map<std::string, TH1 *> histos = {
    { "hBetaP", hBetaP },
    { "hEffEl", hEffEl },
    { "hEffPi", hEffPi },
    { "hEffKa", hEffKa },
    { "hEffPr", hEffPr },
    { "hSigEl", hSigEl }
  };
  
  return histos;
}

void
draw()
{
  style();
  gStyle->SetOptStat(0);

  auto cBeta = new TCanvas("cBeta", "cBeta", 1200, 1200);
  cBeta->Divide(3, 3);

  auto cEff = new TCanvas("cEff", "cEff", 1200, 1200);
  cEff->Divide(3, 3);

  std::vector<std::string> fnames = {"pythia8.1T.root", "pythia8.5kG.root", "pythia8.2kG.root"};
  std::map<std::string, std::map<std::string, TH1 *>> A, B, C;
  int ic = 0;
  for (auto fname : fnames) {
    auto a = tof(fname, {4});
    cBeta->cd(ic + 1)->SetLogz();
    cBeta->cd(ic + 1)->SetGridx();
    a["hBetaP"]->DrawCopy("col");
    cEff->cd(ic + 1)->SetLogz();
    cEff->cd(ic + 1)->SetGridx();
    a["hEffEl"]->DrawCopy();
    a["hEffPi"]->DrawCopy("same");
    a["hEffKa"]->DrawCopy("same");
    a["hEffPr"]->DrawCopy("same");
    A[fname] = a;
    
    auto b = tof(fname, {9});
    cBeta->cd(ic + 2)->SetLogz();
    cBeta->cd(ic + 2)->SetGridx();
    b["hBetaP"]->DrawCopy("col");
    cEff->cd(ic + 2)->SetLogz();
    cEff->cd(ic + 2)->SetGridx();
    b["hEffEl"]->DrawCopy();
    b["hEffPi"]->DrawCopy("same");
    b["hEffKa"]->DrawCopy("same");
    b["hEffPr"]->DrawCopy("same");
    B[fname] = b;
    
    auto c = tof(fname, {4, 9});
    cBeta->cd(ic + 3)->SetLogz();
    cBeta->cd(ic + 3)->SetGridx();
    c["hBetaP"]->DrawCopy("col");
    cEff->cd(ic + 3)->SetLogz();
    cEff->cd(ic + 3)->SetGridx();
    c["hEffEl"]->DrawCopy();
    c["hEffPi"]->DrawCopy("same");
    c["hEffKa"]->DrawCopy("same");
    c["hEffPr"]->DrawCopy("same");
    C[fname] = c;

    cBeta->Update();
    cEff->Update();
    ic = ic + 3;
  }

  auto cBetaS = new TCanvas("cBetaS", "cBetaS", 800, 800);
  cBetaS->DrawFrame(-2., 0., 1., 1.15, ";log_{10}(#it{p} / GeV/#it{c});v/#it{c};");
  C["pythia8.1T.root"]["hBetaP"]->DrawCopy("col,same");
  
  auto cSigEl = new TCanvas("cSigEl", "cSigEl", 800, 800);
  cSigEl->DrawFrame(-2., -12.5, 2., 12.5, ";log_{10}(#it{p} / GeV/#it{c});electron signal;");
  C["pythia8.1T.root"]["hSigEl"]->DrawCopy("col,same");
  
  auto cEffS = new TCanvas("cEffS", "cEffS", 800, 800);
  cEffS->DrawFrame(-2., 0., 0., 1.05, ";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  for (auto h : B["pythia8.2kG.root"]) h.second->SetMarkerStyle(20);
  for (auto h : C["pythia8.2kG.root"])  h.second->SetMarkerStyle(25);
  for (auto h : C["pythia8.1T.root"])  h.second->SetMarkerStyle(28);
  B["pythia8.2kG.root"]["hEffEl"]->DrawCopy("same");
  B["pythia8.2kG.root"]["hEffPi"]->DrawCopy("same");
  B["pythia8.2kG.root"]["hEffKa"]->DrawCopy("same");
  B["pythia8.2kG.root"]["hEffPr"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffEl"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffPi"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffKa"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffPr"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffEl"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffPi"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffKa"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffPr"]->DrawCopy("same");
  
  auto cEffS2 = new TCanvas("cEffS2", "cEffS2", 800, 800);
  cEffS2->DrawFrame(-2., 0., 0., 1.05, ";log_{10}(#it{p}_{T} / GeV/#it{c});efficiency");
  for (auto h : C["pythia8.1T.root"])  h.second->SetMarkerStyle(20);
  for (auto h : C["pythia8.5kG.root"]) h.second->SetMarkerStyle(25);
  for (auto h : C["pythia8.2kG.root"]) h.second->SetMarkerStyle(28);
  C["pythia8.1T.root"]["hEffEl"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffPi"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffKa"]->DrawCopy("same");
  C["pythia8.1T.root"]["hEffPr"]->DrawCopy("same");
  C["pythia8.5kG.root"]["hEffEl"]->DrawCopy("same");
  C["pythia8.5kG.root"]["hEffPi"]->DrawCopy("same");
  C["pythia8.5kG.root"]["hEffKa"]->DrawCopy("same");
  C["pythia8.5kG.root"]["hEffPr"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffEl"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffPi"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffKa"]->DrawCopy("same");
  C["pythia8.2kG.root"]["hEffPr"]->DrawCopy("same");

  
}


