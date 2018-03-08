#include "MvtxAlign.h"

#include "mvtx/MvtxDefUtil.h"

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>


#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
#include <g4detectors/PHG4CylinderGeom_MAPS.h>
#include <g4detectors/PHG4CylinderGeom_Siladders.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderCellGeom.h>

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

MvtxAlign::MvtxAlign(const string &name ) :
  SubsysReco(name),
  clusters_(NULL),
  _timer(PHTimeServer::get()->insert_new(name))
{

}

int MvtxAlign::InitRun(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int MvtxAlign::process_event(PHCompositeNode *topNode)
{

  _timer.get()->restart();

  //------
  //--- get cluster container
  //------
  clusters_ = findNode::getClass<TrkrClusterContainer>(topNode, "TrkrClusterContainer");
  if (!clusters_)
  {
    cout << PHWHERE << "ERROR: Can't find node TrkrClusterContainer" << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  //------
  //-- shift clusters
  //------

  TrkrClusterContainer::ConstRange clusrange = clusters_->GetClusters();
  for ( TrkrClusterContainer::ConstIterator iter = clusrange.first;
        iter != clusrange.second;
        ++iter)
  {

    // get the key and check if we need to realign this cluster
    TrkrDefs::cluskey ckey = (iter->second)->GetClusKey();
    TrkrDefs::hitsetkey hkey = ;
    auto aligniter = alignmap_.find(ckey);

    if ( aligniter != alignmap_.end() )
    {
      // get a non-const pointer to the cluster
      TrkrClusterv1* clus = static_cast<TrkrClusterv1*>(clusters_->FindCluster(ckey));

      if ( verbosity > 1 )
      {
        cout << " applying alignment to " << endl;
        clus->identify();
      }

      // apply the alignment
      clus->SetX(clus->GetX() + (aligniter->second).dx);
      clus->SetY(clus->GetY() + (aligniter->second).dy);
      clus->SetZ(clus->GetZ() + (aligniter->second).dz);

      if ( verbosity > 1 )
        clus->identify();
    }

  }


  //------
  //--- done
  //------
  _timer.get()->stop();
  return Fun4AllReturnCodes::EVENT_OK;
}


void MvtxAlign::AddAlignmentPar(TrkrDefs::hitsetkey key, double dx, double dy, double dz)
{

  // check that the misalignment doesn't already exist
  auto iter = alignmap_.find(key);
  if ( iter != alignmap_.end() )
  {
    cout << PHWHERE << " WARNING: overwriting existing misalignment for"
         << " key:0x" << hex << key << dec << endl;

    (iter->second).dx = dx;
    (iter->second).dy = dy;
    (iter->second).dz = dz;
  }
  else
  {
    AlignmentPar mis;
    mis.dx = dx;
    mis.dy = dy;
    mis.dz = dz;

    alignmap_.insert(make_pair(key, mis));

    if ( verbosity > 0 )
    {
      cout << PHWHERE << " Added misalignment:"
           << " key:0x" << hex << key << dec
           << " dx:" << dx
           << " dy:" << dy
           << " dz:" << dz
           << endl;
    }
  }

}



void MvtxAlign::PrintAlignmentPars(std::ostream& os) const
{
  os << "=============================================================" << endl;
  os << "== " << PHWHERE << "==" << endl;
  os << "=============================================================" << endl;

  for ( auto iter = alignmap_.begin();
        iter != alignmap_.end();
        ++iter)
  {
    os << " key:0x" << hex << iter->first << dec
       << " dx:" << (iter->second).dx
       << " dy:" << (iter->second).dy
       << " dz:" << (iter->second).dz
       << endl;
  }

  os << "=============================================================" << endl;
}

