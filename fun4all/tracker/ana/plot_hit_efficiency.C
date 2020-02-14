#include "post_ana.C"
#include "style.C"

TGraph *remove_empty(TGraph *g) {
  TGraph *gout = new TGraph();
  for (int i = 0; i < g->GetN(); ++i) {
    if (g->GetY()[i] <= 0.001) continue;
    gout->SetPoint(gout->GetN(), g->GetX()[i], g->GetY()[i]);
  }
  return gout;
}

void plot_hit_efficiency(double xmin, double ymin, double xmax, double ymax, const char *figname);

void
plot_hit_efficiency()
{
  plot_hit_efficiency(0.03, 10., 30., 110., "plot_hit_efficiency.png");
}

void
plot_hit_efficiency(double xmin, double ymin, double xmax, double ymax, const char *figname)
{
  style();

  // Ruben's results
  auto f_rs = TFile::Open("rs.eff.root");

  // the canvas
  auto c = new TCanvas("c", "c", 900, 900);
  c->Divide(2, 2);
  
  // electrons
  
  c->cd(1)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); efficiency (%)");
  c->cd(1)->SetLogx();
  //  c->cd(1)->SetLogy();
  c->cd(1);
  
  auto g_rp_el_beam16 = (TH1 *)efficiency("pid=11_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_el_beam16");
  g_rp_el_beam16->Scale(1.e2);
  g_rp_el_beam16->SetMarkerStyle(20);
  g_rp_el_beam16->SetMarkerColor(kGray+2);
  g_rp_el_beam16->SetLineColor(kGray+2);
  g_rp_el_beam16->Draw("same");
  
  auto g_rp_el_beam29 = (TH1 *)efficiency("pid=11_pipe=2.9_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_el_beam29");
  g_rp_el_beam29->Scale(1.e2);
  g_rp_el_beam29->SetMarkerStyle(25);
  g_rp_el_beam29->SetMarkerColor(kGray+2);
  g_rp_el_beam29->SetLineColor(kGray+2);
  g_rp_el_beam29->Draw("same");

  auto g_rs_el_beam16 = (TGraph *)f_rs->Get("el5grEff1");
  g_rs_el_beam16 = remove_empty(g_rs_el_beam16);
  g_rs_el_beam16->SetLineColor(kBlack);
  g_rs_el_beam16->SetLineStyle(kSolid);
  g_rs_el_beam16->Draw("samec");

  auto g_rs_el_beam29 = (TGraph *)f_rs->Get("el5bspgrEff1");
  g_rs_el_beam29 = remove_empty(g_rs_el_beam29);
  g_rs_el_beam29->SetLineColor(kBlack);
  g_rs_el_beam29->SetLineStyle(kDashed);
  g_rs_el_beam29->Draw("samec");

  // pions
  
  c->cd(2)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); efficiency (%)");
  c->cd(2)->SetLogx();
  //  c->cd(2)->SetLogy();
  c->cd(2);
  
  auto g_rp_pi_beam16 = (TH1 *)efficiency("pid=211_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_pi_beam16");
  g_rp_pi_beam16->Scale(1.e2);
  g_rp_pi_beam16->SetMarkerStyle(20);
  g_rp_pi_beam16->SetMarkerColor(kAzure-3);
  g_rp_pi_beam16->SetLineColor(kAzure-3);
  g_rp_pi_beam16->Draw("same");
  
  auto g_rp_pi_beam29 = (TH1 *)efficiency("pid=211_pipe=2.9_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_pi_beam29");
  g_rp_pi_beam29->Scale(1.e2);
  g_rp_pi_beam29->SetMarkerStyle(25);
  g_rp_pi_beam29->SetMarkerColor(kAzure-3);
  g_rp_pi_beam29->SetLineColor(kAzure-3);
  g_rp_pi_beam29->Draw("same");

  auto g_rs_pi_beam16 = (TGraph *)f_rs->Get("pi5grEff1");
  g_rs_pi_beam16 = remove_empty(g_rs_pi_beam16);
  g_rs_pi_beam16->SetLineColor(kBlack);
  g_rs_pi_beam16->SetLineStyle(kSolid);
  g_rs_pi_beam16->Draw("samec");

  auto g_rs_pi_beam29 = (TGraph *)f_rs->Get("pi5bspgrEff1");
  g_rs_pi_beam29 = remove_empty(g_rs_pi_beam29);
  g_rs_pi_beam29->SetLineColor(kBlack);
  g_rs_pi_beam29->SetLineStyle(kDashed);
  g_rs_pi_beam29->Draw("samec");
  
  // kaons
  
  c->cd(3)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); efficiency (%)");
  c->cd(3)->SetLogx();
  //  c->cd(3)->SetLogy();
  c->cd(3);
 
  auto g_rp_ka_beam16 = (TH1 *)efficiency("pid=321_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_ka_beam16");
  g_rp_ka_beam16->Scale(1.e2);
  g_rp_ka_beam16->SetMarkerStyle(20);
  g_rp_ka_beam16->SetMarkerColor(kGreen+2);
  g_rp_ka_beam16->SetLineColor(kGreen+2);
  g_rp_ka_beam16->Draw("same");
  
  auto g_rp_ka_beam29 = (TH1 *)efficiency("pid=321_pipe=2.9_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_ka_beam29");
  g_rp_ka_beam29->Scale(1.e2);
  g_rp_ka_beam29->SetMarkerStyle(25);
  g_rp_ka_beam29->SetMarkerColor(kGreen+2);
  g_rp_ka_beam29->SetLineColor(kGreen+2);
  g_rp_ka_beam29->Draw("same");

  // protons
  
  c->cd(4)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); efficiency (%)");
  c->cd(4)->SetLogx();
  //  c->cd(4)->SetLogy();
  c->cd(4);
  
  auto g_rp_pr_beam16 = (TH1 *)efficiency("pid=2212_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_pr_beam16");
  g_rp_pr_beam16->Scale(1.e2);
  g_rp_pr_beam16->SetMarkerStyle(20);
  g_rp_pr_beam16->SetMarkerColor(kRed+1);
  g_rp_pr_beam16->SetLineColor(kRed+1);
  g_rp_pr_beam16->Draw("same");
  
  auto g_rp_pr_beam29 = (TH1 *)efficiency("pid=2212_pipe=2.9_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoHitPt")->Clone("g_rp_pr_beam29");
  g_rp_pr_beam29->Scale(1.e2);
  g_rp_pr_beam29->SetMarkerStyle(25);
  g_rp_pr_beam29->SetMarkerColor(kRed+1);
  g_rp_pr_beam29->SetLineColor(kRed+1);
  g_rp_pr_beam29->Draw("same");

  auto g_rs_pr_beam16 = (TGraph *)f_rs->Get("p5grEff1");
  g_rs_pr_beam16 = remove_empty(g_rs_pr_beam16);
  g_rs_pr_beam16->SetLineColor(kBlack);
  g_rs_pr_beam16->SetLineStyle(kSolid);
  g_rs_pr_beam16->Draw("samec");

  auto g_rs_pr_beam29 = (TGraph *)f_rs->Get("p5bspgrEff1");
  g_rs_pr_beam29 = remove_empty(g_rs_pr_beam29);
  g_rs_pr_beam29->SetLineColor(kBlack);
  g_rs_pr_beam29->SetLineStyle(kDashed);
  g_rs_pr_beam29->Draw("samec");

  c->SaveAs(figname);
}
