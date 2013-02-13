CFLAGS = -std=c99 -g
LDFLAGS = -lgc -g
FILES = hash_table.o memory_region.o util.o block.o global.o object.o \
	parse.o selector.o value.o builtins.o method_table.o main.o
default: all
all: assembler
assembler: $(FILES)
	${CC} ${LDFLAGS} -o assembler $(FILES)
%.d: %.c
	        $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		    [ -s $@ ] || rm -f $@'
include $(FILES:.o=.d)
clean:
	rm -f *.o
	rm -f *.d
	rm -f assembler
