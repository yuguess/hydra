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
		-I$(HOME)/CedarInclude/WindDataCollector

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(SRC)/WindDataCollector.cpp $(PROTOBUF)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/windDC

all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(SRC)/*.o
