#include "io.C"
#include "style.C"

void
inspect(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  auto hPdg_all = new TH1F("hPdg_all", "", 10000, -5000., 5000.);
  auto hPdg_prim = new TH1F("hPdg_prim", "", 10000, -5000., 5000.);
  
  TVector3 vec;
  
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);
    
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      auto pdg = io.tracks.pdg[itrk];
      auto px  = io.tracks.px[itrk];
      auto py  = io.tracks.py[itrk];
      auto pz  = io.tracks.pz[itrk];
      auto pt  = hypot(px, py);
      auto p   = hypot(pt, pz);
      auto eta = atanh(pz / p);
      if (fabs(eta) > 0.8) continue;

      hPdg_all->Fill(pdg);
      if (io.tracks.parent[itrk] == -1) hPdg_prim->Fill(pdg);
	
    }
  }

  auto db = TDatabasePDG::Instance();

  std::vector<std::pair<std::string, std::pair<float, float>>> counts;
  for (int i = 0; i < hPdg_all->GetNbinsX(); ++i) {
    if (hPdg_all->GetBinContent(i + 1) <= 0) continue;
    auto pdg = hPdg_all->GetBinLowEdge(i + 1);
    auto val1 = hPdg_all->GetBinContent(i + 1);
    auto val2 = hPdg_prim->GetBinContent(i + 1);
    std::string name = db->GetParticle(pdg)->GetName();
    counts.push_back( {name, {val1, val2} } );
  }

  std::sort(counts.begin(), counts.end(),
	    [](const pair<std::string, std::pair<float, float>> &l,
	       const pair<std::string, std::pair<float, float>> &r)
	    {
	      return r.second.first < l.second.first;
	    });

  auto hCounts_all = new TH1F();
  auto hCounts_prim = new TH1F();
  for (auto count : counts) {
    hCounts_all->Fill(count.first.c_str(), count.second.first);
    hCounts_prim->Fill(count.first.c_str(), count.second.second);
  }

  auto hCounts_sec = (TH1 *)hCounts_all->Clone("hCounts_sec");
  hCounts_sec->Add(hCounts_prim, -1.);
  
  hCounts_all->Sumw2();
  hCounts_all->Scale(1. / (float)nevents);

  hCounts_prim->Sumw2();
  hCounts_prim->Scale(1. / (float)nevents);

  hCounts_sec->Sumw2();
  hCounts_sec->Scale(1. / (float)nevents);

  auto c = new TCanvas("c", "c", 800, 800);
  hCounts_all->SetMarkerStyle(25);
  hCounts_all->Draw("histo,p");
  hCounts_prim->SetMarkerStyle(20);
  hCounts_prim->Draw("same,histo,p");
  hCounts_sec->SetMarkerStyle(20);
  hCounts_sec->SetMarkerColor(kRed+1);
  hCounts_sec->Draw("same,histo,p");

  
}
