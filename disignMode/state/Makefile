all:state
state:TCPConnection.cpp TCPState.cpp TCPEstablished.cpp TCPListen.cpp TCPClosed.cpp test_state.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig state
run:
	./state
