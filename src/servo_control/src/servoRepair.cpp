#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include "SCServo.h"
#include <unistd.h>
#include <iostream>
#include <vector>

class servoRepair : public rclcpp::Node {
public:

    servoRepair() : Node("servoRepair") {
        using std::placeholders::_1;
        subscription_ = this->create_subscription<std_msgs::msg::Int32>(
            "servo_id", 10, std::bind(&servoRepair::servo_callback, this, _1));

        const char* serialPort = "/dev/ttyAMA0";
        RCLCPP_INFO(this->get_logger(), "Using serial port: %s", serialPort);

        sc.begin(1000000, serialPort);
        sm_st.begin(1000000, serialPort);
    }

    ~servoRepair() {
        sc.end();
        sm_st.end();
    }

private:

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    SCSCL sc;
    SMS_STS sm_st;

    std::vector<int> SC09 = {4, 5, 6, 7, 8, 9};
    std::vector<int> SC3215 = {1, 2, 3};

    void repair_servo(int id) {
        RCLCPP_INFO(this->get_logger(), "Repairing and configuring Servo ID %d...", id);

        if (sc.Ping(id) < 0) {
            RCLCPP_WARN(this->get_logger(), "Servo ID %d not found. Skipping repair.", id);
            return;
        }

        sc.writeByte(id, 0x30, 0);     // disable write lock
        //sc.writeByte(id, 0x05, id);    // write current ID to EEPROM
        //sc.writeByte(id, 0x0E, 90);    // max voltage = 9.0V
        //sc.writeByte(id, 0x0F, 45);    // min voltage = 4.5V
        //sc.writeByte(id, 0x0D, 70);    // max temp = 70°C
        //sc.writeByte(id, 0x13, 32);    // unload condition = voltage + overload only
        //sc.writeWord(id, 0x18, 45);    // min starting force / punch
        //sc.writeByte(id, 0x12, 1);  // set phase
        //sc.writeByte(id, 0x27,244);  // set overload torque
        sc.writeByte(id, 0x28,1);  // torque switch
        //sc.writeWord(id, 0x09, 20);     // CW angle limit
        //sc.writeWord(id, 0x0B, 1023);  // CCW angle limit (300° in 10-bit units)
        sc.writeWord(id, 0x10,1000); // set max torque
        //sc.writeByte(id, 0x13, 0x00);  // Disables auto-unload completely
        // Keep everything else except overload
        //sc.writeWord(id, 0x1D,0);
        sc.writeByte(id, 0x30, 1);     // re-enable write lock
        //sc.writeByte(id, 0x28, 1);  // Enable torque output



        RCLCPP_INFO(this->get_logger(), "Servo ID %d repair complete. Power cycle recommended.", id);
    }  
    
    void repair_servo_st(int id) {
        RCLCPP_INFO(this->get_logger(), "Repairing and configuring Servo ID %d...", id);

        if (sm_st.Ping(id) < 0) {
            RCLCPP_WARN(this->get_logger(), "Servo ID %d not found. Skipping repair.", id);
            return;
        }

        sm_st.writeByte(id, 0x30, 0);     // disable write lock
        //sm_st.writeByte(id, 0x05, id);    // write current ID to EEPROM
        //sm_st.writeByte(id, 0x0E, 90);    // max voltage = 9.0V
        //sm_st.writeByte(id, 0x0F, 45);    // min voltage = 4.5V
        //sm_st.writeByte(id, 0x0D, 70);    // max temp = 70°C
        //sm_st.writeByte(id, 0x13, 32);    // unload condition = voltage + overload only
        //sm_st.writeWord(id, 0x18, 45);    // min starting force / punch
        //sm_st.writeByte(id, 0x12, 1);  // set phase
        //sm_st.writeByte(id, 0x27,244);  // set overload torque
        sm_st.writeByte(id, 0x28,1);  // torque switch
        //sm_st.writeWord(id, 0x09, 20);     // CW angle limit
        //sm_st.writeWord(id, 0x0B, 1023);  // CCW angle limit (300° in 10-bit units)
        sm_st.writeWord(id, 0x10,1000); // set max torque
        //sm_st.writeByte(id, 0x13, 0x00);  // Disables auto-unload completely
        // Keep everything else except overload
        //sm_st.writeWord(id, 0x1D,0);
        sm_st.writeByte(id, 0x30, 1);     // re-enable write lock
        //sm_st.writeByte(id, 0x28, 1);  // Enable torque output

        RCLCPP_INFO(this->get_logger(), "Servo ID %d repair complete. Power cycle recommended.", id);
    }

    void servo_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        int id = msg->data;
        RCLCPP_INFO(this->get_logger(), "Received request to inspect and repair Servo ID: %d", id);

        if (std::find(SC09.begin(), SC09.end(), id) != SC09.end()) {
            repair_servo(id);

        } else if (std::find(SC3215.begin(), SC3215.end(), id) != SC3215.end()) {
            repair_servo_st(id);

        } else {
            RCLCPP_WARN(this->get_logger(), "Unknown servo ID: %d", id);
        }
        
    }
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<servoRepair>());
    rclcpp::shutdown();
    return 0;
}
