CFLAGS = -std=c99 -g -O2 ${OPTFLAGS} -funroll-loops
LDFLAGS = -lgc -g
FILES = bytecode.o
ALL = $(FILES)
default: all
all: interpreter
interpreter: $(FILES)
	${CC} -o interpreter $(FILES) ${OPTFLAGS} ${LDFLAGS}
%.d: %.c
	        $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		    [ -s $@ ] || rm -f $@'
include $(FILES:.o=.d)
clean:
	rm -f *.o
	rm -f *.d
	rm -f interpreter
