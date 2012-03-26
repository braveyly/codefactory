all:visitor
visitor:Visitor.cpp ConcreteVisitor1.cpp ConcreteVisitor2.cpp Element.cpp ElementA.cpp ElementB.cpp test_visitor.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig visitor
run:
	./visitor
