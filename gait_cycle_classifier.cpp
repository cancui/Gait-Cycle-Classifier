#include "gait_cycle_classifier.hpp"

using namespace std;

Gait_cycle_classifier::Gait_cycle_classifier()
{
	//Initialize the state machine
	initialize_FSM(gait_cycle_states); 
	current_state = gait_cycle_states[0];

	//initialize the feature extractors for accelerometer-y and gyroscope-y with their parameters
	accel_y = new Feature_extractor(11000, -7000, 1000, -2000, 99999, -99999, 99999, 99999, 3500, -2500, 150);
	gyro_y = new Feature_extractor(4000, -1000, 99999, -99999, 5500, -1500, 25, 150, -99999, 99999, 99999);
}

Gait_cycle_classifier::~Gait_cycle_classifier()
{
	delete accel_y;
	accel_y = NULL;
	delete gyro_y;
	gyro_y = NULL;
}

//This function is used to input data points into the classifier.
//When a state is recognized, the members of state_info is updated and the function returns true.
//This function is powered by the state machine
bool Gait_cycle_classifier::intake_data(int ay_value, int gy_value, unsigned int time_value, State_recognized_info & state_info)
{
	bool feature_generated = false;
	feature_generated = feature_generated || accel_y->intake_data(ay_value, time_value);
	feature_generated = feature_generated || gyro_y->intake_data(gy_value, time_value);

	//if either intake_data() function from accel_y or gyro_y returns true after being called,
	//then check for the features required to change to the next gait cycle state in each (accel_y, gyro_y)
	if(feature_generated){
		bool all_features_match = true;

		//for each criterion feature required from accel_y
		for(uint8_t i = 0; i < current_state.ay_num_criteria; i++){
			State_criterion current_criterion = current_state.ay_criteria_for_state[i];
			//if we don't need to look for any features from accel_y
			if(current_criterion.criterion_feature == NO_FEATURE){
				break;
			}

			//check if the criterion feature is in the feature container (if it is not, then we don't have the matches necessary to change state)
			if(!accel_y->feature_in_vector(current_criterion.criterion_feature, current_criterion.criterion_max_age)){
				all_features_match = false;
			} 
		}

		//for each criterion feature required from gyro_y
		for(uint8_t i = 0; i < current_state.gy_num_criteria; i++){
			State_criterion current_criterion = current_state.gy_criteria_for_state[i];
			//if we don't need to look for any features from gyro_y
			if(current_criterion.criterion_feature == NO_FEATURE){
				break;
			}

			//check if the criterion feature is in the feature container (if it is not, then we don't have the matches necessary to change state)
			if(!gyro_y->feature_in_vector(current_criterion.criterion_feature, current_criterion.criterion_max_age)){
				all_features_match = false;
			} 
		}

		//if all features we were looking for were found, then we can change to the next state
		if(all_features_match){

			//set the state_info struct to contain information about the detected state, so main() can use this information
			state_info.recognized_state = current_state.current_state;
			state_info.time_recognized = accel_y->get_latest_time();

			//change to the next state
			current_state = gait_cycle_states[current_state.next_state];

			//At the start of each gait cycle, dump all features that are older than 1000ms
			if(current_state.current_state == HEEL_STRIKE){
				accel_y->dump_old_features(1000);
				gyro_y->dump_old_features(1000);
			}

			//return true to indicate the state was updated
			return true;
		} else {
			//if the state was not updated, return false
			return false;
		}
	}
	return false;
}

//A function for converting a state into human-readable text, for use during demonstration
string get_state_string(Gait_cycle_states state){
	if(state == HEEL_STRIKE){
		return "Heel strike";
	} else if(state == FLAT_FOOT){
		return "Flat foot";
	} else if(state == MID_STANCE){
		return "Mid-stance";
	} else if(state == HEEL_OFF){
		return "Heel-off";
	} else if(state == TOE_OFF){
		return "Toe-off";
	} else if(state == MID_SWING){
		return "Mid-swing";
	} else {
		return "Invalid state!";
	}
}

//This function sets each state in the state machine,
//including the state's name, the criteria to change to the next state,
//and the index of the next state
void initialize_FSM(State states_array[])
{
	//Heel strike----------------------------------------------------------
	State heel_strike = {
		.current_state = HEEL_STRIKE, 
		.next_state = 1, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 2, 
		.gy_criteria_for_state = new State_criterion[2]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	heel_strike.ay_criteria_for_state[0] = {NO_FEATURE, 150};
	heel_strike.gy_criteria_for_state[0] = {NEGATIVE_TROUGH, 150};
	heel_strike.gy_criteria_for_state[1] = {BREACHED_HIGH_THRESHOLD, 30};

	//putting the state we just configured into the state array
	states_array[0] = heel_strike;

	//Flat foot----------------------------------------------------------
	State flat_foot ={
		.current_state = FLAT_FOOT, 
		.next_state = 2, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 2, 
		.gy_criteria_for_state = new State_criterion[2]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	flat_foot.ay_criteria_for_state[0] = {NO_FEATURE, 150};
	flat_foot.gy_criteria_for_state[0] = {BREACHED_HIGH_THRESHOLD, 150};
	flat_foot.gy_criteria_for_state[1] = {POSITIVE_TROUGH, 30};

	//putting the state we just configured into the state array
	states_array[1] = flat_foot;	

	//Mid stance----------------------------------------------------------
	State mid_stance ={
		.current_state = MID_STANCE, 
		.next_state = 3, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 1, 
		.gy_criteria_for_state = new State_criterion[1]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	mid_stance.ay_criteria_for_state[0] = {NEUTRAL, 30};
	mid_stance.gy_criteria_for_state[0] = {POSITIVE_TROUGH, 250};	

	//putting the state we just configured into the state array
	states_array[2] = mid_stance;	

	//Heel off----------------------------------------------------------
	State heel_off ={
		.current_state = HEEL_OFF, 
		.next_state = 4, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 1, 
		.gy_criteria_for_state = new State_criterion[1]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	heel_off.ay_criteria_for_state[0] = {NEUTRAL, 150};
	heel_off.gy_criteria_for_state[0] = {BREACHED_HIGH_THRESHOLD, 30};	

	//putting the state we just configured into the state array
	states_array[3] = heel_off;	

	//Toe off----------------------------------------------------------
	State toe_off = {
		.current_state = TOE_OFF, 
		.next_state = 5, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 2, 
		.gy_criteria_for_state = new State_criterion[2]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	toe_off.ay_criteria_for_state[0] = {NO_FEATURE, 150};
	toe_off.gy_criteria_for_state[0] = {BREACHED_HIGH_THRESHOLD, 200};	
	toe_off.gy_criteria_for_state[1] = {POSITIVE_TROUGH, 30};	

	//putting the state we just configured into the state array
	states_array[4] = toe_off;	

	//Mid swing----------------------------------------------------------
	State mid_swing = {
		.current_state = MID_SWING, 
		.next_state = 0, //the index of the next state

		.ay_num_criteria = 1, 
		.ay_criteria_for_state = new State_criterion[1],

		.gy_num_criteria = 2, 
		.gy_criteria_for_state = new State_criterion[2]
	};

	//The features to look for in accel_y and gyro_y, and how old they can be to count (in ms)
	mid_swing.ay_criteria_for_state[0] = {NO_FEATURE, 150};
	mid_swing.gy_criteria_for_state[0] = {BREACHED_LOW_THRESHOLD, 250};	
	mid_swing.gy_criteria_for_state[1] = {NEGATIVE_TROUGH_MIDDLE, 30};	

	//putting the state we just configured into the state array
	states_array[5] = mid_swing;	
}