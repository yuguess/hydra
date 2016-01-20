# Makefile to build

CC=g++ 
CFLAGS=-std=c++0x 
LDFLAGS = -L/usr/local/lib -L/usr/lib  -L$(HOME)/CedarLib -L../TDFAPI/lib -lzmq -lprotobuf -lglog -ljsoncpp -lTDFAPI30
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I/usr/include \
		-I./include \
		-I../ProtoBufMsg \
		-I../MsgHub/include \
		-I../TDFAPI/include \
		-I../CedarJsonConfig/include \
		-I../CedarHelper/include \
		-I../ProtoBufMsgHub/include \
		-I$(HOME)/CedarInclude/WindMarketData

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(SRC)/WindMarket.cpp $(PROTOBUF)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/windMD
TESTSTRTGY=$(BIN)/testStrtgy

all: $(TESTSTRTGY) $(EXECUTABLE) 

$(TESTSTRTGY):
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) src/testStrtgy.cpp $(PROTOBUF) $(LDFLAGS) -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(TESTCLIENT) $(TESTSTRTGY) $(SRC)/*.o
