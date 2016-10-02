#include "feature_extractor.hpp"

#include <iostream>

using namespace std;

//Default constructor
Feature_extractor::Feature_extractor()
{
	latest_data_entry = 0;
	latest_data_time = 0;
	data_diff_from_last_entry = 0;
	time_diff_from_last_entry = 0;
	slope_from_last_entry = 0;

	high_threshold = 20000;
	low_threshold = -20000;
	high_threshold_breach_time = 0;
	low_threshold_breach_time = 0;
	disable_high_threshold_breach_until = 0;
	disable_low_threshold_breach_until = 0;

	slope_high_threshold = 10000;
	slope_low_threshold = -10000;
	slope_high_threshold_breach_time = 0;
	slope_low_threshold_breach_time = 0;
	disable_slope_high_threshold_breach_until = 0;
	disable_slope_low_threshold_breach_until = 0;

	positive_trough_min_length = 50;
	negative_trough_min_length = 140;
	trough_high_threshold = 5000;
	trough_low_threshold = -2000;
	positive_trough_start_time = 0;
	negative_trough_start_time = 0;
	positive_trough_active = false;
	negative_trough_active = false;
	positive_trough_middle_detected = false;
	negative_trough_middle_detected = false;

	neutral_min_length = 200;
	neutral_max_threshold = 2500;
	neutral_min_threshold = -2500;
	neutral_start_time = 0;
	neutral_active = false;
}

//Explicit constructor
Feature_extractor::Feature_extractor(int high_threshold_, int low_threshold_, int slope_high_threshold_, int slope_low_threshold_, 
	int trough_high_threshold_, int trough_low_threshold_, unsigned int positive_trough_min_length_, unsigned int negative_trough_min_length_,
	int neutral_max_threshold_, int neutral_min_threshold_, unsigned int neutral_min_length_)
{
	latest_data_entry = 0;
	latest_data_time = 0;
	data_diff_from_last_entry = 0;
	time_diff_from_last_entry = 0;
	slope_from_last_entry = 0;

	high_threshold = high_threshold_;
	low_threshold = low_threshold_;
	high_threshold_breach_time = 0;
	low_threshold_breach_time = 0;
	disable_high_threshold_breach_until = 0;
	disable_low_threshold_breach_until = 0;

	slope_high_threshold = slope_high_threshold_;
	slope_low_threshold = slope_low_threshold_;
	slope_high_threshold_breach_time = 0;
	slope_low_threshold_breach_time = 0;
	disable_slope_high_threshold_breach_until = 0;
	disable_slope_low_threshold_breach_until = 0;

	positive_trough_min_length = positive_trough_min_length_;
	negative_trough_min_length = negative_trough_min_length_;
	trough_high_threshold = trough_high_threshold_;
	trough_low_threshold = trough_low_threshold_;
	positive_trough_start_time = 0;
	negative_trough_start_time = 0;
	positive_trough_active = false;
	negative_trough_active = false;

	neutral_min_length = neutral_min_length_;
	neutral_max_threshold = neutral_max_threshold_;
	neutral_min_threshold = neutral_min_threshold_;
	neutral_start_time = 0;
	neutral_active = false;
}

//This function is called each time to input a data point
//It automatically recognizes features and pushes them to the feature container
bool Feature_extractor::intake_data(int datapoint, unsigned int time)
{
	data_diff_from_last_entry = datapoint - latest_data_entry; 	//the change in value of this entry from the last
	time_diff_from_last_entry = time - latest_data_time; 		//the change in time of this entry from the last
	
	//calculate the slope from the last datapoint (but don't if we're dividing by zero)
	if(time_diff_from_last_entry > 0){
		slope_from_last_entry = data_diff_from_last_entry / (int)time_diff_from_last_entry; 
	} 
	
	latest_data_entry = datapoint;
	latest_data_time = time;

	//used to track if any new features are recognized
	bool feature_created = false;
	
	//check if the data has breached the upper threshold 
	if(latest_data_entry > high_threshold && latest_data_time >= disable_high_threshold_breach_until){
		//prevent this from being triggered for 150ms
		disable_high_threshold_breach_until = latest_data_time + 150;

		//Create the feature, set its type and time, and push into the feature container
		Feature *recent_spike = new Feature();
		recent_spike->type = BREACHED_HIGH_THRESHOLD;
		recent_spike->start_time = latest_data_time;
		recent_spike->end_time = latest_data_time;

		feature_vector.push_back(recent_spike);
		feature_created = true;
	} 
	//check if the data has breached the lower threshold 
	else if(latest_data_entry < low_threshold && latest_data_time >= disable_low_threshold_breach_until){
		//prevent this from being triggered for 150ms
		disable_low_threshold_breach_until = latest_data_time + 150;

		//Create the feature, set its type and time, and push into the feature container
		Feature *recent_spike = new Feature();
		recent_spike->type = BREACHED_LOW_THRESHOLD;
		recent_spike->start_time = latest_data_time;
		recent_spike->end_time = latest_data_time;

		feature_vector.push_back(recent_spike);
		feature_created = true;
	} 
	//check if the data has breached the upper threshold for slope
	else if(slope_from_last_entry > slope_high_threshold && latest_data_time >= disable_slope_high_threshold_breach_until){
		//prevent this from being triggered for 150ms
		disable_slope_high_threshold_breach_until = latest_data_time + 150;

		//Create the feature, set its type and time, and push into the feature container
		Feature *recent_spike = new Feature();
		recent_spike->type = BREACHED_SLOPE_HIGH_THRESHOLD;
		recent_spike->start_time = latest_data_time;
		recent_spike->end_time = latest_data_time;

		feature_vector.push_back(recent_spike);
		feature_created = true;
	} 
	//check if the data has breached the upper threshold for slope
	else if(slope_from_last_entry < slope_low_threshold && latest_data_time >= disable_slope_low_threshold_breach_until){
		//prevent this from being triggered for 150ms
		disable_slope_low_threshold_breach_until = latest_data_time + 150;

		//Create the feature, set its type and time, and push into the feature container
		Feature *recent_spike = new Feature();
		recent_spike->type = BREACHED_SLOPE_LOW_THRESHOLD;
		recent_spike->start_time = latest_data_time;
		recent_spike->end_time = latest_data_time;

		feature_vector.push_back(recent_spike);
		feature_created = true;
	}
	
	//if the data rises above the threshold for a positive trough, prepare to detect a positive trough
	if(latest_data_entry > trough_high_threshold && positive_trough_active == false){
		positive_trough_active = true;
		positive_trough_start_time = latest_data_time;
	}
	//if the data falls below the threshold for a negative trough, prepare to detect a negative trough
	else if(latest_data_entry < trough_low_threshold && negative_trough_active == false){
		negative_trough_active = true;
		negative_trough_start_time = latest_data_time;
	}

	//if we've been inside a positive through for the minimum required time, push the "POSITIVE_TROUGH_MIDDLE" feature into the feature container
	if(positive_trough_active && positive_trough_middle_detected == false && latest_data_time >= positive_trough_min_length + positive_trough_start_time){
		positive_trough_middle_detected = true;

		Feature *recent_trough = new Feature();
		recent_trough->type = POSITIVE_TROUGH_MIDDLE;
		recent_trough->start_time = positive_trough_start_time;
		recent_trough->end_time = latest_data_time;

		feature_vector.push_back(recent_trough);
		feature_created = true;
	}
	//if we've been inside a negative through for the minimum required time, push the "NEGATIVE_TROUGH_MIDDLE" feature into the feature container
	else if(negative_trough_active && negative_trough_middle_detected == false && latest_data_time >= negative_trough_min_length + negative_trough_start_time){
		negative_trough_middle_detected = true;

		Feature *recent_trough = new Feature();
		recent_trough->type = NEGATIVE_TROUGH_MIDDLE;
		recent_trough->start_time = negative_trough_start_time;
		recent_trough->end_time = latest_data_time;

		feature_vector.push_back(recent_trough);
		feature_created = true;
	}

	//if we were prepared to detect a positive trough and the signal has fallen back below the threshold
	if(positive_trough_active && latest_data_entry <= trough_high_threshold){
		positive_trough_active = false;

		//if the signal was above the threshold for long enough, push the POSITIVE_TROUGH feature into the feature container
		if(latest_data_time >= positive_trough_min_length + positive_trough_start_time){
			Feature *recent_trough = new Feature();
			recent_trough->type = POSITIVE_TROUGH;
			recent_trough->start_time = positive_trough_start_time;
			recent_trough->end_time = latest_data_time;

			feature_vector.push_back(recent_trough);
			feature_created = true;
		} 
		//otherwise, discard the current potential trough and start looking for the beginning of the next one
		positive_trough_middle_detected = false;
	} 
	//if we were prepared to detect a negative trough and the signal has risen back above the threshold
	if(negative_trough_active && latest_data_entry >= trough_low_threshold){// && positive_trough_active == false){
		negative_trough_active = false;

		//if the signal was below the threshold for long enough, push the NEGATIVE_TROUGH feature into the feature container
		if(latest_data_time >= negative_trough_min_length + negative_trough_start_time){
			Feature *recent_trough = new Feature();
			recent_trough->type = NEGATIVE_TROUGH;
			recent_trough->start_time = negative_trough_start_time;
			recent_trough->end_time = latest_data_time;

			feature_vector.push_back(recent_trough);
			feature_created = true;
		}

		//otherwise, discard the current potential trough and start looking for the beginning of the next one
		negative_trough_middle_detected = false;
	}

	//if the signal is between the neutral thresholds, prepare to push the NEUTRAL feature into the feature container
	if(latest_data_entry < neutral_max_threshold && latest_data_entry > neutral_min_threshold && neutral_active == false){
		neutral_active = true;
		neutral_start_time = latest_data_time;
	}

	//if we've prepared to detect the NEUTRAL feature and the signal exits the neutral range or the feature time is up
	if(neutral_active && (latest_data_time >= neutral_start_time + neutral_min_length || latest_data_entry >= neutral_max_threshold || latest_data_entry <= neutral_min_threshold)){
		//if the feature time is up, push the NEUTRAL feature to the feature container
		if(latest_data_time >= neutral_start_time + neutral_min_length){
			Feature *recent_neutral_centre = new Feature();
			
			recent_neutral_centre->type = NEUTRAL;
			recent_neutral_centre->start_time = neutral_start_time;
			recent_neutral_centre->end_time = latest_data_time;

			feature_vector.push_back(recent_neutral_centre);
			feature_created = true;
		} 

		//otherwise, wait for the next neutral feature to start
		neutral_active = false;
	}

	//if any features were pushed to the feature container, return true
	if(feature_created){
		return true;
	} else {
		//therwise, return false
		return false;
	}
}

//This function searches the feature container for a specific type of feature within a specific age in ms
bool Feature_extractor::feature_in_vector(Feature_type feature, unsigned int max_feature_age)
{
	if(feature_vector.empty()){
		return false;
	}

	vector<Feature *>::iterator iter = feature_vector.end() - 1;

	while(iter != (feature_vector.begin() - 1) && latest_data_time - (*iter)->end_time < max_feature_age){
		if((*iter)->type == feature){
			return true;
		}
		iter--;
	}
	return false;
}

//dumps the elements of the feature container older than max_age
void Feature_extractor::dump_old_features(unsigned int max_age)
{
	vector<Feature *>::iterator iter = feature_vector.end() - 1;

	uint8_t count = 0;

	while(iter != feature_vector.begin() - 1 && (*iter)->end_time + max_age > latest_data_time){
		iter--;
		count++;
	}

	if(count == 0){
		return;
	}

	vector<Feature *> trimmed_vector(feature_vector.end() - 1 - count, feature_vector.end() - 1);

	feature_vector = trimmed_vector;
}

//returns the timesample of the latest inputted data point
unsigned int Feature_extractor::get_latest_time() const
{
	return latest_data_time;
}

//prints the features in the feature container, starting from most recent
void Feature_extractor::print_features()
{
	vector<Feature *>::iterator iter = feature_vector.end() - 1;

	while(iter != feature_vector.begin() - 1){
		cout << "Feature: " << (*iter)->type << " time:" << (*iter)->end_time << endl;
		iter--;
	}
}