#include "post_ana.C"
#include "style.C"

const char *formula = "[0] + [1] * pow(x, [2]) + [3] * pow(x, [4])";

void fit_mom_resolution(double xmin, double ymin, double xmax, double ymax, const char *figname);

void
fit_mom_resolution()
{
  fit_mom_resolution(0.03, 0.01, 30., 1., "fit_mom_resolution.png");
}

void
fit_mom_resolution(double xmin, double ymin, double xmax, double ymax, const char *figname)
{
  style();
  TVirtualFitter::SetMaxIterations(1.e9);
  
  // the canvas
  auto c = new TCanvas("c", "c", 900, 900);
  c->Divide(2, 2);
  
  // electrons
  
  c->cd(1)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); momentum resolution");
  c->cd(1)->SetLogx();
  c->cd(1)->SetLogy();
  c->cd(1);
  
  auto g_rp_el_beam16 = (TH1 *)resolution("pid=11_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoResPt")->Clone("g_rp_el_beam16");
  g_rp_el_beam16->SetMarkerStyle(20);
  g_rp_el_beam16->SetMarkerColor(kGray+2);
  g_rp_el_beam16->SetLineColor(kGray+2);
  g_rp_el_beam16->Draw("same");

  TF1 *f_rp_el_beam16 = new TF1("f_rp_el_beam16", formula, 0., 100.);
  f_rp_el_beam16->SetParameter(0, 0.05);
  g_rp_el_beam16->Fit(f_rp_el_beam16, "0WW");
  g_rp_el_beam16->Fit(f_rp_el_beam16, "0");
  f_rp_el_beam16->SetLineWidth(1);
  f_rp_el_beam16->SetLineColor(kBlack);
  f_rp_el_beam16->Draw("same");
  
  // pions
  
  c->cd(2)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); momentum resolution (%)");
  c->cd(2)->SetLogx();
  c->cd(2)->SetLogy();
  c->cd(2);
  
  auto g_rp_pi_beam16 = (TH1 *)resolution("pid=211_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoResPt")->Clone("g_rp_pi_beam16");
  g_rp_pi_beam16->SetMarkerStyle(20);
  g_rp_pi_beam16->SetMarkerColor(kAzure-3);
  g_rp_pi_beam16->SetLineColor(kAzure-3);
  g_rp_pi_beam16->Draw("same");
  
  TF1 *f_rp_pi_beam16 = new TF1("f_rp_pi_beam16", formula, 0., 100.);
  f_rp_pi_beam16->SetParameter(0, 0.05);
  g_rp_pi_beam16->Fit(f_rp_pi_beam16, "0WW");
  g_rp_pi_beam16->Fit(f_rp_pi_beam16, "0");
  f_rp_pi_beam16->SetLineWidth(1);
  f_rp_pi_beam16->SetLineColor(kBlack);
  f_rp_pi_beam16->Draw("same");
  
  // kaons
  
  c->cd(3)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); momentum resolution (%)");
  c->cd(3)->SetLogx();
  c->cd(3)->SetLogy();
  c->cd(3);
 
  auto g_rp_ka_beam16 = (TH1 *)resolution("pid=321_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoResPt")->Clone("g_rp_ka_beam16");
  g_rp_ka_beam16->SetMarkerStyle(20);
  g_rp_ka_beam16->SetMarkerColor(kGreen+2);
  g_rp_ka_beam16->SetLineColor(kGreen+2);
  g_rp_ka_beam16->Draw("same");
  
  TF1 *f_rp_ka_beam16 = new TF1("f_rp_ka_beam16", formula, 0., 100.);
  f_rp_ka_beam16->SetParameter(0, 0.05);
  g_rp_ka_beam16->Fit(f_rp_ka_beam16, "0WW");
  g_rp_ka_beam16->Fit(f_rp_ka_beam16, "0");
  f_rp_ka_beam16->SetLineWidth(1);
  f_rp_ka_beam16->SetLineColor(kBlack);
  f_rp_ka_beam16->Draw("same");
  
  // protons
  
  c->cd(4)->DrawFrame(xmin, ymin, xmax, ymax, ";#it{p}_{T} (GeV/c); momentum resolution (%)");
  c->cd(4)->SetLogx();
  c->cd(4)->SetLogy();
  c->cd(4);
  
  auto g_rp_pr_beam16 = (TH1 *)resolution("pid=2212_pipe=1.6_field=0.2_eta=0.5.trackerAnalysis.root", "mHistoResPt")->Clone("g_rp_pr_beam16");
  g_rp_pr_beam16->SetMarkerStyle(20);
  g_rp_pr_beam16->SetMarkerColor(kRed+1);
  g_rp_pr_beam16->SetLineColor(kRed+1);
  g_rp_pr_beam16->Draw("same");
  
  TF1 *f_rp_pr_beam16 = new TF1("f_rp_pr_beam16", formula, 0., 100.);
  f_rp_pr_beam16->SetParameter(0, 0.05);
  g_rp_pr_beam16->Fit(f_rp_pr_beam16, "0WW");
  g_rp_pr_beam16->Fit(f_rp_pr_beam16, "0");
  f_rp_pr_beam16->SetLineWidth(1);
  f_rp_pr_beam16->SetLineColor(kBlack);
  f_rp_pr_beam16->Draw("same");
 
  c->SaveAs(figname);

  std::cout << "el: " << f_rp_el_beam16->GetFormula()->GetExpFormula("params") << std::endl;
  std::cout << "pi: " << f_rp_pi_beam16->GetFormula()->GetExpFormula("params") << std::endl;
  std::cout << "ka: " << f_rp_ka_beam16->GetFormula()->GetExpFormula("params") << std::endl;
  std::cout << "pr: " << f_rp_pr_beam16->GetFormula()->GetExpFormula("params") << std::endl;

}
