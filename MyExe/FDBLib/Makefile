#Please Set Following Parameters#
#1.exe file name
EXENAME:=fdb

#2.source file directory, allow multiple ones
SRCDIRS:=.        

#3.complier command and flags
CC:=gcc                   
CFLAGS:=

#4.linkage command and flags
LD:=gcc
LDFLAGS:=

#5.depend flags
DEPENDFLAG:=-MM

#6.exterior hender and lib files 
INCLUDES:=
LIBS:=-lpthread

#7.source file suffix
SUFFIX:=c

SRCS:=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.$(SUFFIX)))
BJS:=$(SRCS:.$(SUFFIX)=.o)
DEPENDS:=$(SRCS:.$(SUFFIX)=.d)

all:$(EXENAME)

$(EXENAME):$(BJS)
	$(LD) $(INCLUDES) $(LIBS) $(LDFLAGS) $? -o $@

%.d:%.$(SUFFIX)
	rm -f $@; \
	$(CC) $(DEPENDFLAG) $< >$@.$$$$; \
	sed 's,\($*\)\.o[:]*,\1.o $@ :,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$


include $(DEPENDS)

.PHONY:clean
clean:
	-rm -f *.o *.d* $(EXENAME)

.PHONY:run
run:
	./$(EXENAME)
