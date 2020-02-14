#include "io.C"
#include "style.C"

const double c_light = TMath::C() * 1.e2 / 1.e9;

const double p_reso = 0.01; // relative
const double time_reso = 0.02; // ps

TH1 *tof(const char *fname, int layer1 = 4, int layer2 = 9);

void
draw(const char *fname, int layer1 = 4, int layer2 = 9)
{
  style();
  gStyle->SetOptStat(0);
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  c->cd(1)->SetLogz();
  c->cd(1)->SetGridx();
  tof(fname, -layer1, layer2)->Draw("col");
  c->cd(2)->SetLogz();
  c->cd(2)->SetGridx();
  tof(fname, layer1, -layer2)->Draw("col");
  c->cd(3)->SetLogz();
  c->cd(3)->SetGridx();
  tof(fname, layer1, layer2)-> Draw("col");
}


TH1 *
tof(const char *fname, int layer1, int layer2)
{
  io_open(fname);
  
  auto hBetaP = new TH2F("hBetaP", ";log_{10}(#it{p_{gen}}/(GeV/#it{c}));v/#it{c};",
			 400, -2., 2., 250, 0., 1.25);
  auto hMassP = new TH2F("hMassP", ";log_{10}(#it{p_{gen}}/(GeV/#it{c}));m^{2} (GeV^{2}/#it{c}^{4});",
			 400, -2., 2., 250, -0.1, 1.4);

  auto hTOFTracks = new TH1F("hTOFTracks", "", 500, 0., 500);
  
  TVector3 vec;
  auto nev = tree_tracks->GetEntries();
  std::cout << " loop over events: " << nev << std::endl;
  for (int iev = 0; iev < nev; ++iev) {

    io_event(iev);
    int ntoftracks = 0;

    // at first iteration, count number of TOF tracks
    // at second iteration, fill the histograms  
    for (int iiter = 0 ; iiter < 2; ++iiter) {

      auto start_time_reso = ntoftracks > 1 ? time_reso / sqrt(ntoftracks - 1) : 0.200;
      if (iiter == 1) hTOFTracks->Fill(ntoftracks);

      //      std::cout << ntoftracks << std::endl;
      
      for (int itrk = 0; itrk < tracks.n; ++itrk) {
	
	auto parent = tracks.parent[itrk];
	if (parent > 0) continue;

	auto px = tracks.px[itrk];
	auto py = tracks.py[itrk];
	auto pz = tracks.pz[itrk];
	vec.SetXYZ(px, py, pz);
	
	auto pt  = vec.Pt();
	auto p   = vec.Mag();
	auto eta = vec.Eta();
	
	if (fabs(eta) > 0.8) continue;

	double time1, length1, time2, length2;
	bool hastof1 = false, hastof2 = false;
	/** search for hits on TOF layers **/
	for (int ihit = 0; ihit < hits.n; ++ihit) { 
	  if (hits.trkid[ihit] != tracks.id[itrk]) continue;
	  if (!hastof1 && hits.lyrid[ihit] == layer1) { // hit on TOF1, save and continue
	    time1 = gRandom->Gaus(hits.t[ihit], time_reso);
	    length1 = hits.trklen[ihit];
	    hastof1 = true;
	    continue;
	  }
	  if (!hastof2 && hits.lyrid[ihit] == layer2) { // hit on TOF2, save and break
	    time2 = gRandom->Gaus(hits.t[ihit], time_reso);
	    length2 = hits.trklen[ihit];
	    hastof2 = true;
	    break;
	  }
	}
	if (!hastof1 && !hastof2) continue;
	
	if (iiter == 0) {
	  //	  std::cout << tracks.pdg[itrk] << std::endl;
	  ntoftracks++;
	  continue;
	}
     	
	/** we have two possible ways for the time-of-flight
	 ** 1) we use the start time of the event and the TOF time
	 ** 2) we use the two TOF layers for start and stop
	 ** and we pick the method with the best resolution for PID
	 **/

	auto tflight_12 = 0.;
	auto tflight_12_reso = 1.e6;
	auto length_12 = 0.;
	if (hastof1 && hastof2) {
	  tflight_12_reso = time_reso * sqrt(2.);
	  tflight_12      = time2 - time1;
	  length_12       = length2 - length1;
	}

	auto tflight = 0.;
	auto tflight_reso = 1.e6;
	auto length = 0.;
	if (hastof2) {
	  tflight_reso = sqrt(time_reso * time_reso + start_time_reso * start_time_reso);
	  tflight = gRandom->Gaus(time2, start_time_reso);
	  length = length2;
	}
	else if (hastof1) {
	  tflight_reso = sqrt(time_reso * time_reso + start_time_reso * start_time_reso);
	  tflight = gRandom->Gaus(time1, start_time_reso);
	  length = length1;
	}

	//	std::cout << tflight_12_reso << "\t" << tflight_reso << std::endl;
	
	if (tflight_12_reso < tflight_reso) {
	  tflight = tflight_12;
	  length = length_12;
	}
	
	auto p_rec = gRandom->Gaus(p, p_reso * p);     // 10% momentum smearing
	auto beta = length / (tflight * c_light);	
	auto mass2 = p_rec * p_rec * (1. / (beta * beta) - 1.);
	hBetaP->Fill(log10(p_rec), beta);
      }

    }
  }

  return hBetaP;
}

