#include "PHG4CrystalCalorimeterDetector.h"
#include "PHG4CylinderGeomContainer.h"
#include "PHG4CylinderGeomv3.h"

#include <g4main/PHG4Utils.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <fun4all/getClass.h>

#include <Geant4/G4AssemblyVolume.hh>
#include <Geant4/G4IntersectionSolid.hh>
#include <Geant4/G4SubtractionSolid.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4Box.hh>
#include <Geant4/G4ExtrudedSolid.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4TwoVector.hh>
#include <Geant4/G4Trap.hh>
#include <Geant4/G4Cons.hh>
#include <Geant4/G4Box.hh>
#include <Geant4/G4Trd.hh>

#include <Geant4/G4VisAttributes.hh>
#include <Geant4/G4Colour.hh>

#include <cmath>
#include <sstream>

#include <iostream>
#include <fstream>
#include <cstdlib>


using namespace std;

//static double no_overlap = 0.00015 * cm; // added safety margin against overlaps by using same boundary between volumes


//_______________________________________________________________________
PHG4CrystalCalorimeterDetector::PHG4CrystalCalorimeterDetector( PHCompositeNode *Node, const std::string &dnam ):
  PHG4Detector(Node, dnam),
  _place_in_x(0.0*mm),
  _place_in_y(0.0*mm),
  _place_in_z(-1080.0*mm),
  _rot_in_x(0.0),
  _rot_in_y(M_PI),
  _rot_in_z(0.0),
  _rMin1(22*mm),
  _rMax1(656*mm),
  _rMin2(26*mm),
  _rMax2(775*mm),
  _dZ(180*mm),
  _sPhi(0),
  _dPhi(2*M_PI),
  _crystal_front_dx(22*mm),
  _crystal_front_dy(22*mm),
  _crystal_dz(180*mm),
  _materialCrystal( "G4_PbWO4" ),
  _active(1),
  _crystallogicnameprefix("eEcalCrystal"),
  _inputFile( "/direct/phenix+u/jlab/github/sPHENIX-Fork/coresoftware/simulation/g4simulation/g4detectors/mapping/CrystalCalorimeter_Mapping_TEST.txt" )
{

}


//_______________________________________________________________________
PHG4CrystalCalorimeterDetector::~PHG4CrystalCalorimeterDetector()
{}


//_______________________________________________________________________
int
PHG4CrystalCalorimeterDetector::IsInCrystalCalorimeter(G4VPhysicalVolume * volume) const
{
  if (volume->GetName().find(_crystallogicnameprefix) != string::npos)
    {
      return 1;
    }
  return 0;
}


//_______________________________________________________________________
void
PHG4CrystalCalorimeterDetector::Construct( G4LogicalVolume* logicWorld )
{
  /* Create the cone envelope = 'world volume' for the crystal calorimeter */
  G4Material* Air = G4Material::GetMaterial("G4_AIR");

  G4VSolid* ecal_envelope_cone = new G4Cons("eEcal_envelope_solid",
					    _rMin1, _rMax1,
					    _rMin2, _rMax2,
					    _dZ/2.,
					    _sPhi, _dPhi );

  G4LogicalVolume* ecal_envelope_log =  new G4LogicalVolume(ecal_envelope_cone, Air, G4String("eEcal_envelope"), 0, 0, 0);

  /* Define visualization attributes for envelope cone */
  G4VisAttributes* ecalVisAtt = new G4VisAttributes();
  ecalVisAtt->SetVisibility(true);
  ecalVisAtt->SetForceSolid(false);
  ecalVisAtt->SetColour(G4Colour::Magenta());
  ecal_envelope_log->SetVisAttributes(ecalVisAtt);

  /* Define rotation attributes for envelope cone */
  G4RotationMatrix ecal_rotm;
  ecal_rotm.rotateX(_rot_in_x);
  ecal_rotm.rotateY(_rot_in_y);
  ecal_rotm.rotateZ(_rot_in_z);

  /* Place envelope cone in simulation */
  new G4PVPlacement( G4Transform3D(ecal_rotm, G4ThreeVector(_place_in_x, _place_in_y, _place_in_z) ),
		     ecal_envelope_log, "CrystalCalorimeter", logicWorld, 0, false, overlapcheck);

  /* Construct crystal calorimeter within envelope */
  ConstructCrystals(ecal_envelope_log);

  return;
}


//_______________________________________________________________________
int
PHG4CrystalCalorimeterDetector::ConstructCrystals(G4LogicalVolume* ecalenvelope)
{

	G4int NumberOfLines;		 			//Number of crystals to be created.
	const G4int NumberOfIndices = 7; 			//Different dimensions needed for crystal placement
	const string FileName = _inputFile.c_str();		//File in which crystal positions are stored
	
	G4int j_cry, k_cry;					//Indices for matrix
	G4int j_idx, k_idx;					//Indices of each crstals
	G4double x_cent, y_cent, z_cent, r_theta, r_phi;	//Coordinates of crystal in [x,y,z,theta,phi]
	G4double dx1 = 50.19*mm; //12.188*mm;			//Half of the extent of the front face of the trapezoid in x
	G4double dx2 = 59.3154545455*mm; //13.702*mm;		//Half of the extent of the back face of the trapezoid in x
	G4double dy1 = dx1;					//Half of the extent of the front face of the trapezoid in y
	G4double dy2 = dx2;					//Half of the extent of the back face of the trapezoid in y
	G4double dz =  90.000*mm;				//Half of the extent of the crystal in z

	//Create single crystal

	G4VSolid* crystal_solid = new G4Trd(G4String("eEcal_crystal"),
		dx1,						//Half length on the small face in x
		dx2,						//Half length on the large face in x
		dy1,						//Half length on the small face in y
		dy2,						//Half length on the large face in y
		dz);						//Half length in z

	G4Material* material_crystal = G4Material::GetMaterial(_materialCrystal.c_str());

	G4LogicalVolume *crystal_logic = new G4LogicalVolume( crystal_solid,
		material_crystal,
		"eEcal_crystal",
		0, 0, 0);

	G4VisAttributes *visattchk = new G4VisAttributes();
	visattchk->SetVisibility(true);
	visattchk->SetForceSolid(true);
	visattchk->SetColour(G4Colour::Cyan());
	crystal_logic->SetVisAttributes(visattchk);

	ostringstream name;

	ifstream datafile;

	if (!datafile.is_open())
	{
		datafile.open(FileName.c_str());
		if(!datafile)
		{
			cerr << endl << "*******************************************************************" << endl;
			cerr << "ERROR: Failed to open " << FileName <<" --- Exiting program." << endl;
			cerr << "*******************************************************************" << endl << endl;
			exit(1);
		}
	}

	//Determine the Number of Crystals to be Created
	NumberOfLines = 0;
	ifstream in(_inputFile.c_str());
	std::string unused;
	while ( std::getline(in, unused) )
	   ++NumberOfLines;
	cout << "Number of Crystals: " << NumberOfLines << endl;			

	j_cry = NumberOfLines; 		// = Number of Crystals
	k_cry = NumberOfIndices; 	// = j, k, x, y, z, alpha, beta.
	
	cout << "We will now create a " << NumberOfLines << " by " << k_cry << " matrix." << endl;
	
	double Crystals[j_cry][k_cry];

	cout << "Done! Now filling the matrix" << endl;
	
	G4int j = 0;
	G4int k = 0;

	while (j_cry > j) {
		while (k_cry > k) {
			datafile >> Crystals[j][k];
			k++;
		}
		j++;
		k = 0;
	}
	
	cout << "Array Filled. Now Constructing Crystals." << endl;
	
	//Write out matrix to confirm correct input file
	for(j = 0; j < j_cry; ++j) {
		for(k = 0; k < k_cry; ++k) 
			cout << Crystals[j][k] << "|";	
		cout << endl;
	}
		
	//Construct and place each crystal

	//G4RotationMatrix *Rot = new G4RotationMatrix(); //rotation matrix for the placement of each crystal
	
	//Second Quadrant
	j = 0;
	while (j_cry > j) {
		j_idx = Crystals[j][0];
		k_idx = Crystals[j][1];
		x_cent = Crystals[j][2];
		y_cent = Crystals[j][3];
		z_cent = Crystals[j][4];
		r_theta = Crystals[j][5];	//Rotation in Horizontal
		r_phi = Crystals[j][6];	//Rotation in Vertical

		G4ThreeVector Crystal_Center = G4ThreeVector(x_cent*mm, y_cent*mm, z_cent*mm);

		//std::cout << "Center Point of Crystal in z is: " << z_cent << endl;
		//std::cout << " | " << j_idx << " | " << k_idx << " | " << x_cent << " | " << y_cent << " | " << z_cent << " | " << r_theta << " | " << r_phi << " | " << endl;

		G4RotationMatrix *Rot = new G4RotationMatrix(); //rotation matrix for the placement of each crystal
		Rot->rotateX(r_phi*rad);
		Rot->rotateY(r_theta*rad);
		Rot->rotateZ(0*rad);

		name.str("");
		name << _crystallogicnameprefix << "_j_" << j_idx << "_k_" << k_idx;
		
		new G4PVPlacement( Rot, Crystal_Center,
			crystal_logic,
			name.str().c_str(),
			ecalenvelope,
			0, 0, overlapcheck);

		j_idx = k_idx = 0;
		x_cent = y_cent = z_cent = r_theta = r_phi = 0.0;

		j++;
	}


	//First Quadrant
        j = 0;
        while (j_cry > j) {
                j_idx = -1 * Crystals[j][0];
                k_idx = Crystals[j][1];
                x_cent = -1.0 * Crystals[j][2];
                y_cent = Crystals[j][3];
                z_cent = Crystals[j][4];
                r_theta = -1.0*Crystals[j][5];
                r_phi = Crystals[j][6];

                std::cout << " | " << j_idx << " | " << k_idx << " | " << x_cent << " | " << y_cent << " | " << z_cent << " | " << r_theta << " | " << r_phi << " | " << endl;

                G4ThreeVector Crystal_Center = G4ThreeVector(x_cent*mm, y_cent*mm, z_cent*mm);

                G4RotationMatrix *Rot = new G4RotationMatrix(); //rotation matrix for the placement of each crystal
	        Rot->rotateX(r_phi*rad);
                Rot->rotateY(r_theta*rad);
                Rot->rotateZ(0*rad);

                name.str("");
                name << _crystallogicnameprefix << "_j_" << j_idx << "_k_" << k_idx;

                new G4PVPlacement( Rot, Crystal_Center,
                        crystal_logic,
                        name.str().c_str(),
                        ecalenvelope,
                        0, 0, overlapcheck);

                j_idx = k_idx = 0;
                x_cent = y_cent = z_cent = r_theta = r_phi = 0.0;

                j++;
        }

	//Fourth Quadrant
        j = 0;
        while (j_cry > j) {
                j_idx = -1 * Crystals[j][0];
                k_idx = -1 * Crystals[j][1];
                x_cent = -1.0 * Crystals[j][2];
                y_cent = -1.0 * Crystals[j][3];
                z_cent = Crystals[j][4];
                r_theta = -1.0*Crystals[j][5];
                r_phi = -1.0*Crystals[j][6];

                G4ThreeVector Crystal_Center = G4ThreeVector(x_cent*mm, y_cent*mm, z_cent*mm);

		G4RotationMatrix *Rot = new G4RotationMatrix(); //rotation matrix for the placement of each crystal
                Rot->rotateX(r_phi*rad);
                Rot->rotateY(r_theta*rad);
                Rot->rotateZ(0*rad);

                name.str("");
                name << _crystallogicnameprefix << "_j_" << j_idx << "_k_" << k_idx;

                new G4PVPlacement( Rot, Crystal_Center,
                        crystal_logic,
                        name.str().c_str(),
                        ecalenvelope,
                        0, 0, overlapcheck);

                j++;
        }

	//Third Quadrant
        j = 0;
        while (j_cry > j) {
                j_idx = Crystals[j][0];
                k_idx = -1 * Crystals[j][1];
                x_cent = Crystals[j][2];
                y_cent = -1.0 * Crystals[j][3];
                z_cent = Crystals[j][4];
                r_theta = Crystals[j][5];
                r_phi = -1.0*Crystals[j][6];

                G4ThreeVector Crystal_Center = G4ThreeVector(x_cent*mm, y_cent*mm, z_cent*mm);

		G4RotationMatrix *Rot = new G4RotationMatrix(); //rotation matrix for the placement of each crystal
                Rot->rotateX(r_phi*rad);
                Rot->rotateY(r_theta*rad);
                Rot->rotateZ(0*rad);

                name.str("");
                name << _crystallogicnameprefix << "_j_" << j_idx << "_k_" << k_idx;

                new G4PVPlacement( Rot, Crystal_Center,
                        crystal_logic,
                        name.str().c_str(),
                        ecalenvelope,
                        0, 0, overlapcheck);

                j++;
        }


	
	cout << endl << "Construction Complete..." << endl;
	datafile.close();
		
	return 0;
}
