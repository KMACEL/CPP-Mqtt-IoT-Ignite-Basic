CXX = g++

PAHO_C_DIR ?= $(abspath ../paho.mqtt.c)
PAHO_C_LIB_DIR ?= $(PAHO_C_DIR)/build/output
PAHO_C_INC_DIR ?= $(PAHO_C_DIR)/src

CPPFLAGS += -I../paho.mqtt.cpp/src -I$(PAHO_C_INC_DIR)
CPPFLAGS += -DOPENSSL
CPPFLAGS += -DDEBUG

CXXFLAGS += -Wall -std=c++11
CXXFLAGS += -g -O0
LDLIBS_SSL += -L../paho.mqtt.cpp/lib -L$(PAHO_C_LIB_DIR) -lpaho-mqttpp3 -lpaho-mqtt3as

all: run

ignite: src/main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDLIBS_SSL)

run: ignite
	 LD_LIBRARY_PATH=../paho.mqtt.cpp/lib ./ignite