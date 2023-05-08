all: adtar

clean:
	rm -f adtar

adtar: adtar.c
	gcc adtar.c -o adtar
