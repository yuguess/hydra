# Makefile to build

CC=g++ 
CFLAGS=-std=c++0x 
LDFLAGS = -L/usr/local/lib -L/usr/lib  -L$(HOME)/CedarLib -L../TDFAPI/lib -lzmq -lprotobuf -lglog -ljsoncpp -lTDFAPI30 -lev
DEBUG = -g
BIN=./bin
SRC=./src

INCLUDE= -I/usr/include \
		-I./include \
		-I../ProtoBufMsg \
		-I../MsgHub/include \
		-I../TDFAPI/include \
		-I../CedarHelper/include \
		-I../CedarJsonConfig/include \
		-I../ProtoBufMsgHub/include \

PROTOBUF= ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc

SOURCES= $(SRC)/Main.cpp $(SRC)/BackTester.cpp $(SRC)/MdAdapter.cpp $(PROTOBUF)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(BIN)/BackTester

all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(DEBUG)  $(INCLUDE) $< -o $@

# --- remove binary and executable files
clean:
	rm $(EXECUTABLE) $(SRC)/*.o
