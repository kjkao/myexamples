CC=gcc
TARGET=$(notdir ${PWD})
MOBJ=main.o
CFLAGS+=-Wall

$(TARGET): $(MOBJ)
	$(CC) -o $@ $^

.c.o:
	$(CC) ${CFLAGS} -o $@ -c $<

clean:
	rm -f $(TARGET) $(MOBJ)
