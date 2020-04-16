TARGET = gpio-test

OBJECTS = gpio-test.o

PREFIX ?= /usr/local
DESTDIR ?= $(PREFIX)/bin/

DEPENDENCIES = .dependencies
EXTRADEPS = Makefile

CC ?= $(CROSS_COMPILE)gcc
CFLAGS += -Wall -Wextra -Werror
LDFLAGS ?=
LIBS += -lpthread

all: $(DEPENDENCIES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o $(TARGET) $(DEPENDENCIES)

install: all
	install -d $(DESTDIR)
	install -m 0755 $(TARGET) $(DESTDIR)
uninstall: deinstall

deinstall:
	rm -f $(DESTDIR)$(TARGET)

$(DEPENDENCIES):
	@$(CC) -MM *.c | sed -e 's/\([^\]\)$$/\1 $(EXTRADEPS)/' \
		> $(DEPENDENCIES)

.PHONY: all clean install

-include $(DEPENDENCIES)
