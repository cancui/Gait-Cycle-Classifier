#ifndef GAIT_CYCLE_CLASSIFIER
#define GAIT_CYCLE_CLASSIFIER

#include <string>

#include "feature_extractor.hpp"

using namespace std;

//The phases of the gait cycle
enum Gait_cycle_states{
	HEEL_STRIKE,
	FLAT_FOOT,
	MID_STANCE,
	HEEL_OFF,
	TOE_OFF,
	MID_SWING
};

//Contains the specific feature and max age of a criterion
//These are used to specify the features required to change from one state to another in the state machine
struct State_criterion{
	Feature_type criterion_feature;
	unsigned int criterion_max_age;
};

//This struct represents one state of the state machine.
//It contains information on the current state, the index of the next state,
//and the criteria (features) required to change to the next state
struct State{
	Gait_cycle_states current_state;
	uint8_t next_state; //index of the next state

	uint8_t ay_num_criteria;
	State_criterion *ay_criteria_for_state;
	
	unsigned int gy_num_criteria;
	State_criterion *gy_criteria_for_state;
};

//Stores the information of a recognized state (type, and time recognized)
struct State_recognized_info{
	Gait_cycle_states recognized_state;
	unsigned int time_recognized;
};

//This class encapsulates classification of features into gait cycle states
class Gait_cycle_classifier{
protected:
	//The states (phases) of the gait cycle, used by the state machine
	State gait_cycle_states[6];
	//The current state of the gait cycle, used by the state machine
	State current_state;

	//Extracts the features from the y-axis accelerometer signal in real time
	Feature_extractor *accel_y; 

	//Extracts the features from the y-axis gyroscope signal in real time
	Feature_extractor *gyro_y;

public:

	Gait_cycle_classifier();
	~Gait_cycle_classifier();

	//This function is used to input data points into the classifier.
	//When a state is recognized, the members of state_info is updated and the function returns true.
	//This function is powered by the state machine
	bool intake_data(int ay_value, int gy_value, unsigned int time_value, State_recognized_info & state_info);
};

//A function for converting a state into human-readable text, for use during demonstration
string get_state_string(Gait_cycle_states state);

//This function sets each state in the state machine,
//including the state's name, the criteria to change to the next state,
//and the index of the next state
void initialize_FSM(State states_array[]);

#endif