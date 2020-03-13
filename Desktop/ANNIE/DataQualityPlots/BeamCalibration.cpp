#include "BeamCalibration.h"

BeamCalibration::BeamCalibration():Tool(){}


bool BeamCalibration::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////
  int annieeventexists = m_data->Stores.count("ANNIEEvent");
  m_variables.Get("verbosity",verbosity);
  m_variables.Get("OutputFile",outputfile);
 /* m_variables.Get("SaveWaveforms",SaveWaves);
  m_variables.Get("PulseThreshold", pulse_threshold); 
  m_variables.Get("UseLEDWaveforms", use_led_waveforms); 
  m_variables.Get("WavesWithPulsesOnly",PulsesOnly);
  m_variables.Get("MaxWaveforms",MaxWaveforms);
  m_variables.Get("LEDsUsed",LEDsUsed);
  m_variables.Get("LEDSetpoints",LEDSetpoints);*/

if(annieeventexists==0) {
    std::cout << "PrintADCData: No ANNIE Event in store to print!" << std::endl;
    return false;
  }
   PEs_per_beam_event = new TH1D("PEs_per_beam_event","Number of PEs per Beam Event",100,0,100);
   PMTs_multiple_PEs = new TH1D("PMTs_multiple_PEs","Number of PMTs with multiple PE per Beam Event",100,0,100);
   Percentage_5_PMTs_multiple_PEs = new TH1D("Percentage_5_PMTs_multiple_PEs","Percentage of events with more than 5 PMTs with multiple PEs",100,0,100);
   auto get_geometry= m_data->Stores.at("ANNIEEvent")->Header->Get("AnnieGeometry",geom);
  if(!get_geometry){
  	Log("DigitBuilder Tool: Error retrieving Geometry from ANNIEEvent!",v_error,verbosity); 
  	return false; 
  	n_tank_pmts = geom->GetNumDetectorsInSet("Tank");
  	Position detector_center=geom->GetTankCentre();
  	tank_center_x = detector_center.X();
  	tank_center_y = detector_center.Y();
  	tank_center_z = detector_center.Z();


  	WaveformNum = 0;



  return true;
}


bool BeamCalibration::Execute(){
	int PMT_Gain = .11;
  if(verbosity>3) std::cout << "PrintADCData: getting total entries from header" << std::endl;
  m_data->Stores["ANNIEEvent"]->Header->Get("TotalEntries",totalentries);
  if(verbosity>3) std::cout << "PrintADCData: Number of ANNIEEvent entries: " << totalentries << std::endl;
  if(verbosity>3) std::cout << "PrintADCData: looping through entries" << std::endl;
	m_data->Stores["ANNIEEvent"]->Get("RawADCData",RawADCData);
  m_data->Stores["ANNIEEvent"]->Get("RawADCAuxData",RawADCAuxData);
  m_data->Stores["ANNIEEvent"]->Get("RunNumber",RunNum);
  m_data->Stores["ANNIEEvent"]->Get("SubrunNumber",SubrunNum);
   auto get_pulses = m_data->Stores["ANNIEEvent"]->Get("RecoADCHits",RecoADCHits);
   for (int i = 0; i < RecoADCHits.size(); i++){
   	int channel_key = i;
  	std::vector<std::vector<ADCPulse>> ADChits = RecoADCHits.at(channel_key);
  		for(int j = 0; j < ADChits.size();j++){
  			std::vector<ADCPulse>> hit = ADChits.at(pulse);
  			for(int k = 0; k < hit.size();k++){
  				ADCPulse apulse = hit.at(k);
  				std::vector<double> PMT_PEs = nullptr;
  				PMT_PEs.insert(k,apulse.charge()/PMT_Gain);

  			}
  		}

	}	


  return true;
}


bool BeamCalibration::Finalise(){
	TCanvas *c1 = new TCanvas("c1","c1",800,1000);
	TCanvas *c2 = new TCanvas("c2","c2",800,1000);
	TCanvas *c3 = new TCanvas("c3","c3",800,1000);
	PEs_per_beam_event->Fill(PMT_PEs);
	PMTs_multiple_PEs->Fill();
	Percentage_5_PMTs_multiple_PEs->Fill();
	c1->cd();
	PEs_per_beam_event->Draw();
	c2->cd()
	PMTs_multiple_PEs->Draw();
	c3->cd()
	Percentage_5_PMTs_multiple_PEs->Draw();

	

  return true;
}
