#include "StackingAction.hh"
#include "G4Track.hh"
#include "G4NeutrinoE.hh"

#include "RootIO.hh"

/******************************************************************************/

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track *aTrack)
{

  // keep primary particle
  if (aTrack->GetParentID() == 0) {
    RootIO::Instance()->AddTrack(aTrack);
    return fUrgent;
  }

  // kill the rest
  return fKill;
  
  // kill secondary neutrino
  if (aTrack->GetDefinition() == G4NeutrinoE::NeutrinoE()) return fKill;
  else return fUrgent;
  
}

/******************************************************************************/

