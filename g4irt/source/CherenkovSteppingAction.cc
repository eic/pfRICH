
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include <G4SteppingManager.hh>
#include <G4DataInterpolation.hh>
#include "G4OpticalPhoton.hh"

#include "CherenkovSteppingAction.h"

#define _GEANT_SOURCE_CODE_
#include <CherenkovEvent.h>
#include <ReflectionPoint.h>
#include <CherenkovDetectorCollection.h>
#include <CherenkovPhotonDetector.h>
#include <G4RadiatorMaterial.h>

// -------------------------------------------------------------------------------------

CherenkovSteppingAction::CherenkovSteppingAction(CherenkovDetectorCollection *geometry, 
						   CherenkovEvent *event): 
  G4UserSteppingAction(), m_EventPtr(event), m_Geometry(geometry), m_SeconadriesDisabled(false)
{ 
} // CherenkovSteppingAction::CherenkovSteppingAction()

// -------------------------------------------------------------------------------------

double CherenkovSteppingAction::GetQE(const CherenkovPhotonDetector *pd, double eph) 
{  
  return pd->CheckQERange(eph) ? pd->GetQE()->CubicSplineInterpolation(eph) : 0.0;
} // CherenkovSteppingAction::GetQE()

// -------------------------------------------------------------------------------------

double CherenkovSteppingAction::GetAttenuationLength(const CherenkovRadiator *radiator, double eph) 
{  
  auto material = radiator->GetMaterial();

  return material ? material->AttenuationLength(eph) : 0.0;
} // CherenkovSteppingAction::GetAttenuationLength()

// -------------------------------------------------------------------------------------

double CherenkovSteppingAction::GetRefractiveIndex(const CherenkovRadiator *radiator, double eph) 
{  
  auto material = radiator->GetMaterial();

  return material ? material->RefractiveIndex(eph) : 0.0;
} // CherenkovSteppingAction::GetRefractiveIndex()

// -------------------------------------------------------------------------------------

TransientTrackInformation *CherenkovSteppingAction::AttachUserInfo(G4Track* track, TransientParticle *myself, 
								   TransientParticle *parent)
{
  auto info = new TransientTrackInformation(myself, parent);
  track->SetUserInformation(info);

  return info;
} // CherenkovSteppingAction::AttachUserInfo()

// -------------------------------------------------------------------------------------

void CherenkovSteppingAction::UserSteppingAction(const G4Step* step)
{
  static G4ParticleDefinition* opticalphoton = G4OpticalPhoton::OpticalPhotonDefinition();

  G4Track* track = step->GetTrack();
  auto tdef = track->GetDynamicParticle()->GetParticleDefinition();
  // Nothing to do with the neutrals, unless it is an optical photon;
  //+if (tdef->GetPDGEncoding() && !tdef->GetPDGCharge()) return;
  const G4ParticleDefinition* particleDef = track->GetDynamicParticle()->GetParticleDefinition();
  if (!tdef->GetPDGCharge() && particleDef != opticalphoton) return;

  auto info = dynamic_cast<TransientTrackInformation *>(track->GetUserInformation());
  // Can be a primary particle -> no structure was created so far, and no parent;
  if (!info) {
    auto particle = new ChargedParticle(tdef->GetPDGEncoding());
    info = AttachUserInfo(track, particle, 0);

    // Assume first step is from the vertex?;
    const auto &vtx = step->GetPreStepPoint();
    const auto &x = vtx->GetPosition(), &p = vtx->GetMomentum(); 
    particle->SetVertexPosition((1/mm) *TVector3(x.x(), x.y(), x.z()));
    particle->SetVertexMomentum((1/GeV)*TVector3(p.x(), p.y(), p.z()));
    particle->SetVertexTime(vtx->GetGlobalTime()/ns);

    m_EventPtr->AddChargedParticle(particle);
  } //if

  G4StepPoint *xfrom = step->GetPreStepPoint(), *xto = step->GetPostStepPoint();
  assert(xfrom && xto);
  G4VPhysicalVolume *vfrom = xfrom->GetPhysicalVolume(), *vto = xto->GetPhysicalVolume();
  if (vfrom && vto) {
    TransientParticle *pptr = info->Myself();
    auto lfrom = vfrom->GetLogicalVolume(), lto = vto->GetLogicalVolume();
    auto rfrom = m_Geometry->FindRadiator(lfrom), rto = m_Geometry->FindRadiator(lto);

    // Now it can either be a charged particle or an optical photon;
    if (pptr->IsCharged()) {
      auto particle = dynamic_cast<ChargedParticle*>(pptr);

      // Once the particle hits a mirror, stop keeping track of it;
      if (!particle->TracingIsStopped()) {
	// FIXME: this logic may require certain changes for the secondaries; for now assume 
	// that a reasonable particle can enter and exit each radiator only once; but may make steps;
	for(unsigned fb=0; fb<2; fb++) {
	  auto radiator = fb ? rto : rfrom;
	  
	  // Record only steps of interest (inside a radiator); 
	  if (!radiator) continue;

	  auto history = particle->FindRadiatorHistory(radiator);
	  if (!history) {
	    history = new RadiatorHistory();
	    particle->StartRadiatorHistory(std::make_pair(radiator, history));
	  } //if
	  
	  // No duplicate entries; assume that a step starts at the same location where the 
	  // previous step stopped (and that location was recorded already);
	  if (!fb && history->GetStepCount()) continue;
	  
	  {
	    auto point = fb ? xto : xfrom;// : xto;
	    const auto &x = point->GetPosition(), &p = point->GetMomentum(); 
	    
	    auto position = (1/mm) *TVector3(x.x(), x.y(), x.z());
	    auto momentum = (1/GeV)*TVector3(p.x(), p.y(), p.z()); 
	    auto trace = new ChargedParticleStep(position, momentum, point->GetGlobalTime()/ns);//, step->GetStepLength()/mm);
	    history->AddStep(trace);
	  } 
	} //for fb

	if (m_Geometry->FindMirror(lto)) particle->StopTracing();
      } //if
    } else {
      auto photon = dynamic_cast<OpticalPhoton*>(pptr); assert(photon);
      G4String nfrom = vfrom->GetName(), nto = vto->GetName();
      
      // Assign generic information once; FIXME: do it better later;
      if (!photon->GetVertexMomentum().Mag()) {
	auto parent = dynamic_cast<ChargedParticle*>(info->Parent()); assert(parent);

	// Vertex at birth;
	{
	  const G4ThreeVector &x = track->GetVertexPosition();
	  photon->SetVertexPosition(TVector3(x.x(), x.y(), x.z()));
	}

	// Logical volume at birth and connection to a radiator;
	{
	  double e = track->GetVertexKineticEnergy();
	  const G4ThreeVector &d = track->GetVertexMomentumDirection();
	  photon->SetVertexMomentum((e/eV)*TVector3(d.x(), d.y(), d.z()));

	  {
	    auto radiator = m_Geometry->FindRadiator(track->GetLogicalVolumeAtVertex());
	    //assert(radiator);

	    if (radiator) {
	      photon->SetVertexAttenuationLength(GetAttenuationLength(radiator, e));
	      photon->SetVertexRefractiveIndex (GetRefractiveIndex(radiator, e));

	      auto history = parent->FindRadiatorHistory(radiator);
	      // FIXME: this happens with the sensor window volumes; why?;
	      if (!history) {
		history = new RadiatorHistory();
		parent->StartRadiatorHistory(std::make_pair(radiator, history));
	      } //if
	      history->AddOpticalPhoton(photon);
	    } else {
	      if (m_Geometry->CheckBits(_STORE_ORPHAN_PHOTONS_))
		parent->AddOrphanPhoton(photon);
	    } //if
	  }
	}
      } //if
      
      // Perhaps a reflection point?;
      {
	auto mirror = m_Geometry->FindMirror(lto);

	if (m_Geometry->CheckBits(_STORE_REFLECTION_POINTS_) && mirror) {
	  G4ThreeVector xx = xto->GetPosition(), pp = xto->GetMomentum();
	  auto reflection = new ReflectionPoint(mirror, vto->GetCopyNo(), 
						TVector3(xx.x(), xx.y(), xx.z()), 
						TVector3(pp.x(), pp.y(), pp.z()));
	  
	  photon->AddReflectionPoint(reflection);
	} else {
	  if (m_Geometry->CheckBits(_STORE_REFRACTION_POINTS_) && rfrom && rto && rfrom != rto) {
	    G4ThreeVector xx = xto->GetPosition(), pfrom = xfrom->GetMomentum(), pto = xto->GetMomentum();
	    auto refraction = new RefractionPoint(rfrom, rto, //vto->GetCopyNo(), 
						  TVector3(xx.x(), xx.y(), xx.z()), 
						  TVector3(pfrom.x(), pfrom.y(), pfrom.z()),
						  TVector3(pto.x(), pto.y(), pto.z()));
	    
	    photon->AddRefractionPoint(refraction);
	  } else {
	    // Otherwise a photon detection point?;
	    auto pd = m_Geometry->FindPhotonDetector(lto);

	    if (pd) {
	      photon->SetPhotonDetector(pd);
	      
	      //photon->SetVolumeCopy(vto->GetCopyNo());
	      //>GetTouchable()
	      //printf("%2d %2d %2d %2d\n", vto->GetCopyNo(), 
	      //     xto->GetTouchable()->GetCopyNumber(), 
	      //     xto->GetTouchable()->GetCopyNumber(0), 
	      //     xto->GetTouchable()->GetCopyNumber(1));
	      // !!! FIXME: 
	      //printf("%d\n", pd->GetCopyIdentifierLevel());
	      photon->SetVolumeCopy(xto->GetTouchable()->GetCopyNumber(pd->GetCopyIdentifierLevel()));
	      
	      G4ThreeVector xx = xto->GetPosition();
	      photon->SetDetectionPosition((1/mm)*TVector3(xx.x(), xx.y(), xx.z()));
	      
	      photon->SetDetectionTime(xto->GetGlobalTime()/ns);
	      
	      // The logic behind this multiplication and division by the same number is 
	      // to select calibration photons, which originate from the same QE(lambda) 
	      // parent distribution, but do not pass the overall efficiency test;
	      if (GetQE(pd, track->GetTotalEnergy())*pd->GetScaleFactor() > G4UniformRand()) { 
		if (pd->GetGeometricEfficiency()/pd->GetScaleFactor() > G4UniformRand())
		  photon->SetDetected(true);
		else
		  photon->SetCalibrationFlag();
	      } //if

	      // For now assume that the photodetector volume absorbs all photons; FIXME: make optional;
	      track->SetTrackStatus(fStopAndKill);
	    } //if
	  } //if
	} //if
      }
    } //if
  } //if
  
    // Apply unique user info to all the new secondary tracks after this step;
  {
    G4int nSecAtRest = fpSteppingManager->GetfN2ndariesAtRestDoIt();
    G4int nSecAlong  = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
    G4int nSecPost   = fpSteppingManager->GetfN2ndariesPostStepDoIt();
    G4int nSecTotal  = nSecAtRest+nSecAlong+nSecPost;
    G4TrackVector* secVec = fpSteppingManager->GetfSecondary();   
    
    //printf("   %3d %3d\n", nSecTotal, secVec->size());
    for(unsigned iq=secVec->size()-nSecTotal; iq<secVec->size(); iq++) {
      G4Track *strack = (*secVec)[iq];
      
      {
	const G4ParticleDefinition* particleDef = strack->GetDynamicParticle()->GetParticleDefinition();

	auto sdef = strack->GetDynamicParticle()->GetParticleDefinition();
	//if (m_SeconadriesDisabled && sdef->GetPDGEncoding() != 0) {
	// FIXME: why 'sdef' does not work here?;
	if (m_SeconadriesDisabled && particleDef != opticalphoton) {
	  strack->SetTrackStatus(fStopAndKill);
	  continue;
	} //if

	if (xfrom && xfrom->GetProcessDefinedStep()) {
	  TransientParticle *pptr = info->Myself();
	  auto particle = dynamic_cast<ChargedParticle*>(pptr);
	  if (particle && xfrom && xfrom->GetProcessDefinedStep()->GetProcessType() == 2) 
	    particle->m_HadronicInteractionOccured = true;

	  //printf("%3d -> %5d (%3d) --> %s\n", iq, xfrom->GetProcessDefinedStep()->GetProcessType(),
	  //	 xfrom->GetProcessDefinedStep()->GetProcessSubType(),
	  //	 xfrom->GetProcessDefinedStep()->GetProcessName().c_str());
	} //if

	// Keep track of charged particles (if secondaries are enabled) and of the optical photons;
	  
	// FIXME: why 'sdef' does not work here?;
	if ( particleDef == opticalphoton) {
	  auto lfrom = vfrom->GetLogicalVolume();
	  auto rfrom = m_Geometry->FindRadiator(lfrom);
	  if (rfrom && !rfrom->OpticalPhotonGenerationEnabled())
	    strack->SetTrackStatus(fStopAndKill);
	  else {
	    auto photon = new OpticalPhoton();
	    {
	      G4ThreeVector pparent = xto->GetMomentum();
	      
	      photon->SetVertexTime(xto->GetGlobalTime()/ns);
	      photon->SetVertexParentMomentum((1/GeV)*TVector3(pparent.x(), pparent.y(), pparent.z()));
	    }
	    
	    AttachUserInfo(strack, photon, info->Myself());
	  } //if
	} else {
	  if (sdef->GetPDGCharge()) {
	    auto particle = new ChargedParticle(sdef->GetPDGEncoding());
	    AttachUserInfo(strack, particle, info->Myself());

	    m_EventPtr->AddChargedParticle(particle);
	  } //if
	} //if
      }
    } //for iq
  } 
} // CherenkovSteppingAction::UserSteppingAction()

// -------------------------------------------------------------------------------------
