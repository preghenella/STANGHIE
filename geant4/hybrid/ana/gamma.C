#include "io.C"
#include "style.C"

void
gamma(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  auto hTran = new TH1F("hTran", "", 60, -3., 3.);
  auto hComp = new TH1F("hComp", "", 60, -3., 3.);
  auto hConv = new TH1F("hConv", "", 60, -3., 3.);
    
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);
    
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      auto st  = io.tracks.status[itrk];
      auto pdg = io.tracks.pdg[itrk];
      auto px  = io.tracks.px[itrk];
      auto py  = io.tracks.py[itrk];
      auto pz  = io.tracks.pz[itrk];
      auto e   = io.tracks.e[itrk];
      auto pt  = hypot(px, py);
      auto p   = hypot(pt, pz);
      auto eta = atanh(pz / p);

      if (fabs(eta) > 0.8) continue;
      if (pdg != 22) continue;
      
      if (st & io.kTransport) {
	hTran->Fill(log10(e));
	if (st & io.kConversion) {
	  hConv->Fill(log10(e));
	}
	if (st & io.kCompton) {
	  hComp->Fill(log10(e));
	}
      }
	
    }
  }
  
  hTran->Sumw2();
  hComp->Sumw2();
  hConv->Sumw2();

  hConv->Divide(hConv, hTran, 1., 1., "B");
  hComp->Divide(hComp, hTran, 1., 1., "B");

  auto c = new TCanvas("c", "c", 800, 800);
  hConv->SetLineColor(kAzure-3);
  hConv->Draw();
  hComp->SetLineColor(kRed+1);
  hComp->Draw("same");
  
  
}
