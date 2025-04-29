#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include "SCServo.h"
#include <unistd.h>
#include <iostream>
#include <vector>

class ServoController : public rclcpp::Node {
public:
    ServoController() : Node("servo_controller") {
        using std::placeholders::_1;
        subscription_ = this->create_subscription<std_msgs::msg::Int32>(
            "servo_id", 10, std::bind(&ServoController::servo_callback, this, _1));

        // Initialize the serial connection
        const char* serialPort = "/dev/ttyAMA0";
        RCLCPP_INFO(this->get_logger(), "Using serial port: %s", serialPort);

        if (!sm_st.begin(1000000, serialPort)) {
            RCLCPP_ERROR(this->get_logger(), "Failed to init SCSCL motor!");
        }

        sc.begin(1000000, serialPort);  

        RCLCPP_INFO(this->get_logger(), "ServoController Node initialized.");
    }

    ~ServoController() {
        sc.end();
        sm_st.end();
    }

private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscription_;
    SMS_STS sm_st;
    SCSCL sc;

    std::vector<int> SC09 = {4, 5, 6, 7, 8, 9};
    std::vector<int> SC3215 = {1, 2, 3};
    int speed = 800;

    void moveSc(int id) {
        sc.WritePos(id, 1000, 0, speed);
        RCLCPP_INFO(this->get_logger(), "SC%02d pos = %d", id, 1000);
        usleep(2187 * 1000);

        sc.WritePos(id, 510, 0, speed);
        RCLCPP_INFO(this->get_logger(), "SC%02d pos = %d", id, 20);
        usleep(2187 * 1000);
    }

    void moveSt(int id) {
        sm_st.WritePosEx(id, 4095, 2400, 50);
        RCLCPP_INFO(this->get_logger(), "ST%02d pos = %d", id, 4095);
        usleep(2187 * 1000);

        sm_st.WritePosEx(id, 2048, 2400, 50);
        RCLCPP_INFO(this->get_logger(), "ST%02d pos = %d", id, 0);
        usleep(2187 * 1000);
    }

    void servo_callback(const std_msgs::msg::Int32::SharedPtr msg) {
        int id = msg->data;
        RCLCPP_INFO(this->get_logger(), "Received servo ID: %d", id);

        if (std::find(SC09.begin(), SC09.end(), id) != SC09.end()) {
            moveSc(id);
        } else if (std::find(SC3215.begin(), SC3215.end(), id) != SC3215.end()) {
            moveSt(id);
        } else {
            RCLCPP_WARN(this->get_logger(), "Unknown servo ID: %d", id);
        }
    }
};

// 🔧 Add main() here to make it executable
int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ServoController>());
    rclcpp::shutdown();
    return 0;
}
