#include "GeneratorPythia8.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"

#include "TVector3.h"

/*****************************************************************/

GeneratorPythia8::GeneratorPythia8()
{
  mPythia8Directory = new G4UIdirectory("/pythia8/");

  mConfigFileNameCmd = new G4UIcmdWithAString("/pythia8/config", this);
  mConfigFileNameCmd->SetGuidance("Config filename");
  mConfigFileNameCmd->SetParameterName("filename", false);
  mConfigFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  mInitCmd = new G4UIcmdWithoutParameter("/pythia8/init", this);
  mInitCmd->SetGuidance("Initialise");
  mInitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
}

/*****************************************************************/

void
GeneratorPythia8::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mConfigFileNameCmd) {
    mPythia.readFile(value, true);
  }
  if (command == mInitCmd) {
    mPythia.init();
  }
}
  
/*****************************************************************/

GeneratorPythia8::~GeneratorPythia8()
{
}

/*****************************************************************/

void
GeneratorPythia8::GeneratePrimaryVertex(G4Event *event)
{

  mPythia.next();
  
  auto nParticles = mPythia.event.size();
  TVector3 vec;
  for (int iparticle = 0; iparticle < nParticles; iparticle++) { // first particle is system
    auto aParticle = mPythia.event[iparticle];
    if (aParticle.statusHepMC() != 1) continue;
    auto pdg = aParticle.id();
    auto px = aParticle.px() * GeV;
    auto py = aParticle.py() * GeV;
    auto pz = aParticle.pz() * GeV;
    auto et = aParticle.e() * GeV;
    auto vx = aParticle.xProd() * mm;
    auto vy = aParticle.yProd() * mm;
    auto vz = aParticle.zProd() * mm;
    auto vt = aParticle.tProd() * mm / c_light;

    /** only at mid-rapidity **/
    vec.SetXYZ(px, py, pz);
    if (fabs(vec.Eta()) > 0.8) continue;
    
    auto particle = new G4PrimaryParticle(pdg, px, py, pz, et);
    auto vertex = new G4PrimaryVertex(vx, vy, vz, vt);      
    vertex->SetPrimary(particle);
    event->AddPrimaryVertex(vertex);
  }
  
}
