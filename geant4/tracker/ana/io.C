struct {
  
  struct Hits_t {
    int    n;
    int    trkid[8192];
    float  trklen[8192];
    float  edep[8192];
    float  x[8192];
    float  y[8192];
    float  z[8192];
    float  t[8192];
    int    lyrid[8192];
  } hits;
  
  struct Tracks_t {
    int    n;
    int    id[8192];
    int    parent[8192];
    int    pdg[8192];
    float  px[8192];
    float  py[8192];
    float  pz[8192];
  } tracks;
  
  TTree *tree_hits, *tree_tracks;
    
  bool
  open(std::string filename) {
    auto fin = TFile::Open(filename.c_str());  
    std::cout << " io.open: reading data from " << filename << std::endl;
    
    tree_hits = (TTree *)fin->Get("Hits");
    tree_hits->SetBranchAddress("n"      , &hits.n);
    tree_hits->SetBranchAddress("trkid"  , &hits.trkid);
    tree_hits->SetBranchAddress("trklen" , &hits.trklen);
    tree_hits->SetBranchAddress("edep"   , &hits.edep);
    tree_hits->SetBranchAddress("x"      , &hits.x);
    tree_hits->SetBranchAddress("y"      , &hits.y);
    tree_hits->SetBranchAddress("z"      , &hits.z);
    tree_hits->SetBranchAddress("t"      , &hits.t);
    tree_hits->SetBranchAddress("lyrid"  , &hits.lyrid);
    auto tree_hits_nevents = tree_hits->GetEntries();
    
    tree_tracks = (TTree *)fin->Get("Tracks");
    tree_tracks->SetBranchAddress("n"  , &tracks.n);
    tree_tracks->SetBranchAddress("id" , &tracks.id);
    tree_tracks->SetBranchAddress("pdg" , &tracks.pdg);
    tree_tracks->SetBranchAddress("px" , &tracks.px);
    tree_tracks->SetBranchAddress("py" , &tracks.py);
    tree_tracks->SetBranchAddress("pz" , &tracks.pz);
    auto tree_tracks_nevents = tree_tracks->GetEntries();
    
    if (tree_hits_nevents != tree_tracks_nevents) {
      std::cout << " io.open: entries mismatch in trees " << std::endl
		<< "          " << tree_hits_nevents   << " events in \'Hits\' tree "   << std::endl
		<< "          " << tree_tracks_nevents << " events in \'tracks\' tree " << std::endl;
	return true;
    }
    std::cout << " io.open: successfully retrieved " << tree_tracks_nevents << " events " << std::endl;
    return false;
  }

  auto nevents() { return tree_tracks->GetEntries(); }
  void event(int iev) { tree_tracks->GetEntry(iev); tree_hits->GetEntry(iev); }
  
} io;
