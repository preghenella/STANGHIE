#ifndef _StackingAction_h_
#define _StackingAction_h_

#include "G4UserStackingAction.hh"

class StackingAction : public G4UserStackingAction
{

public:
  
  StackingAction() = default;
  ~StackingAction() = default;
  
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack) override;
  
protected:

};

#endif /** _StackingAction_h_ **/
