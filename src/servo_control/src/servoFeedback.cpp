#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include "SCServo.h"
#include <unistd.h>
#include <iostream>
#include <vector>

class servoFeedback : public rclcpp::Node {
public:

    servoFeedback() : Node("servo_config") {
        using std::placeholders::_1;
        subscription_ = this->create_subscription<std_msgs::msg::Int32>(
            "servo_id", 10, std::bind(&servoFeedback::servo_callback, this, _1));

        const char* serialPort = "/dev/ttyAMA0";
        RCLCPP_INFO(this->get_logger(), "Using serial port: %s", serialPort);

        sc.begin(1000000, serialPort);
        sm_st.begin(1000000, serialPort);
    }

    ~servoFeedback() {
        sc.end();
        sm_st.end();
    }

private:

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    SCSCL sc;
    SMS_STS sm_st;

    std::vector<int> SC09 = {4, 5, 6, 7, 8, 9};
    std::vector<int> SC3215 = {1, 2, 3};

    void realtimeData(int id) {
 
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nEEPROM Dump for Servo %d:", id);
        int time = 0;
        while(time <= 50){
    
            //RCLCPP_INFO(logger, "Torque switch: %d", sc.readByte(id, 0x28));
            //RCLCPP_INFO(logger, "Target Position: %d", sc.readWord(id, 0x2A));
            //RCLCPP_INFO(logger, "Operation Time: %d", sc.readWord(id, 0x2C));
            //RCLCPP_INFO(logger, "Operation Speed: %d", sc.readWord(id, 0x2E));
            //RCLCPP_INFO(logger, "Lock Flag: %d", sc.readByte(id, 0x30));
            //RCLCPP_INFO(logger, "Current Position: %d", sc.readWord(id, 0x38));
            //RCLCPP_INFO(logger, "Current Speed: %d", sc.readWord(id, 0x3A));
            //RCLCPP_INFO(logger, "Current Load: %d", sc.readWord(id, 0x3C));
            //RCLCPP_INFO(logger, "Current Voltage: %d", sc.readByte(id, 0x3E));
            //RCLCPP_INFO(logger, "Current Temperature: %d°C", sc.readByte(id, 0x3F));
            //RCLCPP_INFO(logger, "Async Write Flag: %d", sc.readByte(id, 0x40));
            //RCLCPP_INFO(logger, "Servo Status: %d", sc.readByte(id, 0x41));
            //RCLCPP_INFO(logger, "Move Flag (0=stopped, 1=moving): %d", sc.readByte(id, 0x42));
            std::cout << sc.readWord(id, 0x38) << "\t" << sc.readWord(id, 0x3A) << "\t" << sc.readWord(id, 0x3C) << "\t" << sc.readByte(id, 0x3E) << "\t" << sc.readByte(id, 0x3F) << "\t" << sc.readByte(id, 0x41) << std::endl;


            usleep(100000);
            time +=1;
        }
    }

    void realtimeData_st(int id) {
 
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nEEPROM Dump for Servo %d:", id);
        int time = 0;
        while(time <= 50){
    
            //RCLCPP_INFO(logger, "Torque switch: %d", sm_st.readByte(id, 0x28));
            //RCLCPP_INFO(logger, "Target Position: %d", sm_st.readWord(id, 0x2A));
            //RCLCPP_INFO(logger, "Operation Time: %d", sm_st.readWord(id, 0x2C));
            //RCLCPP_INFO(logger, "Operation Speed: %d", sm_st.readWord(id, 0x2E));
            //RCLCPP_INFO(logger, "Lock Flag: %d", sm_st.readByte(id, 0x30));
            //RCLCPP_INFO(logger, "Current Position: %d", sm_st.readWord(id, 0x38));
            //RCLCPP_INFO(logger, "Current Speed: %d", sm_st.readWord(id, 0x3A));
            //RCLCPP_INFO(logger, "Current Load: %d", sm_st.readWord(id, 0x3C));
            //RCLCPP_INFO(logger, "Current Voltage: %d", sm_st.readByte(id, 0x3E));
            //RCLCPP_INFO(logger, "Current Temperature: %d°C", sm_st.readByte(id, 0x3F));
            //RCLCPP_INFO(logger, "Async Write Flag: %d", sm_st.readByte(id, 0x40));
            //RCLCPP_INFO(logger, "Servo Status: %d", sm_st.readByte(id, 0x41));
            //RCLCPP_INFO(logger, "Move Flag (0=stopped, 1=moving): %d", sm_st.readByte(id, 0x42));
            std::cout << sm_st.readWord(id, 0x38) << "\t" << sm_st.readWord(id, 0x3A) << "\t" << sm_st.readWord(id, 0x3C) << "\t" << sm_st.readByte(id, 0x3E) << "\t" << sm_st.readByte(id, 0x3F) << "\t" << sm_st.readByte(id, 0x41) << std::endl;

            usleep(100000);
            time +=1;
        }
    }


    void servo_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        int id = msg->data;
        RCLCPP_INFO(this->get_logger(), "Received request to inspect and repair Servo ID: %d", id);

        if (std::find(SC09.begin(), SC09.end(), id) != SC09.end()) {
            realtimeData(id);

        } else if (std::find(SC3215.begin(), SC3215.end(), id) != SC3215.end()) {
            realtimeData_st(id);

        } else {
            RCLCPP_WARN(this->get_logger(), "Unknown servo ID: %d", id);
        }
        
    }
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<servoFeedback>());
    rclcpp::shutdown();
    return 0;
}
