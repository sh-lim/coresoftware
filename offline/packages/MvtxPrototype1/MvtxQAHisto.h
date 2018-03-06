#ifndef __MvtxQAHisto_H__
#define __MvtxQAHisto_H__

#include <fun4all/SubsysReco.h>
#include <string>

//Forward declerations
class PHCompositeNode;
class TrkrClusterContainer;
class TFile;
class TH1F;

//Brief: basic ntuple and histogram creation for sim evaluation
class MvtxQAHisto: public SubsysReco
{
 public: 
  //Default constructor
  MvtxQAHisto(const std::string &name="MvtxQAHisto");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_filename(const char* file)
  { if(file) _outfile = file; }

 private:
  //output filename
  std::string _outfile;
   
  //Event counter
  int _event;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //flags
  unsigned int _flags;

	TH1F *h_nclus[4]; //cluster in a chip per event 

	TrkrClusterContainer *cluscon;


};

#endif //* __MvtxQAHisto_H__ *//
