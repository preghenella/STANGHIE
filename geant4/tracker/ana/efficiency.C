#include "io.C"
#include "style.C"

TH1 *efficiency(const char *fname, int layer = 9, int pdg = 211);

void
draw()
{
  style();
  
  auto hel = efficiency("e-.root", 9);
  hel->SetLineColor(kBlack);
  hel->SetMarkerColor(kBlack);
  hel->SetMarkerStyle(20);
  
  auto hpi = efficiency("pi+.root", 9);
  hpi->SetLineColor(kAzure-3);
  hpi->SetMarkerColor(kAzure-3);
  hpi->SetMarkerStyle(20);
  
  auto hka = efficiency("kaon+.root", 9);
  hka->SetLineColor(kGreen+2);
  hka->SetMarkerColor(kGreen+2);
  hka->SetMarkerStyle(20);
  
  auto hpr = efficiency("proton.root", 0);
  hpr->SetLineColor(kRed+1);
  hpr->SetMarkerColor(kRed+1);
  hpr->SetMarkerStyle(20);
  
  auto c = new TCanvas("c", "c", 800, 800);
  c->DrawFrame(3.e-2, 0., 3., 1.05, ";#it{p}_{T} (GeV/#it{c});efficiency");
  hel->Draw("same");
  hpi->Draw("same");
  hka->Draw("same");
  hpr->Draw("same");
}

TH1 *
efficiency(const char *fname, int layer, int pdg)
{
  io_open(fname);

  auto hGenPt = new TH1F("hGenPt", ";#it{p}_{T} (GeV/#it{c});hit probability;",
			 100, 0., 10.);
  
  auto hRecPt = new TH1F("hRecPt", ";#it{p}_{T} (GeV/#it{c});hit probability;",
			 100, 0., 10.);

  TVector3 vec;
  
  auto nev = tree_tracks->GetEntries();
  for (int iev = 0; iev < nev; ++iev) {

    io_event(iev);
    
    for (int itrk = 0; itrk < tracks.n; ++itrk) {

      if (pdg != 0 && abs(tracks.pdg[itrk]) != pdg) continue;

      auto px = tracks.px[itrk];
      auto py = tracks.py[itrk];
      auto pz = tracks.pz[itrk];
      vec.SetXYZ(px, py, pz);
      
      auto pt  = vec.Pt();
      auto p   = vec.Mag();
      auto eta = vec.Eta();

      if (fabs(eta) > 0.8) continue;
      
      hGenPt->Fill(pt);
      
      for (int ihit = 0; ihit < hits.n; ++ihit) { 
	
	if (hits.trkid[ihit] != tracks.id[itrk]) continue;
	if (hits.lyrid[ihit] != layer) continue;
	
	hRecPt->Fill(pt);
	break;
	
      }
    }
  }

  hRecPt->Sumw2();
  hRecPt->Divide(hRecPt, hGenPt, 1., 1., "B");

  return hRecPt;  
}
