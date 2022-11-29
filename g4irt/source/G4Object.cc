
#include "G4Material.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4CSGSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include <G4Colour.hh>
#include <G4VisAttributes.hh>
#include <G4IntersectionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>

//#include <tuning.h>

#define _GEANT_SOURCE_CODE_
#include <G4Object.h>

// -------------------------------------------------------------------------------------

void G4Object::DefineIntersectionCut(G4VSolid *cut, G4RotationMatrix *rotation, 
				     const G4ThreeVector &translation, bool flip) 
{
  auto old = m_Solid;

  // FIXME: there is no memory leak here, but need to change the name;
  m_Solid = new G4IntersectionSolid(m_Solid->GetName().data(), flip ? cut : m_Solid, 
				    flip ? m_Solid : cut, rotation, translation); 
  old->SetName("OLD");
} // G4Object::DefineIntersectionCut()

// -------------------------------------------------------------------------------------

void G4Object::DefineSubtractionCut(G4VSolid *cut, G4RotationMatrix *rotation, 
				    const G4ThreeVector &translation, bool flip)
{
  auto old = m_Solid;
  // FIXME: there is no memory leak here, but need to change the name;
  m_Solid = new G4SubtractionSolid(m_Solid->GetName().data(), flip ? cut : m_Solid, 
				    flip ? m_Solid : cut, rotation, translation);

  old->SetName("OLD");
} // G4Object::DefineSubtractionCut()

// -------------------------------------------------------------------------------------

void G4Object::DefineUnion(G4VSolid *solid)
{
  auto old = m_Solid;
  // FIXME: there is no memory leak here, but need to change the name;
  m_Solid = new G4UnionSolid(m_Solid->GetName().data(), m_Solid, solid);
  old->SetName("OLD");
} // G4Object::DefineUnion()

// -------------------------------------------------------------------------------------

void G4Object::SetColor(const G4Colour &color) 
{
  G4VisAttributes* visAtt = new G4VisAttributes(color);
  visAtt->SetVisibility(true);
  visAtt->SetForceSolid(true);
  
  {
    if (!m_LogicalVolume) DefineLogicalVolume();
    m_LogicalVolume->SetVisAttributes(visAtt);
  }
} // G4Object::SetColor()

// -------------------------------------------------------------------------------------
#if 0
G4VPhysicalVolume *G4Object::PlaceCopy(G4VPhysicalVolume *mother, G4RotationMatrix *rotation, 
				       const G4ThreeVector &translation) 
{
  if (!m_LogicalVolume) DefineLogicalVolume();
  
  G4VPhysicalVolume *phys = 
    new G4PVPlacement(rotation, translation, m_LogicalVolume, m_Solid->GetName(), 
		      //mother->GetLogicalVolume(), false, m_CopyCounter++);
		      mother->GetLogicalVolume(), false, m_Copies.size());//yCounter++);
  //m_Copies.push_back(new G4ObjectCopy(phys));
  m_Copies.push_back(CreateCopy(phys));

  //ExtraCall(mother, rotation, translation, phys);
#if 1//_MOVED_  
  if (m_MirrorSurface)
    // Do I really need them separately?;
    //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
    new G4LogicalBorderSurface(m_Solid->GetName(), mother, phys, m_MirrorSurface);
#endif  

  return phys;
} // G4Object::PlaceCopy()
#endif
// -------------------------------------------------------------------------------------

void G4Object::PlaceWedgeCopies(G4VPhysicalVolume *mother) 
{
  if (!m_LogicalVolume) DefineLogicalVolume();
#if 0
  {
    TVector3 nx(1,0,0), axis(0,0,1);
    //nx.Rotate(M_PI/2, axis);
    nx.RotateZ(M_PI/2);//, axis);
    //printf("@R@ %5.2f %5.2f\n", nx.x(), nx.y());
  }
  {
    CLHEP::Hep3Vector nx(1,0,0), out = CLHEP::HepRotationZ(M_PI/2) * nx;
    
    printf("@R@ %5.2f %5.2f\n", out.x(), out.y());
  }
#endif

  for(int iq=0; iq<6; iq++) {
    //if (iq != 1 && iq != 4) continue;
    //+G4RotationMatrix *rZ = new G4RotationMatrix(CLHEP::HepRotationZ((iq-0)*60*degree));
    G4RotationMatrix *rZ = new G4RotationMatrix(CLHEP::HepRotationZ(-(iq-1)*60*CLHEP::degree));
    //G4RotationMatrix *rZ = new G4RotationMatrix(CLHEP::HepRotationZ(-90*degree));

    G4VPhysicalVolume *phys = new G4PVPlacement(rZ, G4ThreeVector(0,0,0), m_LogicalVolume, m_Solid->GetName(), 
						mother->GetLogicalVolume(), false, m_Copies.size());
    m_Copies.push_back(CreateCopy(phys));
    
    // Can not place GEANT call into the Mirror class; do it here if needed;
    {
      auto msurface = GetMirrorSurface();

      if (msurface)
	// Do I really need them separately?;
	//char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
	new G4LogicalBorderSurface(m_Solid->GetName(), mother, phys, msurface);
    }
  } //for iq

  AdjustWedgeCopies(mother);
} // G4Object::PlaceWedgeCopies()

// -------------------------------------------------------------------------------------

  // This call is deferred to the moment after all cuts on this solid are applied;
void G4Object::DefineLogicalVolume( void ) 
{
  if (!m_LogicalVolume)
    m_LogicalVolume = new G4LogicalVolume(m_Solid, m_Material, m_Solid->GetName(), 0, 0, 0);
} // G4Object::DefineLogicalVolume()

// -------------------------------------------------------------------------------------
