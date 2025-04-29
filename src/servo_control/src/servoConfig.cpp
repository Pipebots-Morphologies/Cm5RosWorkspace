#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include "SCServo.h"
#include <unistd.h>
#include <iostream>
#include <vector>

class ServoConfig : public rclcpp::Node {
public:

    ServoConfig() : Node("servo_config") {
        using std::placeholders::_1;
        subscription_ = this->create_subscription<std_msgs::msg::Int32>(
            "servo_id", 10, std::bind(&ServoConfig::servo_callback, this, _1));

        const char* serialPort = "/dev/ttyAMA0";
        RCLCPP_INFO(this->get_logger(), "Using serial port: %s", serialPort);

        sc.begin(1000000, serialPort);
        sm_st.begin(1000000, serialPort);
    }

    ~ServoConfig() {
        sc.end();
        sm_st.end();
    }

private:

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    SCSCL sc;
    SMS_STS sm_st;

    std::vector<int> SC09 = {4, 5, 6, 7, 8, 9};
    std::vector<int> SC3215 = {1, 2, 3};

    void dump_servo_ram(int id) {
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nRAM Dump for Servo %d:", id);
        
        RCLCPP_INFO(logger, "Torque switch: %d", sc.readByte(id, 0x28));
        RCLCPP_INFO(logger, "Target Position: %d", sc.readWord(id, 0x2A));
        RCLCPP_INFO(logger, "Operation Time: %d", sc.readWord(id, 0x2C));
        RCLCPP_INFO(logger, "Operation Speed: %d", sc.readWord(id, 0x2E));
        RCLCPP_INFO(logger, "Lock Flag: %d", sc.readByte(id, 0x30));
        RCLCPP_INFO(logger, "Current Position: %d", sc.readWord(id, 0x38));
        RCLCPP_INFO(logger, "Current Speed: %d", sc.readWord(id, 0x3A));
        RCLCPP_INFO(logger, "Current Load: %d", sc.readWord(id, 0x3C));
        RCLCPP_INFO(logger, "Current Voltage: %d", sc.readByte(id, 0x3E));
        RCLCPP_INFO(logger, "Current Temperature: %d°C", sc.readByte(id, 0x3F));
        RCLCPP_INFO(logger, "Async Write Flag: %d", sc.readByte(id, 0x40));
        RCLCPP_INFO(logger, "Servo Status: %d", sc.readByte(id, 0x41));
        RCLCPP_INFO(logger, "Move Flag (0=stopped, 1=moving): %d", sc.readByte(id, 0x42));
    }

    void dump_servo_eeprom(int id) {
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nEEPROM Dump for Servo %d:", id);

        RCLCPP_INFO(logger, "Firmware Major: %d", sc.readByte(id, 0x00));
        RCLCPP_INFO(logger, "Firmware Minor: %d", sc.readByte(id, 0x01));
        RCLCPP_INFO(logger, "Servo Major Ver: %d", sc.readByte(id, 0x03));
        RCLCPP_INFO(logger, "Servo Minor Ver: %d", sc.readByte(id, 0x04));
        RCLCPP_INFO(logger, "ID: %d", sc.readByte(id, 0x05));
        RCLCPP_INFO(logger, "Baud Rate: %d", sc.readByte(id, 0x06));
        RCLCPP_INFO(logger, "Return Delay Time: %d", sc.readByte(id, 0x07));
        RCLCPP_INFO(logger, "Status Return Level: %d", sc.readByte(id, 0x08));
        RCLCPP_INFO(logger, "Min Angle Limit: %d", sc.readWord(id, 0x09));
        RCLCPP_INFO(logger, "Max Angle Limit: %d", sc.readWord(id, 0x0B));
        RCLCPP_INFO(logger, "Max Temp Limit: %d°C", sc.readByte(id, 0x0D));
        RCLCPP_INFO(logger, "Voltage Limit: %d–%d (Max–Min)", sc.readByte(id, 0x0E), sc.readByte(id, 0x0F));
        RCLCPP_INFO(logger, "Max Torque: %d", sc.readWord(id, 0x10));
        RCLCPP_INFO(logger, "Phase Value: %d", sc.readByte(id, 0x12));
        RCLCPP_INFO(logger, "Unload Condition: %d", sc.readByte(id, 0x13));
        RCLCPP_INFO(logger, "LED Alarm Condition: %d", sc.readByte(id, 0x14));
        RCLCPP_INFO(logger, "PID Coefficients - P: %d, D: %d, I: %d", sc.readByte(id, 0x15), sc.readByte(id, 0x16), sc.readByte(id, 0x17));
        RCLCPP_INFO(logger, "Min Starting Force: %d", sc.readWord(id, 0x18));
        RCLCPP_INFO(logger, "CW/CCW Compliance Margin: %d / %d", sc.readByte(id, 0x1A), sc.readByte(id, 0x1B));
        RCLCPP_INFO(logger, "Hysteresis: %d", sc.readByte(id, 0x1C));
        RCLCPP_INFO(logger, "Protection Torque: %d", sc.readByte(id, 0x25));
        RCLCPP_INFO(logger, "Protection Time: %d", sc.readByte(id, 0x26));
        RCLCPP_INFO(logger, "Overload Torque: %d", sc.readByte(id, 0x27));
        RCLCPP_INFO(logger, "Punch (Min Force): %d", sc.readWord(id, 0x18));
    }

    void dump_servo_ram_st(int id) {
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nRAM Dump for Servo %d:", id);
        
        RCLCPP_INFO(logger, "Torque switch: %d", sm_st.readByte(id, 0x28));
        RCLCPP_INFO(logger, "Target Position: %d", sm_st.readWord(id, 0x2A));
        RCLCPP_INFO(logger, "Operation Time: %d", sm_st.readWord(id, 0x2C));
        RCLCPP_INFO(logger, "Operation Speed: %d", sm_st.readWord(id, 0x2E));
        RCLCPP_INFO(logger, "Lock Flag: %d", sm_st.readByte(id, 0x30));
        RCLCPP_INFO(logger, "Current Position: %d", sm_st.readWord(id, 0x38));
        RCLCPP_INFO(logger, "Current Speed: %d", sm_st.readWord(id, 0x3A));
        RCLCPP_INFO(logger, "Current Load: %d", sm_st.readWord(id, 0x3C));
        RCLCPP_INFO(logger, "Current Voltage: %d", sm_st.readByte(id, 0x3E));
        RCLCPP_INFO(logger, "Current Temperature: %d°C", sm_st.readByte(id, 0x3F));
        RCLCPP_INFO(logger, "Async Write Flag: %d", sm_st.readByte(id, 0x40));
        RCLCPP_INFO(logger, "Servo Status: %d", sm_st.readByte(id, 0x41));
        RCLCPP_INFO(logger, "Move Flag (0=stopped, 1=moving): %d", sm_st.readByte(id, 0x42));
    }

    void dump_servo_eeprom_st(int id) {
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "\nEEPROM Dump for Servo %d:", id);

        RCLCPP_INFO(logger, "Firmware Major: %d", sm_st.readByte(id, 0x00));
        RCLCPP_INFO(logger, "Firmware Minor: %d", sm_st.readByte(id, 0x01));
        RCLCPP_INFO(logger, "Servo Major Ver: %d", sm_st.readByte(id, 0x03));
        RCLCPP_INFO(logger, "Servo Minor Ver: %d", sm_st.readByte(id, 0x04));
        RCLCPP_INFO(logger, "ID: %d", sm_st.readByte(id, 0x05));
        RCLCPP_INFO(logger, "Baud Rate: %d", sm_st.readByte(id, 0x06));
        RCLCPP_INFO(logger, "Return Delay Time: %d", sm_st.readByte(id, 0x07));
        RCLCPP_INFO(logger, "Status Return Level: %d", sm_st.readByte(id, 0x08));
        RCLCPP_INFO(logger, "Min Angle Limit: %d", sm_st.readWord(id, 0x09));
        RCLCPP_INFO(logger, "Max Angle Limit: %d", sm_st.readWord(id, 0x0B));
        RCLCPP_INFO(logger, "Max Temp Limit: %d°C", sm_st.readByte(id, 0x0D));
        RCLCPP_INFO(logger, "Voltage Limit: %d–%d (Max–Min)", sm_st.readByte(id, 0x0E), sm_st.readByte(id, 0x0F));
        RCLCPP_INFO(logger, "Max Torque: %d", sm_st.readWord(id, 0x10));
        RCLCPP_INFO(logger, "Phase Value: %d", sm_st.readByte(id, 0x12));
        RCLCPP_INFO(logger, "Unload Condition: %d", sm_st.readByte(id, 0x13));
        RCLCPP_INFO(logger, "LED Alarm Condition: %d", sm_st.readByte(id, 0x14));
        RCLCPP_INFO(logger, "PID Coefficients - P: %d, D: %d, I: %d", sm_st.readByte(id, 0x15), sm_st.readByte(id, 0x16), sm_st.readByte(id, 0x17));
        RCLCPP_INFO(logger, "Min Starting Force: %d", sm_st.readWord(id, 0x18));
        RCLCPP_INFO(logger, "CW/CCW Compliance Margin: %d / %d", sm_st.readByte(id, 0x1A), sm_st.readByte(id, 0x1B));
        RCLCPP_INFO(logger, "Hysteresis: %d", sm_st.readByte(id, 0x1C));
        RCLCPP_INFO(logger, "Protection Torque: %d", sm_st.readByte(id, 0x25));
        RCLCPP_INFO(logger, "Protection Time: %d", sm_st.readByte(id, 0x26));
        RCLCPP_INFO(logger, "Overload Torque: %d", sm_st.readByte(id, 0x27));
        RCLCPP_INFO(logger, "Punch (Min Force): %d", sm_st.readWord(id, 0x18));
    }

    void servo_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        int id = msg->data;
        RCLCPP_INFO(this->get_logger(), "Received request to inspect and repair Servo ID: %d", id);

        if (std::find(SC09.begin(), SC09.end(), id) != SC09.end()) {
            dump_servo_eeprom(id);
            dump_servo_ram(id);

        } else if (std::find(SC3215.begin(), SC3215.end(), id) != SC3215.end()) {
            dump_servo_eeprom_st(id);
            dump_servo_ram_st(id);

        } else {
            RCLCPP_WARN(this->get_logger(), "Unknown servo ID: %d", id);
        }
        
    }
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ServoConfig>());
    rclcpp::shutdown();
    return 0;
}
