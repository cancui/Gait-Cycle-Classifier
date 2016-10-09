# Gait Cycle Classifier (Graphical representation of output below!)
##### Intro
A gait cycle is the sequence of events that a person's feet go through during walking, which together make up a stride. Knowing which phase of the gait cycle the user is in at any given moment can be useful for many wearables. By collecting and classifying accelerometer and gyroscope data of one of the user's feet, we can derive this information. 

##### Summary
- This is a real-time algorithm that classifies accelerometer and gyroscope data to identify the current phase of the user's gait cycle
- All 6 phases of the classic gait cycle can be identified
- The algorithm assumes IMU (accelerometer and gyroscope) is placed on the bridge of the right foot

### Technical Highlights
- Feature extraction and all other frequently-called functions run with constant time complexity
- The algorithm uses exclusively fixed-point arithmetic 
- The algorithm works with 100% accuracy on the two sample data sets, and is able to tolorate slight differences in features 
- The feature classifier is implemented with a state machine. This makes it easy to organize, add, and modify conditions for each gait cycle phase. It's also easier to incorportate data from additional sensors
- The algorithm is encapsulated in the *Gait_cycle_classifier* class, for easy implementation

### Graphical Representation of Output

This first graph indicates where the heel strikes were detected in walkData1.csv. The black vertical lines represent the moment in time that heel strike was detected. Only the signal from Gyro. Y is plotted, for the sake of brevity.

![alt text](https://github.com/cancui/Gait-Cycle-Classifier/blob/master/datasets/heel_strikes.png "Logo Title Text 1")

This next graph indicates where every phase was detected in the first two full cycles of walkData1.csv. Only the signal from Gyro. Y is plotted, for the sake of brevity.

![alt text](https://github.com/cancui/Gait-Cycle-Classifier/blob/master/datasets/all_phases.png "Logo Title Text 1")

### High Level Algorithm Overview
Note that main.cpp is used purely for demonstration purposes. The algorithm is encapsulated in the *Gait_cycle_classifier* class.

1. Intake data points in real time
  * This is done by calling a function and passing the data points as the arguments. Additionally, a struct for gait phase information is passed by reference so that the function can return information on the gait cycle phase when a new phase is detected
2. Identify features such as spikes, troughs, and areas of little activity in the data
  * This capability is encapsulated by a class called Feature_extractor
3. Every time a new feature is identified, all the recently-identified features are compared against the features required to advance to the next state (next gait cycle phase)
  * This is encapsulated in a class called Gait_cycle_classifier , which contains instances of Feature_extractor (one for each signal)
  * Gait_cycle_classifier uses a state machine to organize and store the criteria required to advance from one phase to the next
  * For example, the features required to move from heel strike to foot flat are (1) a positive edge through a high threshold in the gyro. Z signal within the last 150ms and (2) a negative edge through a high threshold in the gyro. Z signal within the last 30ms
  * When the current state advances to the next, the criteria to advance to the next stage is updated by the state machine
4. When a new phase is detected, the information regarding what phase it is and when it was detected are passed to the caller

### Note
I used an open source .csv parser in this project (csvparser.cpp, csvparser.h). It can be found here: https://sourceforge.net/projects/cccsvparser/
