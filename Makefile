all:
	gcc mqttsleep.c -o mqttsleep -O3 -lmosquitto
	gcc mqttwake.c -o mqttwake -O3 -lmosquitto
install:
	cp mqttsleep /bin/mqttsleep
	cp mqttwake /bin/mqttwake
	chmod 4755 /bin/mqttsleep
	chmod 4755 /bin/mqttwake
clean:
	rm mqttsleep
	rm mqttwake

