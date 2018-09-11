//============================================================================
// Name        : CPP-Mqtt-IoT-Ignite-Basic
// Writer      : Mert AceL
// Version     : 1.0
// Copyright   : AceL
// Description : IoT-Ignite Connection and Send Message Example
//============================================================================

#include <iostream>
#include <iostream>
#include <string>
#include <chrono>
#include "mqtt/async_client.h"

using namespace std;

const std::string SERVER_ADDRESS{"ssl://mqtt.ardich.com:8883"};
const std::string CLIENT_ID{"user@cpp"};
const std::string USER_NAME{"user4cpp"};
const std::string PASSWORD{"123456789"};

const std::string TOPIC{"user@cpp/publish/DeviceProfile/CPPNode/CPPSensor"};
const char *PAYLOAD1 = "{data:{sensorData:[{date:1536667591000,values:[36]}],formatVersion:2}}";
const int QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);
const int RETRY_ATTEMPTS = 5;
const auto RECONNECT_TIME = std::chrono::milliseconds(2500);

//==================== Listener ====================
class ActionListener : public virtual mqtt::iaction_listener
{
  private:
    std::string name;

  private:
    void on_failure(const mqtt::token &tok) override
    {
        std::cout << name << " Failure ";
        if (tok.get_message_id() != 0)
        {
            std::cout << "For TOKEN : " << tok.get_message_id() << std::endl;
        }
        std::cout << std::endl;
    }

    void on_success(const mqtt::token &tok) override
    {
        std::cout << name << " Success ";
        if (tok.get_message_id() != 0)
        {
            std::cout << "For TOKEN :" << tok.get_message_id() << std::endl;
        }

        auto top = tok.get_topics();
        if (top && !top->empty())
        {
            std::cout << "\tToken Topic: '" << (*top)[0] << std::endl;
        }
        std::cout << std::endl;
    }

  public:
    ActionListener(const std::string &name) : name(name) {}
};
//==================== Listener END ====================

//==================== Callback ====================
class Callback : public virtual mqtt::callback,
                 public virtual mqtt::iaction_listener

{
  private:
    // Counter for the number of connection retries
    int retryCounter;
    // The MQTT client
    mqtt::async_client &asyncClient;
    // Options to use if we need to reconnect
    mqtt::connect_options &connectOptions;
    // An action listener to display the result of actions.
    ActionListener subListener;

    // This deomonstrates manually reconnecting to the broker by calling
    // connect() again. This is a possibility for an application that keeps
    // a copy of it's original connect_options, or if the app wants to
    // reconnect with different options.
    // Another way this can be done manually, if using the same options, is
    // to just call the async_client::reconnect() method.
    void reconnect()
    {
        std::this_thread::sleep_for(RECONNECT_TIME);
        std::cout << "Reconnect Size : " << retryCounter + 1 << std::endl;

        try
        {
            asyncClient.connect(connectOptions, nullptr, *this);
        }
        catch (const mqtt::exception &exc)
        {
            std::cerr << "Reconnect Error : " << exc.what() << std::endl;
            exit(1);
        }
    }

    // Re-connection failure
    void on_failure(const mqtt::token &tok) override
    {
        std::cout << "Connection failed" << std::endl;
        if (++retryCounter > RETRY_ATTEMPTS)
        {
            exit(1);
        }
        reconnect();
    }

    // Re-connection success
    void on_success(const mqtt::token &tok) override
    {
        std::cout << "\nConnection success" << std::endl;
        std::cout << "\tSubscribing to topic : " << TOPIC << std::endl
                  << "\tClient : " << CLIENT_ID << std::endl
                  << "\tQoS : " << QOS << std::endl;

        asyncClient.subscribe(TOPIC, QOS, nullptr, subListener);
    }

    // Callback for when the connection is lost.
    // This will initiate the attempt to manually reconnect.
    void connection_lost(const std::string &cause) override
    {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tCause : " << cause << std::endl;

        std::cout << "Reconnecting..." << std::endl;
        retryCounter = 0;
        reconnect();
    }

    // Callback for when a message arrives.
    void message_arrived(mqtt::const_message_ptr msg) override
    {
        std::cout << "Message arrived :" << std::endl;
        std::cout << "\tTopic : " << msg->get_topic() << std::endl;
        std::cout << "\tPayload : " << msg->to_string() << std::endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override
    {
        std::cout << "Delivery Complete" << std::endl;
    }

  public:
    Callback(mqtt::async_client &cli, mqtt::connect_options &connOpts)
        : retryCounter(0), asyncClient(cli), connectOptions(connOpts), subListener("Subscription") {}
};
//==================== Callback END ====================

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{

    cout << "Initializing for server \nAdress : " << SERVER_ADDRESS << " \nClient : " << CLIENT_ID << endl;

    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
    mqtt::connect_options connopts(USER_NAME, PASSWORD);

    Callback cb(client, connopts);
    client.set_callback(cb);

    mqtt::ssl_options sslopts;
    connopts.set_ssl(sslopts);

    cout << "  ...OK" << endl;

    try
    {
        cout << "\nConnecting..." << endl;
        mqtt::token_ptr conntok = client.connect(connopts);
        cout << "Waiting for the connection..." << endl;
        conntok->wait();
        cout << "  ...OK" << endl;

        cout << "\nSending message..." << endl;
        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, PAYLOAD1);
        pubmsg->set_qos(QOS);
        client.publish(pubmsg)->wait_for(TIMEOUT);
        cout << "  ...OK" << endl;

        while (std::tolower(std::cin.get()) != 'q')
            ;

        // Disconnect
        cout << "\nDisconnecting..." << endl;
        conntok = client.disconnect();
        conntok->wait();
        cout << "  ...OK" << endl;
    }
    catch (const mqtt::exception &exc)
    {
        cerr << "Error : " << exc.what() << endl;
        return 1;
    }

    return 0;
}
