#include <iostream>
#include "SCServo.h"
#include <vector>
#include <fstream>  
#include <cstdlib>

SCSCL sc;

int id = 4;

//declare space variables
int milis = 1000;
int timeStep = 0;
std::vector<int> timeData;   // Time data points
std::vector<double> pos;  // Speed data points

// represent the Speeds as Loads
 double speedToLoad(double crntSpeed){
    double Load = 2.3 + (crntSpeed * 2.3 / 1200);
    return Load;
 }

// remove outliers from speed measurements
bool Outlier(double crntSpeed){
    if (crntSpeed < -1200|| crntSpeed > 0){
        return true;
    }
    else return false;

}

// update data on time and speed vectors
void addData(int iterations){
    for (int i = 0; i < iterations; i++){
        int position = sc.ReadPos(id);
        timeData.push_back(timeStep);
        pos.push_back(position);
        usleep(milis);
        timeStep += 1;     
    }
}

int main(int argc, char **argv){

    // open port
    const char* serialPort = "/dev/ttyAMA0";
    std::cout << "Using serial port: " << serialPort << std::endl;
    if (!sc.begin(1000000, serialPort)) {
        std::cout << "Failed to init SCSCL motor!" << std::endl;
        return 0;
    }

    // open file
    std::ofstream outFile("Data.txt");
    if (!outFile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // declare variables form measuring rotations
    int rotations = 25;
    int Counter = 0;
    bool Rotation = false;
    int StrtPos = sc.ReadPos(id);
    int Pos;

    // set servo as PWM mode and send velocity
    sc.PWMMode(id);
    sc.WritePWM(id, 800);
    usleep(100 * milis);

    // loop to complete rotations while collecting data
    while(Counter < rotations){

        // read speed and position
        Pos = sc.ReadPos(id);

        // check whether a rotation has been completed
        if(Pos >= StrtPos - 20 && Pos <= StrtPos + 20){
            Counter += 1;    
            Rotation = true;
            std::cout<<"Rotations = "<<Counter<<std::endl;        
        }

        // not sure if this needed...
        usleep(milis);

        // half second delay if a rotation is recorded - prevents double measurement
        if(Rotation){
            if(Counter == rotations) addData(1);        
            else addData(100);
            Rotation = false;          
        }

        // dont delay if ther is no rotation recorded
        if(!Rotation){
            addData(1);
        }

    }

    // stop servo
    sc.WritePWM(id, 0);

    //write data from vectors to file
    for (size_t i = 0; i < timeData.size(); ++i) {
        outFile << timeData[i] << " " << pos[i] << "\n";
    }
    outFile.close();

    // plot data
    std::system("gnuplot -e \"plot 'Data.txt' using 1:2 with lines title 'Load vs Time'\" -e \"pause -1\"");
    return 0;

    // close servo
    sc.end();
	return 1;
}

