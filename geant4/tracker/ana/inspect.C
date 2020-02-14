#include "io.C"
#include "style.C"

TH1 *
inspect(const char *fname, bool primary = true)
{
  io_open(fname);

  auto hPdg = new TH1F("hPdg", "", 10000, -5000., 5000.);
  
  TVector3 vec;
  
  auto nev = tree_tracks->GetEntries();
  for (int iev = 0; iev < nev; ++iev) {

    io_event(iev);
    
    for (int itrk = 0; itrk < tracks.n; ++itrk) {

      auto parent = tracks.parent[itrk];
      if (primary && parent > 0) continue;

      auto px     = tracks.px[itrk];
      auto py     = tracks.py[itrk];
      auto pz     = tracks.pz[itrk];
      vec.SetXYZ(px, py, pz);
      
      auto pt  = vec.Pt();
      auto p   = vec.Mag();
      auto eta = vec.Eta();

      if (fabs(eta) > 0.8) continue;
      
      auto pdg    = tracks.pdg[itrk];

      hPdg->Fill(pdg);
      
    }
  }

  int charged = 0.;
  std::vector<std::pair<std::string, float>> counts;
  auto db = TDatabasePDG::Instance();
  for (int i = 0; i < hPdg->GetNbinsX(); ++i) {
    if (hPdg->GetBinContent(i + 1) <= 0) continue;
    auto pdg = hPdg->GetBinLowEdge(i + 1);
    auto val = hPdg->GetBinContent(i + 1);
    std::string name = db->GetParticle(pdg)->GetName();
    if (db->GetParticle(pdg)->Charge() != 0.)
      charged += val;
    counts.push_back( {name, val} );
  }

  std::sort(counts.begin(), counts.end(),
	    [](const pair<std::string, float> &l, const pair<std::string, float> &r)
	    {
	      return r.second < l.second;
	    });

  auto hCounts = new TH1F();
  for (auto count : counts) {
    for (int i = 0; i < count.second; ++i)
      hCounts->Fill(count.first.c_str(), 1.);
  }

  hCounts->Sumw2();
  hCounts->Scale(1. / (float)nev);

  std::cout << (float)charged / (float)nev << " charged particles in |eta| < 0.8 " << std::endl;
  
  return hCounts;
}
