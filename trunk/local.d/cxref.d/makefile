#
# $Header: /home/Vince/cvs/local.d/cxref.d/makefile,v 1.5 2002-11-24 05:05:43 Vincent Exp $
# makefile for cxref -- C cross referencing program
#
# Arnold Robbins, Information and Computer Science, Georgia Tech
#	gatech!arnold
# Copyright (c) 1984 by Arnold Robbins.
# All rights reserved.
# This program may not be sold, but may be distributed
# provided this header is included.
#
ALL	= cxref$(EXE) docxref$(EXE) fmtxref$(EXE) cxrfilt$(EXE)
MODEL	= -i
OPTIONS	= -O -DSRCDIR='"$(LIBDIR)"'
include $(HOME)/CONFIG

SCANOBJS = docxref.o cscan.o
SCANSRCS = docxref.c cscan.l
INCLS	= constdefs.h basename.c
SRCS	= $(SCANSRCS) cxref.c fmtxref.c cxrfilt.c
DOCS	= README makefile cxref.1
PRINTS	= $(INCLS) $(SRCS) $(DOCS)

docxref$(EXE): $(SCANOBJS)
	$(CC) $(SCANOBJS) -o $@

cscan.o docxref.o cxrfilt.o: $(INCLS)

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp cxref $(BINDIR)
	cd $(BINDIR); chmod 711 cxref
	cp cxrfilt docxref fmtxref $(LIBDIR)
	cd $(LIBDIR); chmod 711 cxrfilt docxref fmtxref
	cp cxref.1 $(MAN1)
	cd $(MAN1); chmod 644 cxref.1

clean:
	rm -f $(SCANOBJS) cxrfilt.o $(ALL) core make.out
