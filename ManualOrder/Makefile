# Makefile to build

CC=g++ 
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_THREAD_COUNT=2
#CFLAGS=-std=c++0x -D_GLIBCXX_USE_SCHED_YIELD -DGCC_44 -D_TAIL_Q_START
#CFLAGS=-O2 -std=c++0x  
#CFLAGS=  -O3 -std=c++0x -DUSE_QUEUE

CFLAGS=-std=c++0x 
LDFLAGS = -L/usr/local/lib -L$(HOME)/CedarLib -ljsoncpp -lprotobuf -lzmq -lpthread
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I./include \
		-I../ProtoBufMsg \
		-I../CedarHelper/include \
	 	-I../CedarJsonConfig/include \
		-I../ProtoBufMsgHub/include \

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(SRC)/ManualOrder.cpp  $(PROTOBUF)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/ManualOrder 

TESTCASE_SRC= $(SRC)/Testcase.cpp $(PROTOBUF)
TESTCASE_OBJECTS=$(TESTCASE_SRC:.cpp=.o)
TESTCASE=$(BIN)/Testcase

all: $(EXECUTABLE) $(TESTCASE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS)  -o $@ $(LDFLAGS)

$(TESTCASE): $(TESTCASE_OBJECTS)
	$(CC) $(TESTCASE_OBJECTS)  -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(SRC)/*.o
