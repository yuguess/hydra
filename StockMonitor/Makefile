# Makefile to build

CC=g++ 
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_THREAD_COUNT=2
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_TAIL_Q_START
#CFLAGS=-O2 -std=c++0x  
#CFLAGS=  -O3 -std=c++0x -DUSE_QUEUE

CFLAGS=-std=c++0x 
#LDFLAGS = -L/usr/local/lib -L/usr/lib -L/bld/dev_multi -llzo2 -lpthread -lrt -lconnector -llogger -lxsim -ldatamanip -lcommonutils /usr/local/lib/libsnappy.a 
LDFLAGS = -L/usr/local/lib -L/usr/lib  -L$(HOME)/CedarLib -lboost_system -lboost_thread -lglog -ljsoncpp -lprotobuf -lzmq
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I/usr/include \
	 	-I$(HOME)/CedarInclude/Core \
	 	-I$(HOME)/CedarInclude/CTP \
	 	-I$(HOME)/CedarInclude/Common \
	 	-I$(HOME)/CedarInclude/Connection \
	 	-I./include \
		-I../ProtoBufMsg \
		-I../MsgHub/include \
		-I../CedarHelper/include \
	 	-I../CedarJsonConfig/include \
		-I../ProtoBufMsgHub/include

SOURCES= $(SRC)/Main.cpp $(SRC)/StockProcessor.cpp \
		../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/StockMonitor

all: $(EXECUTABLE)  

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(OBJECTS) $(EXECUTABLE)
