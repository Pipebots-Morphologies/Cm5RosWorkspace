#include <iostream>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include "SCServo.h"


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    if (argc < 4) {
        std::cerr << "Usage: ros2 run my_package change_servo_id <old_id> <new_id> <servo_type>" << std::endl;
        return 1;
    }

    int old_id = std::stoi(argv[1]);
    int new_id = std::stoi(argv[2]);
    int servo_type = std::stoi(argv[3]);

    std::cout << "Changing ID from " << old_id << " to " << new_id << std::endl;



    if(servo_type == 1){
        SCSerial sc;
        sc.begin(1000000, "/dev/ttyAMA0"); // Adjust port and baudrate
        sc.writeByte(old_id, 0x30, 0);
        usleep(1000);
        sc.writeByte(old_id, 0x05, new_id);   
        usleep(1000);    
        sc.writeByte(new_id, 0x30, 1);

        int ID = sc.Ping(new_id);
	    std::cout<<"ID detected at "<<std::to_string(ID)<<std::endl;
    }

    if(servo_type == 2){
        SMS_STS sm_st;
        sm_st.begin(1000000, "/dev/ttyAMA0");

        sm_st.unLockEprom(old_id);
	    sm_st.writeByte(old_id, 0x05, new_id);

	    sm_st.LockEprom(new_id);
        
        int ID = sm_st.Ping(new_id);
	    std::cout<<"ID detected at "<<std::to_string(ID)<<std::endl;
    }

    rclcpp::shutdown();
    return 0;
}
