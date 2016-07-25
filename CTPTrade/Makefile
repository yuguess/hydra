# Makefile to build

CC=g++ 
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_THREAD_COUNT=2
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_TAIL_Q_START
#CFLAGS=-O2 -std=c++0x  
#CFLAGS=  -O3 -std=c++0x -DUSE_QUEUE

INFRA=/home/yuguess
CFLAGS=-std=c++0x 
LDFLAGS = -L/usr/local/lib -L/usr/lib  -L$(INFRA)/CedarLib -lzmq -lthosttraderapi -ljsoncpp -lprotobuf -lpthread
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I/usr/include \
	 		-I$(INFRA)/CedarInclude/CTP \
	 		-I./include \
			-I../CedarHelper/include \
			-I../ProtoBufMsgHub/include \
			-I../CedarJsonConfig/include \
			-I../ProtoBufMsg

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(SRC)/TradeHandler.cpp $(PROTOBUF)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/CTP_TRADE

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(SRC)/*.o
