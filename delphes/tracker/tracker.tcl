### execution path
set ExecutionPath {

    ParticlePropagator

    ElectronFilter
    ElectronTrackingEfficiency
    ElectronMomentumSmearing

    PionFilter
    PionTrackingEfficiency
    PionMomentumSmearing

    KaonFilter
    KaonTrackingEfficiency
    KaonMomentumSmearing

    ProtonFilter
    ProtonTrackingEfficiency
    ProtonMomentumSmearing

    TrackMerger
    TimeSmearing
    
    TreeWriter
}

### propagate inside the solenoid

module ParticlePropagator ParticlePropagator {
    set InputArray Delphes/stableParticles
    set OutputArray stableParticles
    set Radius 100.e-2
    set HalfLength 100.e-2
    set Bz 0.2
}

### filters

module PdgCodeFilter ElectronFilter {
    set InputArray ParticlePropagator/stableParticles
    set OutputArray electrons
    set Invert true
    add PdgCode {11}
    add PdgCode {-11}
}

module PdgCodeFilter PionFilter {
    set InputArray ParticlePropagator/stableParticles
    set OutputArray pions
    set Invert true
    add PdgCode {211}
    add PdgCode {-211}
}

module PdgCodeFilter KaonFilter {
    set InputArray ParticlePropagator/stableParticles
    set OutputArray kaons
    set Invert true
    add PdgCode {321}
    add PdgCode {-321}
}

module PdgCodeFilter ProtonFilter {
    set InputArray ParticlePropagator/stableParticles
    set OutputArray protons
    set Invert true
    add PdgCode {2212}
    add PdgCode {-2212}
}

### efficiencies

module Efficiency ElectronTrackingEfficiency {
    set InputArray ElectronFilter/electrons
    set OutputArray electrons
    set EfficiencyFormula { 0.999221+6.07572e-06*pow(pt,-5.25443)+-6.0444e-06*pow(pt,-5.25623) }
}

module Efficiency PionTrackingEfficiency {
    set InputArray PionFilter/pions
    set OutputArray pions
    set EfficiencyFormula { 0.991768+-2.33589e-16*pow(pt,-12.0768)+-0.0221524*pow(pt,-0.90131) }
}

module Efficiency KaonTrackingEfficiency {
    set InputArray KaonFilter/kaons
    set OutputArray kaons
    set EfficiencyFormula { 0.99657+-0.0636378*pow(pt,-0.8653)+-0.0636378*pow(pt,-0.8653) }
}

module Efficiency ProtonTrackingEfficiency {
    set InputArray ProtonFilter/protons
    set OutputArray protons
    set EfficiencyFormula { 0.987965+-6.11573e-05*pow(pt,-3.61847)+-6.11573e-05*pow(pt,-3.61847) }
}

### momentum resolution

module MomentumSmearing ElectronMomentumSmearing {
    set InputArray ElectronTrackingEfficiency/electrons
    set OutputArray electrons
    set ResolutionFormula { 0.0430083+0.0101011*pow(pt,0.0733751)+0.0101011*pow(pt,0.0733751) }
}

module MomentumSmearing PionMomentumSmearing {
    set InputArray PionTrackingEfficiency/pions
    set OutputArray pions
    set ResolutionFormula { 0.0586108+0.000155453*pow(pt,-1.8907)+0.000155453*pow(pt,-1.8907) }
}

module MomentumSmearing KaonMomentumSmearing {
    set InputArray KaonTrackingEfficiency/kaons
    set OutputArray kaons
    set ResolutionFormula { 0.0595104+0.00197738*pow(pt,-1.7971)+0.00197738*pow(pt,-1.7971) }
}

module MomentumSmearing ProtonMomentumSmearing {
    set InputArray ProtonTrackingEfficiency/protons
    set OutputArray protons
    set ResolutionFormula { 0.0596247+0.00720398*pow(pt,-1.54741)+0.00720398*pow(pt,-1.54741) }
}

### merger

module Merger TrackMerger {
    add InputArray ElectronMomentumSmearing/electrons
    add InputArray PionMomentumSmearing/pions
    add InputArray KaonMomentumSmearing/kaons
    add InputArray ProtonMomentumSmearing/protons
    set OutputArray tracks
}

### time smearing

module TimeSmearing TimeSmearing {
    add InputArray TrackMerger/tracks
    add OutputArray tracks
    set TimeResolution 0.020e-9
}

### tree definition

module TreeWriter TreeWriter {
    # add Branch InputArray BranchName BranchClass
    add Branch Delphes/allParticles particles GenParticle
    add Branch TimeSmearing/tracks tracks Track
}
