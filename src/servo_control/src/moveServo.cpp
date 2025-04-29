#include <rclcpp/rclcpp.hpp>
#include "SCServo.h"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstdlib>

class ServoController : public rclcpp::Node {
public:
    ServoController(int id, int pos) : Node("servo_controller"), servo_id(id), target_pos(pos) {
        const char* serialPort = "/dev/ttyAMA0";
        RCLCPP_INFO(this->get_logger(), "Using serial port: %s", serialPort);

        if (!sm_st.begin(1000000, serialPort)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to init SCSCL motor!");
        }

        sc.begin(1000000, serialPort);  

        RCLCPP_INFO(this->get_logger(), "ServoController initialized with ID: %d, Pos: %d", servo_id, target_pos);

        if (std::find(SC09.begin(), SC09.end(), servo_id) != SC09.end()) {
            moveSc(servo_id, target_pos);
        } else if (std::find(SC3215.begin(), SC3215.end(), servo_id) != SC3215.end()) {
            moveSt(servo_id, target_pos);
        } else {
            RCLCPP_WARN(this->get_logger(), "Unknown servo ID: %d", servo_id);
        }

        sc.end();
        sm_st.end();
    }

private:
    SMS_STS sm_st;
    SCSCL sc;

    std::vector<int> SC09 = {4, 5, 6, 7, 8, 9};
    std::vector<int> SC3215 = {1, 2, 3};
    int servo_id;
    int target_pos;
    int speed = 800;

    void moveSc(int id, int pos) {

        sc.writeByte(id, 0x30, 0);
        sc.writeWord(id, 0x09, 20);     // CW angle limit
        sc.writeWord(id, 0x0B, 1003);
        sc.writeByte(id, 0x30, 0);

        usleep(100000);
        // 20 - 1003 mid point of 510
        sc.WritePos(id, pos, 0, speed);
        RCLCPP_INFO(this->get_logger(), "SC%02d -> pos = %d", id, pos);
    }

    void moveSt(int id, int pos) {
        // 0 - 4095 mid point of 2048 
        sm_st.WritePosEx(id, pos, 2400, 50);
        RCLCPP_INFO(this->get_logger(), "ST%02d -> pos = %d", id, pos);
    }
};

int main(int argc, char * argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: servo_controller <servo_id> <position>\n";
        return 1;
    }

    int id = std::atoi(argv[1]);
    int pos = std::atoi(argv[2]);

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ServoController>(id, pos);
    rclcpp::shutdown();
    return 0;
}
