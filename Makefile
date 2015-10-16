TARGET=mygif

CFLAGS+=-Wall -Werror


all:$(TARGET)

clean:
	$(RM) $(TARGET)
