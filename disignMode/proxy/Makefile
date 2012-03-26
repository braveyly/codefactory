all:proxy
proxy:test_proxy.cpp Forum.cpp DbForum.cpp ForumProxy.cpp
	g++ $? -o $@
clean:
	rm -rf *.o *.orig proxy
run:
	./proxy
