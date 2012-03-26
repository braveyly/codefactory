all:decorator
decorator:concrete_component.cpp concrete_decorator_a.cpp concrete_decorator_b.cpp test_decorator.cpp
	g++ concrete_component.cpp concrete_decorator_a.cpp concrete_decorator_b.cpp test_decorator.cpp -o decorator
clean:
	rm -rf *.o *.orig decorator
run:
	./decorator
