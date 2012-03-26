all:bridge
bridge:window.cpp icon_window.cpp transient_window.cpp xwindow_imp.cpp pmwindow_imp.cpp test_bridge.cpp
	g++ window.cpp icon_window.cpp transient_window.cpp xwindow_imp.cpp pmwindow_imp.cpp test_bridge.cpp -o bridge
clean:
	rm -rf *.o *.orig bridge
run:
	./bridge
