
#include <iostream>
#include "SCServo.h"

SMS_STS sm_st;

int main(int argc, char **argv)
{

    const char* serialPort = "/dev/ttyAMA0";
	std::cout << "Using serial port: " << serialPort << std::endl;
    if (!sm_st.begin(1000000, serialPort)) {
        std::cout << "Failed to init SCSCL motor!" << std::endl;
        return 0;
    }

	while(1){
		sm_st.WritePosEx(2, 4095, 2400, 50);
		std::cout<<"pos = "<<4095<<std::endl;
		usleep(2187*1000);
  
		sm_st.WritePosEx(2, 0, 2400, 50);
		std::cout<<"pos = "<<0<<std::endl;
		usleep(2187*1000);
	}
	sm_st.end();
	return 1;
}

