#
#	$Header: /home/Vince/cvs/local.d/displays.d/makefile,v 1.7 2002-11-26 07:35:53 vrs Exp $
#
ALL	= displays$(EXE)
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG
SRCS	= keyboard.c main.c pdisplay.c pipe.c pty.c vdisplay.c
OBJS	= $(SRCS:.c=.o)

$(ALL): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f lint $(ALL) $(OBJS)

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown $(SAFEUID) $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)

lint:	$(SRCS)
	lint $(OPTIONS) $(LIBS) $(SRCS) | tee $@

$(OBJS): globals.h
