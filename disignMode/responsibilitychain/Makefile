all:chain
chain: HelpHandler.cpp Application.cpp Dialog.cpp Button.cpp Text.cpp test_chain.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig chain
run:
	./chain
