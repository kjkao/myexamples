CC=gcc
TARGET=$(notdir ${PWD})
MOBJ=main.o
CFLAGS+=-Wall
LIBS+=-lcurl

$(TARGET): $(MOBJ)
	$(CC) -o $@ $^ $(LIBS)

.c.o:
	$(CC) ${CFLAGS} -o $@ -c $<

clean:
	rm -f $(TARGET) $(MOBJ)
