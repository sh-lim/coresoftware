#ifndef __MvtxUnpackPRDFF__
#define __MvtxUnpackPRDFF__

//* Unpacks raw HCAL PRDF files *//
//Abhisek Sen

#include <fun4all/SubsysReco.h>
#include <phool/PHObject.h>

class Event;
class Packet;
class Packet_hbd_fpgashort;

class MvtxUnpackPRDF : public SubsysReco
{
public:
  MvtxUnpackPRDF();

  int
  Init(PHCompositeNode *topNode);

  int
  InitRun(PHCompositeNode *topNode);

  int
  process_event(PHCompositeNode *topNode);

  int
  End(PHCompositeNode *topNode);

  void
  CreateNodeTree(PHCompositeNode *topNode);

private:

  Event* _event;
  Packet_hbd_fpgashort* _packet;
  int _nevents;

};

#endif //**MvtxUnpackPRDFF**//
