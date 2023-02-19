PROJECT = udev-rules
VERSION = 1.0
DESTDIR    ?=

GEN_SRC = \
	udev-rules-scanner.c \
	udev-rules-scanner.h \
	udev-rules-parser.c \
	udev-rules-parser.h
SRC = \
	udev-string.c \
	udev-goto-label.c \
	udev-rules.c

SRC += $(filter %.c,$(GEN_SRC))

OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

WARNINGS = -Wall -W -Wshadow -Wpointer-arith -Wwrite-strings \
	-Wconversion -Waggregate-return -Wstrict-prototypes \
	-Wmissing-prototypes -Wmissing-declarations -Wmissing-noreturn \
	-Wmissing-format-attribute -Wredundant-decls -Wdisabled-optimization

CPPFLAGS = -std=gnu99 -D_GNU_SOURCE \
	-DPACKAGE_NAME=\"$(PROJECT)\" \
	-DVERSION=\"$(VERSION)\"

CPPFLAGS += -g -O0 -Wall -Wmissing-prototypes -Wmissing-declarations -Wmissing-noreturn

ifdef DEBUG
CPPFLAGS += -DDEBUG
endif

TOUCH_R ?= touch -r

override CFLAGS += $(WARNINGS)

.PHONY:	all install clean indent

all: $(PROJECT)

udev-rules-parser.c: udev-rules-parser.y
	$(TOUCH_R) udev-rules-scanner.l udev-rules-scanner.h
	bison --defines=udev-rules-parser.h -o $@ $<

udev-rules-scanner.c: udev-rules-scanner.l udev-rules-parser.c
	flex -o $@ $<

$(OBJ): $(GEN_SRC)

$(PROJECT): $(OBJ)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	$(RM) $(PROJECT) $(DEP) $(OBJ) $(GEN_SRC) core *~

indent:
	indent -linux -cli8 -psl *.h *.c

%.d:	%.c Makefile
	@echo Making dependences for $<
	@$(CC) -MM $(CPPFLAGS) $< |sed -e 's,\($*\)\.o[ :]*,\1.o $@: Makefile ,g' >$@

ifneq ($(DEP),)
-include $(DEP)
endif
