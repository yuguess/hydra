# Makefile to build

CC=g++
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_THREAD_COUNT=2
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_TAIL_Q_START
#CFLAGS=-O2 -std=c++0x
#CFLAGS=  -O3 -std=c++0x -DUSE_QUEUE

INFRA=/home/yuguess
CFLAGS=-std=c++0x -fPIC
LDFLAGS = -L/usr/local/lib -L$(INFRA)/CedarLib -lzmq -ljsoncpp -lprotobuf -lboost_filesystem -lboost_system  -lboost_date_time
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I$(INFRA)/CedarInclude/CTP \
	 		-I./include \
			-I../CedarHelper/include \
			-I../ProtoBufMsgHub/include \
			-I../CedarJsonConfig/include \
			-I../ProtoBufMsg

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Backtester.cpp $(SRC)/OrderBook.cpp $(PROTOBUF)
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=$(BIN)/libBacktester.so

EXEC_SRC=$(SRC)/Main.cpp $(SRC)/Backtester.cpp $(SRC)/OrderBook.cpp $(PROTOBUF)
EXEC_OBJS=$(EXEC_SRC:.cpp=.o)
EXEC=$(BIN)/Backtester

all: $(LIBRARY) $(EXEC)

$(LIBRARY): $(OBJECTS)
	$(CC) -fPIC -shared $(OBJECTS) -o $@

$(EXEC): $(EXEC_OBJS)
	$(CC) $(EXEC_OBJS)  -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(TARGET) $(SRC)/*.o
