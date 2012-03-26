all:template
template:AlgorithmTemplate.cpp ComplicatedAlgorithm.cpp SimpleAlgorithm.cpp test_template.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig template
run:
	./template
