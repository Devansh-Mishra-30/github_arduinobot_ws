#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <arduinobot_msgs/srv/add_twoints.hpp>
#include <chrono>
using std::placeholders::_1;
using namespace std::chrono_literals;

class SimpleServiceClient : public rclcpp::Node
{
public:
    SimpleServiceClient(int a, int b) : Node("simple_service_client")
    {
        client_ = create_client<arduinobot_msgs::srv::AddTwoints>("add_two_ints");
        auto request = std::make_shared<arduinobot_msgs::srv::AddTwoints::Request>();
        request ->a = a;
        request ->b = b;

        while(!client_->wait_for_service(1s))
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Service not available, waiting more time...");
            if(!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for service");
                return;
            }
        }
        auto result = client_ ->async_send_request(request, std::bind(&SimpleServiceClient::responseCallback, this, _1));
    }
private:
    rclcpp::Client<arduinobot_msgs::srv::AddTwoints>::SharedPtr client_;

    void responseCallback(rclcpp::Client<arduinobot_msgs::srv::AddTwoints>::SharedFuture future)
    {
        if(future.valid())
        {
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Service Response " << future.get()->sum);
        }
        else
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Service Failure");

        }
    }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);

    if(argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Wrong number of arguments! Usage: simple_service_client A B");
        return 1;
    }

    auto node = std::make_shared<SimpleServiceClient>(atoi(argv[1]), atoi(argv[2]));
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}