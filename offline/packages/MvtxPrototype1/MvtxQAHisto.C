////////////////////////////////////////////////////////////////////////////////
//
// This module is desgined to grab svtx tracks and put truth and cluster
// information into a TTree for GenFit testing
//
////////////////////////////////////////////////////////////////////////////////
//
// Darren McGlinchey
// 1 Apr 2016
//
////////////////////////////////////////////////////////////////////////////////


#include "MvtxQAHisto.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <trackbase/TrkrDefUtil.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv1.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetv1.h>
#include <mvtx/MvtxDefUtil.h>
#include <mvtx/MvtxHitSetv1.h>

#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>

#include <iostream>

using namespace std;

//----------------------------------------------------------------------------//
//-- Constructor:
//--  simple initialization
//----------------------------------------------------------------------------//
MvtxQAHisto::MvtxQAHisto(const string &name):
  SubsysReco( name )
{
  //initialize
  _outfile = "MvtxQAHisto.root";

	beam_x[0] = 826.693;
	beam_x[1] = 822.267;
	beam_x[2] = 818.413;
	beam_x[3] = 830.190;

	beam_y[0] = 158.773;
	beam_y[1] = 167.537;
	beam_y[2] = 181.318;
	beam_y[3] = 190.988;

}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int MvtxQAHisto::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Openning file " << _outfile << endl;
  PHTFileServer::get().open( _outfile, "RECREATE");

	for (int ichip=0; ichip<4; ichip++){
		//h_nclus[ichip] = new TH1F(Form("h_nclus_chip%d",ichip),"",51,-0.5,50.5);
		h2d_hit[ichip] = new TH2F(Form("h2d_hit_chip%d",ichip),"",1024,-0.5,1023.5,512,-0.5,511.5);
		h1d_hit_per_evt[ichip] = new TH1F(Form("h1d_hit_per_evt_chip%d",ichip),"",101,-0.5,100);

		h2d_hit_beam[ichip] = new TH2F(Form("h2d_hit_beam_chip%d",ichip),"",1025,-512.5,512.5,513,-256.5,256.5);
	}

  return 0;
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//
int MvtxQAHisto::process_event(PHCompositeNode *topNode)
{
  _event++;
  //if (1)
  if (_event % 1000 == 0)
    cout << PHWHERE << "Events processed: " << _event << endl;

  GetNodes(topNode);

	TrkrDefUtil trkrdefutil;
	MvtxDefUtil mvtxdefutil;

	int nhit_per_chip[4] = {0};

	double avg_x = 0.0;
	double avg_y = 0.0;
	double norm = 0.0;

	TrkrHitSetContainer::ConstRange iter_range = hitsetcon->GetHitSets();
	for ( TrkrHitSetContainer::ConstIterator iter = iter_range.first; iter!=iter_range.second; ++iter){

		int ichip = int(mvtxdefutil.GetChipId(iter->first));

		MvtxHitSetv1 *hitset = static_cast<MvtxHitSetv1 *>(iter->second);

		MvtxHitSetv1::ConstRange hit_iter_range = hitset->GetHits();
		for ( MvtxHitSetv1::ConstIterator hit_iter = hit_iter_range.first; hit_iter!=hit_iter_range.second; ++hit_iter){

			int icol = int(hit_iter->first);
			int irow = int(hit_iter->second);

			h2d_hit[ichip]->Fill(icol,irow);

			nhit_per_chip[ichip]++;

		}//hit_iter
	}//iter

	for (int ichip=0; ichip<4; ichip++){
		h1d_hit_per_evt[ichip]->Fill(nhit_per_chip[ichip]);
	}

	/*
	int nclus[4] = {0};

	TrkrClusterContainer::ConstRange iter_range = cluscon->GetClusters();
	for (TrkrClusterContainer::ConstIterator iter = iter_range.first; iter != iter_range.second; ++iter){

		int ichip  = trkrdefutil.GetLayer(iter->first);

		//cout << "CHIP: " << ichip << endl;
		nclus[ichip]++;

	}//


	for (int ichip=0; ichip<4; ichip++){
		h_nclus[ichip]->Fill(nclus[ichip]);
	}
	*/


  return 0;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//
int MvtxQAHisto::End(PHCompositeNode *topNode)
{

	cout << "-----MvtxQAHisto::End------" << endl;

  PHTFileServer::get().cd( _outfile );
	PHTFileServer::get().write( _outfile );
  //PHTFileServer::get().close();

  return 0;
}


//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
void MvtxQAHisto::GetNodes(PHCompositeNode * topNode)
{
  //DST objects
	//
	//
	/*
	cluscon = findNode::getClass<TrkrClusterContainer>(topNode, "TrkrClusterContainer");
	if (cluscon==0)
	{
		cout << "MvtxQAHisto::Process_Event - TrkrHitClusterContainer not found" << endl;
		exit(-1);
	}
	*/

	hitsetcon = findNode::getClass<TrkrHitSetContainer>(topNode, "TrkrHitSetContainer");
	if (hitsetcon==0)
	{
		cout << "MvtxQAHisto::Process_Event - TrkrHitSetContainer not found" << endl;
		exit(-1);
	}

}



