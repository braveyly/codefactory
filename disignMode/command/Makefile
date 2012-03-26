all:command
command:Command.cpp Invoker.cpp MenuItem.cpp Receiver.cpp ToolBar.cpp Document.cpp Picture.cpp CopyCommand.cpp PasteCommand.cpp test_command.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig command
run:
	./command
