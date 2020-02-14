#ifndef _RootIO_h_
#define _RootIO_h_

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4Event;
class G4Run;
class G4Track;
class G4Step;
class TFile;
class TTree;

class RootIO : public G4UImessenger
{
  
public:

  static RootIO *Instance() {
    if (!mInstance) mInstance = new RootIO();
    return mInstance;
  };

  void InitMessenger();
  void SetNewValue(G4UIcommand *command, G4String value);
  
  void BeginOfRunAction(const G4Run *aRun);
  void EndOfRunAction(const G4Run *aRun);
  void BeginOfEventAction(const G4Event *aEvent);
  void EndOfEventAction(const G4Event *aEvent);

  void Open(std::string filename);
  void Close();
  
  void ResetTracks();
  void FillTracks();
  void AddTrack(const G4Track *aTrack);
  
  void ResetHits();
  void FillHits();
  void AddHit(const G4Step *aStep);
    
  private:

  RootIO() = default;
  
  static RootIO *mInstance;
  std::string mFilePrefix = "tracker";
  TFile *mFile = nullptr;
  TTree *mTreeHits = nullptr;
  TTree *mTreeTracks = nullptr;

  G4UIdirectory *mDirectory;
  G4UIcmdWithAString *mFileNameCmd;
  
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
  } mHits; //!

  struct Tracks_t {
    int    n;
    int    id[8192];
    int    parent[8192];
    int    pdg[8192];
    float  e[8192];
    float  px[8192];
    float  py[8192];
    float  pz[8192];
  } mTracks; //!
  
};

#endif /** _RootIO_h_ **/
