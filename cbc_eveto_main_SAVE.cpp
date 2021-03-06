#include "cbc_eveto.h"

int eveto::cbc_eveto_main( 
		Long_t gps_start_time, 
		Long_t gps_end_time, 
		TString* detector, 
		TString* safe_channel_file,
		TString* cbc_trigger_database, 
		Float_t cbc_snr_threshold,
		TString* omicron_trigger_path, 
		Double_t omicron_snr_threshold, 
		Double_t omicron_cluster_window,
		TString* output_directory, 
		Float_t sig_threshold,
		Float_t dumb_veto_window,
		Int_t max_rounds,
		bool verbose
		)
{
	int retcode = 0;

	//
	// Read in the veto channel data.
	//
	// For each channel, we get a tree containing the clustered omicron triggers
	// and a tree containing the segments for which vetoes are generated. These
	// are stored in two arrays, each entry in the array is a tree for an input
	// channel.
	//


	// Create a tree containing the list of safe veto channels to be processed
	TTree* safe_channels = new TTree( "safe_channel_list", "List of safe channels to process" );
	Long64_t num_veto_channels = safe_channels->ReadFile( safe_channel_file->Data(), "channel/C" );
	if ( verbose ) {
		std::cout << "Read " << num_veto_channels << " safe channel names" << std::endl;
		safe_channels->Print();
	}

	TTree* clustered_veto_trigger_tree[num_veto_channels];
	TTree* veto_segment_tree[num_veto_channels];

	// Read in the omicron triggers for the interval that we want to process
	retcode = eveto::read_omicron_triggers(
			clustered_veto_trigger_tree, // output
			veto_segment_tree, // output
			safe_channels, // input
			omicron_trigger_path, // input
			omicron_snr_threshold, // input
			omicron_cluster_window, // input
			verbose );

	if ( retcode ) {
		std::cerr << "error reading omicron triggers" << std::endl;
		return 1;
	}

	//
	// Read in cbc triggers from database.
	//

	TTree* cbc_trigger_tree;

	retcode = eveto::read_cbc_triggers(
			&cbc_trigger_tree, // output
			cbc_trigger_database, // input
			gps_start_time, // input
			gps_end_time, // input
			detector, // input
			cbc_snr_threshold, // input
			verbose );

	if ( retcode ) {
		std::cerr << "error reading cbc triggers" << std::endl;
		return 1;
	}

	//
	//Veto Algorithm
	//currently the segments are stored in omicron_triggers.root/segments. For now
	//assume that all segments are science segments. Grab segments from segment tree
	//and perform veto algorithm. 


	int num_safe_channels = safe_channels->GetEntries();
	float max_sig = 0;
	int i; //counts over safe channels
	float sig[num_safe_channels]; //make sure num_safe_channels is defined
	int max_sig_index; //=winning channel!
	int r = 1; //round increment


	//define TTrees
	TTree* cbc_trigs_round[max_rounds + 1];
	TTree* omicron_trigs_round[max_rounds + 1][num_safe_channels];
	//TTree* cbc_segs_round[max_rounds + 1][num_safe_channels]; //doesn't currently exist
	//TTree* omicron_segs_round[max_rounds + 1][num_safe_channels];

 
	//initialize arrays
	cbc_trigs_round[0] = cbc_trigger_tree;
	//cbc_segs_round[0] = cbc_segs_tree;


        std::cerr << "Number of safe channels = " << num_safe_channels << std::endl;
	for (i=0; i<num_safe_channels; ++i) {
                if (verbose) std::cerr << "storing pointers for round 0, channel" << i << std::endl;
		omicron_trigs_round[0][i] = clustered_veto_trigger_tree[i]; //check name
		omicron_trigs_round[0][i]->Print(); //prints out omicron triggers. 
                if (verbose) std::cerr << "stored pointer for round 0" << std::endl;
	}

	 
		//currently seg faults on print() for omicron trigs here. 
		do {
                        if (verbose) std::cerr << "Processing round " << r << " of " << max_rounds << std::endl;

                       
	                if (omicron_trigs_round[r-1][i] != NULL) {

                                if ( verbose ) std::cerr << "calculating dumb significance for tree at " << omicron_trigs_round[r-1][i] << std::endl;
				sig[i] = eveto::calc_dumb_sig(cbc_trigs_round[r-1], omicron_trigs_round[r-1][i],dumb_veto_window,verbose);
                                if ( verbose ) std::cerr << "signifcance was " << sig[i] << std::endl;

				if(sig[i]>max_sig) {
					max_sig = sig[i];
					max_sig_index = i;
				}
			}
#if 0
		cbc_trigs_round[r] = remove_triggers(cbc_trigs_round[r-1], omicron_trigs_round[r-1][max_sig_index],cbc_segs_round[r-1], omicron_segs_round[r-1][max_sig_index]);

			if (i != max_sig_index) {	
				omicron_trigs_round[r][i] = remove_triggers(omicron_trig_round[r-1][i], omicron_trigs_round[r-1][max_sig_index], omicron_segs_round[r-1][i], omicron_segs_round[r-1][max_sig_index]); //should segments have their own remove triggers function?

			}
			else{
				i = NULL;
			}
#endif
			
                        if (verbose) std::cerr << "Finished round " << r << std::endl;
			r += 1;

		}
		while( max_sig > sig_threshold || r <= max_rounds );

                return 0;
}
