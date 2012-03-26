all:facade
facade:facade.cpp subsystem_a.cpp subsystem_b.cpp test_facade.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig facade
run:
	./facade
