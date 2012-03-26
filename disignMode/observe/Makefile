all:observe
observe:Subject.cpp DataSubject.cpp Observe.cpp TableObserve.cpp ColumnObserve.cpp CobObserve.cpp test_observe.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig observe
run:
	./observe
