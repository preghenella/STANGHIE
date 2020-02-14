TH1 *
efficiency(const char *fname, const char *what = "mHistoHitPt")
{
  auto fin = TFile::Open(fname);
  auto hgen = (TH1 *)fin->Get("mHistoGenPt");
  auto hrec = (TH1 *)fin->Get(what);
  
  auto hout = (TH1 *)hrec->Clone("heffout");
  hout->Sumw2();
  hout->Divide(hout, hgen, 1., 1., "B");
  
  return hout;
}

TH1 *
resolution(const char *fname, const char *what = "mHistoResPt", const char *fitopt = "IMRE0Q", int oneBin = -1)
{
  auto fin = TFile::Open(fname);
  auto hres = (TH2 *)fin->Get(what);

  auto hout = hres->ProjectionX("hresout");
  hout->Reset();
  
  auto f = (TF1 *)gROOT->GetFunction("gaus");
  for (int i = 0; i < hres->GetNbinsX(); ++i) {
    if (oneBin != -1 && i != oneBin) continue;
    auto hpy = hres->ProjectionY("hpy", i + 1, i + 1);
    if (hpy->Integral() < 100.) continue;
    hpy->Rebin(2);
    hpy->Fit(f, fitopt);
    hpy->Fit(f, fitopt, "", f->GetParameter(1) - 3. * f->GetParameter(2), f->GetParameter(1) + 3. * f->GetParameter(2));
    hout->SetBinContent(i + 1, f->GetParameter(2));
    hout->SetBinError(i + 1, f->GetParError(2));
    if (oneBin != -1) {
      return hpy;
    }
    delete hpy;
  }
  return hout;
}
