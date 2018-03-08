#ifndef __PHG4SVTXCLUSTERIZER_H__
#define __PHG4SVTXCLUSTERIZER_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHTimeServer.h>
#include <map>
#include <limits.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefUtil.h>

class TrkrHitSetContainer;

class MvtxAlign : public SubsysReco {

public:

  struct AlignmentPar {
    double dx;
    double dy;
    double dz;
  };


  MvtxAlign(const std::string &name = "MvtxAlign");
  virtual ~MvtxAlign() {}

  //! module initialization
  int Init(PHCompositeNode *topNode) {return 0;}

  //! run initialization
  int InitRun(PHCompositeNode *topNode);

  //! event processing
  int process_event(PHCompositeNode *topNode);

  //! end of process
  int End(PHCompositeNode *topNode) {return 0;}

  //! put in misalignment by hand
  void AddAlignmentPar(TrkrDefs::hitsetkey key, double dx, double dy, double dz);

  //! print stored misalignments
  void PrintAlignmentPars(std::ostream &os = std::cout) const;

private:


  // node tree storage pointers
  TrkrClusterContainer* clusters_;

  // storage object for misalignments
  std::map<TrkrDefs::hitsetkey, AlignmentPar> alignmap_;

  PHTimeServer::timer _timer;
};

#endif
