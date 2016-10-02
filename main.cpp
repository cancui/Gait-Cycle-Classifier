#include <cstdlib>
#include <iostream>
#include <string>

#include "feature_extractor.hpp"
#include "gait_cycle_classifier.hpp"
#include "csvparser.h"

using namespace std;

//parse the specified .csv file and put the data into time[], ay[], gyp[], data_length
bool parse_csv(unsigned int time[], int ay[], int gy[], unsigned int &data_length, string file_location);

//main.cpp is used purely to demonstrate the functionality of my classifier. 
//The classifier is encapsulated by Gait_cycle_classifier in gait_cycle_classifier.cpp
int main()
{
	//Arrays for the datapoints of accelerometer-y and gyroscope-y
	unsigned int time_values[941];
	int accel_y_values[941];
	int gyro_y_values[941];
	unsigned int data_length = 0;

	//Read the .csv data of the first data set into the data arrays
	if(!parse_csv(time_values, accel_y_values, gyro_y_values, data_length, "datasets/walkData1.csv")){
		cout << "Error reading .csv file!" << endl;
		return -1;
	}

	//instantiate the classifier
	Gait_cycle_classifier *classifer = new Gait_cycle_classifier();

	//used to receive the information of newly-recognized states
	State_recognized_info latest_state_info;

	cout << "Starting to input \"walkData1.csv\"..." << endl;

	//inputs the data into the classifier from first to last, simulating real time application of the classifier
	for(unsigned int i = 0; i < data_length; i++){

		//if a new state(phase) was recognized, print out the state and the time it was recognized at
		if (classifer->intake_data(accel_y_values[i], gyro_y_values[i], time_values[i], latest_state_info)){
			cout << "Just recognized: " << get_state_string(latest_state_info.recognized_state) << " at time: " << latest_state_info.time_recognized << endl;
		}

	}

	cout << "Finished inputting \"walkData1.csv\"" << endl;

	delete classifer;
	classifer = NULL;


	//Read the .csv data of the second data set into the same data arrays
	if(!parse_csv(time_values, accel_y_values, gyro_y_values, data_length, "datasets/walkData2.csv")){
		cout << "Error reading .csv file!" << endl;
		return -1;
	}

	//instantiate another classifier
	Gait_cycle_classifier *classifer2 = new Gait_cycle_classifier();

	cout << endl << "Starting to input \"walkData2.csv\"..." << endl;

	for(unsigned int i = 0; i < data_length; i++){

		//if a new state(phase) was recognized, print out the state and the time it was recognized at
		if (classifer2->intake_data(accel_y_values[i], gyro_y_values[i], time_values[i], latest_state_info)){
			cout << "Just recognized: " << get_state_string(latest_state_info.recognized_state) << " at time: " << latest_state_info.time_recognized << endl;
		}

	}

	cout << "Finished inputting \"walkData2.csv\"" << endl;

	delete classifer2;
	classifer2 = NULL;

	return 0;
}

//parse the specified .csv file and put the data into time[], ay[], gyp[], data_length
bool parse_csv(unsigned int time[], int ay[], int gy[], unsigned int & data_length, string file_location)
{
	data_length = 0;

    CsvParser *csvparser = CsvParser_new(file_location.c_str(), ",", 0);

    if(!csvparser){
    	return false;
    }

    CsvRow *row;    

    row = CsvParser_getRow(csvparser);
    CsvParser_destroy_row(row);
    row = CsvParser_getRow(csvparser);
    CsvParser_destroy_row(row);

    while ((row = CsvParser_getRow(csvparser))) {
        const char **rowFields = CsvParser_getFields(row);
        if(CsvParser_getNumFields(row) < 15){
        	CsvParser_destroy_row(row);
        	break;
        }

        time[data_length] = static_cast<unsigned int>(stoi(rowFields[1]));
        ay[data_length] = stoi(rowFields[3]);
        gy[data_length] = stoi(rowFields[6]);

        CsvParser_destroy_row(row);
        data_length++;
    }

    CsvParser_destroy(csvparser);

	return true;
}