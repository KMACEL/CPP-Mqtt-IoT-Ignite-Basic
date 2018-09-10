### **CPP-Mqtt-IoT-Ignite-Basic**

```bash
# System Installation
sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
sudo apt-get install doxygen graphviz

sudo apt-get install fakeroot fakeroot devscripts dh-make lsb-release
sudo apt-get install libssl-dev

# MQTT Installation
mkdir MQTT-Ignite
cd MQTT-Ignite

git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c/
git checkout develop 
make
sudo make install

export PAHO_MQTT_C_PATH=$PWD
export LD_LIBRARY_PATH=$PWD/build/output/
cd ..

git clone https://github.com/eclipse/paho.mqtt.cpp.git
cd paho.mqtt.cpp/
make
cd ..

# CPP-Mqtt-IoT-Ignite-Basic Installation
git clone https://github.com/KMACEL/CPP-Mqtt-IoT-Ignite-Basic.git
cd CPP-Mqtt-IoT-Ignite-Basic
```
