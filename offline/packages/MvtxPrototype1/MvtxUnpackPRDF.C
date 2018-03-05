#include "MvtxUnpackPRDF.h"

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packetConstants.h>
#include <Event/packet.h>
#include <Event/packet_hbd_fpgashort.h>
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

//____________________________________
MvtxUnpackPRDF::MvtxUnpackPRDF() :
    SubsysReco("MvtxUnpackPRDF"),
    /*Event**/_event(NULL),
    /*Packet_hbd_fpgashort**/_packet(NULL),
    /*int*/_nevents(0)
{
}

//____________________________________
int
MvtxUnpackPRDF::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________
int
MvtxUnpackPRDF::InitRun(PHCompositeNode *topNode)
{
  CreateNodeTree(topNode);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________
int
MvtxUnpackPRDF::process_event(PHCompositeNode *topNode)
{
  _nevents++;
  _event = findNode::getClass<Event>(topNode, "PRDF");
  if (_event == 0)
    {
      cout << "MvtxUnpackPRDF::Process_Event - Event not found" << endl;
      return -1;
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//_______________________________________
void
MvtxUnpackPRDF::CreateNodeTree(PHCompositeNode *topNode)
{
}

//___________________________________
int
MvtxUnpackPRDF::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

