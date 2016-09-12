.PHONY: all cpp python clean

all: cpp python

cpp:    CPlusPlusMain
python: PythonMain

clean:
	rm -f CPlusPlusMain PythonMain 
	rm -f protoc_middleman 
	rm -f *.pyc

protoc_middleman: ProtoBufMsg.proto
	protoc --cpp_out=CPlusPlusCode --python_out=PythonCode ProtoBufMsg.proto

CPlusPlusMain: CPlusPlusMain.cpp protoc_middleman
	g++ CPlusPlusMain.cpp CPlusPlusCode/ProtoBufMsg.pb.cc -o CPlusPlusMain -lprotobuf

PythonMain: PythonMain.py protoc_middleman
	@echo '#! /bin/sh' > PythonMain 
	@echo './PythonMain.py "$$@"' >> PythonMain
	@chmod +x PythonMain
