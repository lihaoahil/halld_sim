#include "DIRCSmearer.h"

//-----------
// dirc_config_t  (constructor)
//-----------
dirc_config_t::dirc_config_t(JEventLoop *loop)
{
        // default values
        DIRC_TSIGMA           = 0.5; // 0.5 ns 
	DIRC_MAX_CHANNELS     = 108*64; 

#if 0
	// Get values from CCDB
	cout<<"get DIRC/mc_timing_smear parameters from calibDB"<<endl;
	map<string, double> dircmctimingsmear;
	if(loop->GetCalib("DIRC/mc_timing_smear", dircmctimingsmear)) {
		jerr << "Problem loading DIRC/mc_timing_smear from CCDB!" << endl;
	} else {
		DIRC_TSIGMA = dircmctimingsmear["DIRC_TSIGMA"];
	}
#endif

	// get DIRC channel status from DB
	vector<int> new_status(DIRC_MAX_CHANNELS);
	dChannelStatus.push_back(new_status); 
	dChannelStatus.push_back(new_status);
	if (loop->GetCalib("/DIRC/North/channel_status", dChannelStatus[0]))
		jout << "Error loading /DIRC/North/channel_status !" << endl;
	if (loop->GetCalib("/DIRC/South/channel_status", dChannelStatus[1]))
		jout << "Error loading /DIRC/South/channel_status !" << endl;
	
	// get per-pixel efficiencies from CCDB
}

//-----------
// SmearEvent
//-----------
void DIRCSmearer::SmearEvent(hddm_s::HDDM *record)
{
#ifdef SMEARDIRC
	hddm_s::DircTruthPmtHitList truthPmtHits = record->getDircTruthPmtHits();
	hddm_s::DircTruthPmtHitList::iterator iter;
	for (iter = truthPmtHits.begin(); iter != truthPmtHits.end(); ++iter) {
		iter->deleteDircPmtHits();
		
		// add per-pixel efficiencies from MAPMT test data
		//if (config->APPLY_EFFICIENCY_CORRECTIONS && !gDRandom.DecideToAcceptHit(dirc_config->GetEfficiencyCorrectionFactor(iter->getCh())) ) {
		//	continue;
		//}

		double t = iter->getT();
		double t_fixed = iter->getT_fixed();
		int ch = iter->getCh();
		
		if(config->SMEAR_HITS) {
			// Smear the timing of the hit
                        t += gDRandom.SampleGaussian(dirc_config->DIRC_TSIGMA);
			
			// Add cross talk here?

			// Remove pixels with bad status
			int box = (iter->getCh() < dirc_config->DIRC_MAX_CHANNELS) ? 1 : 0;
			int channel = iter->getCh() % dirc_config->DIRC_MAX_CHANNELS;
			dirc_status_state status = static_cast<dirc_status_state>(dirc_config->dChannelStatus[box][channel]);
			if ( (status==BAD) || (status==NOISY) ) {
				continue;
			}
		}
		
		hddm_s::DircPmtHitList hits = iter->addDircPmtHits();
		hits().setT(t);
		hits().setCh(ch);
	}
#endif
}
