# Makefile to build

CC=g++ 
CFLAGS=-std=c++0x 
LDFLAGS = -L/usr/local/lib -L/usr/lib  -L$(HOME)/CedarLib -lzmq -lprotobuf -lglog -ljsoncpp
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I/usr/include \
		-I./include \
		-I../ProtoBufMsg \
		-I../MsgHub/include \
		-I../CedarJsonConfig/include \
		-I../CedarHelper/include \
		-I../ProtoBufMsgHub/include

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(PROTOBUF)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/msgHub
TESTCLIENT=$(BIN)/testClient

all: $(TESTCLIENT) $(EXECUTABLE)

$(TESTCLIENT):
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) src/TestClient.cpp $(PROTOBUF) $(LDFLAGS) -o $@ 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(TESTCLIENT) $(SRC)/*.o
