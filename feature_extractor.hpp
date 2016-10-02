#ifndef FEATURE_EXTRACTOR_HPP
#define FEATURE_EXTRACTOR_HPP

#include <vector>

using namespace std;

//the features that Feature_extractor can detect in a signal
enum Feature_type{
	NO_FEATURE = 0,
	BREACHED_HIGH_THRESHOLD,
	BREACHED_LOW_THRESHOLD,
	BREACHED_SLOPE_HIGH_THRESHOLD,
	BREACHED_SLOPE_LOW_THRESHOLD,
	POSITIVE_TROUGH,
	NEGATIVE_TROUGH,
	POSITIVE_TROUGH_MIDDLE,
	NEGATIVE_TROUGH_MIDDLE,
	NEUTRAL
};

//used to store the information pertaining to a feature
struct Feature{
	Feature_type type;
	unsigned int start_time; 
	unsigned int end_time;	

	Feature() : type(NO_FEATURE), start_time(0), end_time(0) {}
};

//encapsulates all the data and functions of Feature_extractor,
//which can take a discrete input signal and detect features within it
class Feature_extractor{
protected:
	//information pertaining to the most recent datapoint
	int latest_data_entry;
	unsigned int latest_data_time;
	int data_diff_from_last_entry;
	unsigned int time_diff_from_last_entry;
	int slope_from_last_entry; 

	//information used to detect BREACHED_HIGH_THRESHOLD and BREACHED_LOW_THRESHOLD features
	int high_threshold;
	int low_threshold;
	unsigned int high_threshold_breach_time;
	unsigned int low_threshold_breach_time;
	unsigned int disable_high_threshold_breach_until;
	unsigned int disable_low_threshold_breach_until;

	//information used to detect BREACHED_SLOPE_HIGH_THRESHOLD and BREACHED_SLOPE_LOW_THRESHOLD features
	int slope_high_threshold;
	int slope_low_threshold;
	unsigned int slope_high_threshold_breach_time;
	unsigned int slope_low_threshold_breach_time;
	unsigned int disable_slope_high_threshold_breach_until;
	unsigned int disable_slope_low_threshold_breach_until;

	//information used to detect POSITIVE_TROUGH, NEGATIVE_TROUGH, POSITIVE_TROUGH_MIDDLE, and NEGATIVE_TROUGH_MIDDLE features
	unsigned int positive_trough_min_length;
	unsigned int negative_trough_min_length;
	int trough_high_threshold;
	int trough_low_threshold;
	unsigned int positive_trough_start_time;
	unsigned int negative_trough_start_time;
	bool positive_trough_active;
	bool negative_trough_active;
	bool positive_trough_middle_detected;
	bool negative_trough_middle_detected;

	//information used to detect the NEUTRAL feature
	unsigned int neutral_min_length;
	int neutral_max_threshold;
	int neutral_min_threshold;
	unsigned int neutral_start_time;
	bool neutral_active;

	//features that are detected are pushed to this container
	vector<Feature *> feature_vector;

public:
	//Default constructor
	Feature_extractor();

	//Explicit constructor used to input specific parameters that determine what waveforms are detected as features
	Feature_extractor(int high_threshold_, int low_threshold_, int slope_high_threshold_, int slope_low_threshold_, 
		int trough_high_threshold_, int trough_low_threshold_, unsigned int positive_trough_min_length_, unsigned int negative_trough_min_length_,
		int neutral_max_threshold_, int neutral_min_threshold_, unsigned int neutral_min_length_);

	//This function is called each time to input a data point
	//It automatically recognizes features and pushes them to the feature container
	bool intake_data(int datapoint, unsigned int time);

	//This function searches the feature container for a specific type of feature within a specific age in ms
	bool feature_in_vector(Feature_type feature, unsigned int max_feature_age);
	
	//dumps the elements of the feature container older than max_age
	void dump_old_features(unsigned int max_age);

	//returns the timesample of the latest inputted data point
	unsigned int get_latest_time() const;

	//prints the features detected so far, starting from most recent
	void print_features(); 
};


#endif