OBJS	= nmea.o main.o
CC	= gcc
CFLAGS	= -Wl,--no-as-needed -fPIC -Wall -g
LDFLAGS	= -s
LDLIBS	= 

nmea:	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(OBJS) -o nmea

main.o:	main.c
	$(CC) -c $< -o $@

nmea.o:	nmea.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o
	rm -f nmea
