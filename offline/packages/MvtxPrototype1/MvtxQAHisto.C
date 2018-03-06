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
		h_nclus[ichip] = new TH1F(Form("h_nclus_chip%d",ichip),"",51,-0.5,50.5);
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
	cluscon = findNode::getClass<TrkrClusterContainer>(topNode, "TrkrClusterContainer");
	if (cluscon==0)
	{
		cout << "MvtxQAHisto::Process_Event - TrkrHitClusterContainer not found" << endl;
		exit(-1);
	}

}



