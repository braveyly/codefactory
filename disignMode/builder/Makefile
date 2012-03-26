all:build

build:product.cpp builder.cpp concrete_builder_a.cpp concrete_builder_b.cpp director.cpp test_builder.cpp
	g++ product.cpp builder.cpp concrete_builder_a.cpp concrete_builder_b.cpp director.cpp test_builder.cpp -o builder

clean:
	rm -rf *.o *.orig builder

run:
	./builder
