DEST=/usr/local/bin

all: timeadj timeadjx

timeadj: timeadj.c
	gcc -DADJTIMEX=\"$(DEST)/timeadjx\" timeadj.c -o timeadj -march=native `pkg-config --cflags --libs gtk+-3.0`

timeadjx: timeadjx.c
	gcc timeadjx.c -o timeadjx

install: timeadj timeadjx
	cp timeadj $(DEST)
	cp timeadjx $(DEST)
	chown root $(DEST)/timeadjx
	chmod u+s $(DEST)/timeadjx

clean:
	rm timeadj timeadjx *~
