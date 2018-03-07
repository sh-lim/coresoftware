#include "MvtxApplyHotDead.h"

#include "mvtx/MvtxDefUtil.h"
#include "mvtx/MvtxHitSetv1.h"

#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetv1.h>
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

MvtxApplyHotDead::MvtxApplyHotDead(const string &name, const std::string file ) :
  SubsysReco(name),
  hits_(NULL),
  hdfile_(file),
  _timer(PHTimeServer::get()->insert_new(name))
{
  ReadHotDeadFile();
}

int MvtxApplyHotDead::InitRun(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int MvtxApplyHotDead::process_event(PHCompositeNode *topNode)
{

  _timer.get()->restart();

  //------
  //--- get node containing the digitized hits
  //------
  hits_ = findNode::getClass<TrkrHitSetContainer>(topNode, "TrkrHitSetContainer");
  if (!hits_)
  {
    cout << PHWHERE << "ERROR: Can't find node TrkrHitSetContainer" << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  //------
  //-- mask pixels
  //------



  //------
  //--- done
  //------
  _timer.get()->stop();
  return Fun4AllReturnCodes::EVENT_OK;
}

void MvtxApplyHotDead::ReadHotDeadFile()
{
  if (verbosity > 0)
    cout << "Entering MvtxApplyHotDead::ReadHotDeadFile " << endl;

  //----------
  //--- Open file
  //----------

  // check for reasonable file name
  if ( hdfile_.length() < 2 )
    return;

  ifstream fin;
  fin.open(hdfile_.c_str());
  if ( fin.is_open() )
  {
    string line;

    MvtxDefUtil util;

    while ( getline(fin, line) )
    {
      int lyr, stave, chip, col, row;
      sscanf(line.c_str(), "%i %i %i %i %i", &lyr, &stave, &chip, &col, &row);

      // check validity
      if ( lyr < 0 || stave < 0 || chip < 0 || col < 0 || row < 0 )
      {
        cout << PHWHERE << "WARNING: skipping invalid entry: "
             << " lyr:" << lyr
             << " stave:" << stave
             << " chip:" << chip
             << " col:" << col
             << " row:" << row
             << endl;
      }

      // generate the hitsetkey
      TrkrDefs::hitsetkey key = util.GenHitSetKey(lyr, stave, chip);

      // add to the map
      hdmap_.insert(make_pair(key, make_pair(col, row)));
    }
    fin.close();
  }
  else
  {
    cout << PHWHERE << "ERROR: Can't open " << hdfile_ << ", will not mask any pixels." << endl;
  }

  return;
}

void MvtxApplyHotDead::PrintHotDeadMap(std::ostream& os) const
{
  os << "=============================================================" << endl;
  os << "== " << PHWHERE << "==" << endl;
  os << "=============================================================" << endl;

  MvtxDefUtil util;
  for ( ConstIterator iter = hdmap_.begin();
        iter != hdmap_.end();
        ++iter)
  {
    os << " "
       << util.GetLayer(iter->first) << " "
       << util.GetStaveId(iter->first) << " "
       << util.GetChipId(iter->first) << " "
       << (iter->second).first << " "
       << (iter->second).second
       << endl;
  }

  os << "=============================================================" << endl;
}

