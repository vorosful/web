// $Id: pisaToDST.C,v 1.52 2007/11/28 14:04:09 hpereira Exp $
/*!
   \file pisaToDST.C
   \brief simDST reconstruction chain to produce working group nanoDST
   \author <a href="mailto:pereira@hep.saclay.cea.fr">Hugo Pereira</a>
   \version $Revision: 1.52 $
   \date $Date: 2007/11/28 14:04:09 $
*/

void Fun4All_RecoDST_sim(

  Int_t nEvents = 100,
  char *filein="simDST.root",
  char *cntout = "simCNT.root",
  char *cwgout = "simCWG.root",
  char *ewgout = "simEWG.root",
  char *hwgout = "simHWG.root",
  char *mwgout = "simMWG.root"
 )
{
   // print output
  cout << "Fun4All_recoDST_sim - nEvents: " << nEvents << endl;
  cout << "Fun4All_recoDST_sim - filein: " << filein << endl;

  if( cntout ) cout << "Fun4All_recoDST_sim - cntout: " << cntout << endl;
  if( cwgout ) cout << "Fun4All_recoDST_sim - cwgout: " << cwgout << endl;
  if( ewgout ) cout << "Fun4All_recoDST_sim - ewgout: " << ewgout << endl;
  if( hwgout ) cout << "Fun4All_recoDST_sim - hwgout: " << hwgout << endl;
  if( mwgout ) cout << "Fun4All_recoDST_sim - mwgout: " << mwgout << endl;
  cout << endl;

  // load libraries
  gSystem->Load("libfun4all.so");
  gSystem->Load("libmutoo_subsysreco.so");
  gSystem->Load("libfun4allfuncs.so");
  gSystem->Load("libsimreco.so");

  gROOT->ProcessLine(".L pisaToDST_IOManager.C");

  ///////////////////////////////////////////
  bool write_cnt = true;
  bool write_cwg = true;
  bool write_ewg = true;
  bool write_hwg = true;
  bool write_mwg = true;

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();

  // there is no subsysreco needed for the CNT
  // because all nodes are already included in
  // the simDST generated by the pisaToDST macro
  // what is done here is just a copy of these nodes
  // to the dedicated file
  if( write_cnt && cntout )
  { CNT_IOManager(cntout, se); }

  // cwg output
  if( write_cwg && cwgout )
  {
    //FilterReco constructor FilterReco(cglflag, photflag, "reconame", "Hard");
    // flags: 0 no node, 1 tr node, 2 ev node, 3 both, 4 both ev/tr cuts but combined in 1 node
    gSystem->Load("libHardpDST.so");
    gSystem->Load("libFilterReco.so");
    FilterReco *hpdst   = new FilterReco(1,4,"hardpdstreco", "");
    hpdst->set_trkdchqualcut(7);
    hpdst->set_trktrackptlowcut(0.2);
    hpdst->set_trkclustptlowcut(0.5);
    hpdst->set_evtlowclustercut(0.3);
    hpdst->set_photontriggercut(2.0);
    hpdst->set_evtnphotoncut(1);
    se->registerSubsystem( hpdst );

    FilterReco *hpdst3   = new FilterReco(4,4,"hardtinypdstreco", "Hard");
    //qual 31 || 63
    hpdst3->set_trkdchqualcut(12345);
    hpdst3->set_trktrackptlowcut(1.5);
    hpdst3->set_trkclustptlowcut(1.8);
    hpdst3->set_evtlowclustercut(0.45);
    hpdst3->set_photontriggercut(3.5);
    hpdst3->set_evtnphotoncut(1);
    hpdst3->set_evttrackptlowcut(1.5);
    hpdst3->set_evtnpi0cut(1);
    hpdst3->set_evttrigpi0minmass(0.110);
    hpdst3->set_evttrigpi0maxmass(0.170);
    hpdst3->set_evttrigpi0minpt(3.5);
    hpdst3->set_evttrigpi0maxasym(0.75);
    hpdst3->set_evtminbiastrig(20);
    se->registerSubsystem( hpdst3 );
    CWG_IOManager(cwgout, se);
  }

  // ewg (electron working group) output
  if( write_ewg && ewgout )
  {
    gSystem->Load("libEWG.so");
    EWGInclusiveNanoCutsv2 *EWGcuts = new EWGInclusiveNanoCutsv2();
    EWGcuts->set_vertexcut(50.);
    EWGcuts->set_eoverpmin(-9999.);
    EWGcuts->set_eoverpmax(9999.);
    EWGcuts->set_emcPhicut(9999.);
    EWGcuts->set_emcZcut(9999.);
    se->registerSubsystem( new WGReco(EWGcuts,"EWGCentralTrack") );
    EWG_IOManager(ewgout, se);
  }

  // hwg (hadron working group) output
  if( write_hwg && hwgout )
  {
    gSystem->Load("libHWG.so");
    PHInclusiveNanoCuts *HWGcuts = new HWGInclusiveNanoCutsv2();
    SubsysReco *HWG = new WGReco(HWGcuts,"HWGCentralTrack");
    se->registerSubsystem(HWG);
    HWG_IOManager(hwgout, se);
  }

  // mwg (muon working group) output
  if( write_mwg && mwgout )
  {
    gSystem->Load("libMWGOO.so");

    // event counter
    se->registerSubsystem( new MuonCounter() );

    // unfortunately the muon arm need the sign of the magnetic field
    // and does not have yet the logic to retrieve it from the pisa event header
    // it is hard-coded here
    mMfmMT::setMapFileScale(1.0);
    MuonUtil::set_check_mapfile_scale( false );

    TMutExtVtx::get().set_verbosity( MUTOO::SOME );

    // muon reconstruction
    MuonUnpackSim* muon_unpack = new MuonUnpackSim();
    muon_unpack->SetMode(  MuonUnpackSim::MC_SIGNAL_NO_BG );
    se->registerSubsystem( muon_unpack );
    se->registerSubsystem( new MuiooReco() );
    se->registerSubsystem( new MuonDev() );

    //  This is the class which makes the GlobalEvent data on the nanoDST output
    /*
    Global reco needs to be re-run in this macro even though it was already run
    in the pisaToDST macro to fill the muon information that is available only after
    the reconstruction is performed
    */
    se->registerSubsystem(new GlobalReco());

    // MWG reconstruction
    PHInclusiveNanoCuts *MWGcuts = new MWGInclusiveNanoCutsv2();
    se->registerSubsystem(new MWGOOReco(MWGcuts));

    // output manager
    MWG_IOManager(mwgout, se);

  }

  ///////////////////////////////////////////
  // InputManager
  ///////////////////////////////////////////
  Fun4AllInputManager *input_manager = new Fun4AllNoSyncDstInputManager("DST_IM","DST");
  se->registerInputManager(input_manager);
  se->fileopen(input_manager->Name(),filein);

  // when processing Muon simulations, the input file
  // must also be opened under a "SIGNAL" node, because of the way
  // embedding is implemented in the muon world
  if( write_mwg && mwgout )
  {
    recoConsts *rc = recoConsts::instance();
    rc->set_CharFlag("EMBED_MC_TOPNODE","SIGNAL");
    Fun4AllInputManager *signal_input_manager = new Fun4AllNoSyncDstInputManager("SIGNAL_IM", "DST", "SIGNAL");
    se->registerInputManager(signal_input_manager);
    se->fileopen(signal_input_manager->Name(),filein);
  }

  // process input events
  gBenchmark->Start("eventLoop");
  se->run(nEvents);
  se->End();
  gBenchmark->Show("eventLoop");

  // If you do not see this message, the job failed
  cout << "Completed reconstruction." << endl;

}
