all:strategy
strategy:Context.cpp StrategyA.cpp StrategyB.cpp Strategy.cpp test_strategy.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig strategy
run:
	./strategy
