# 
# $Header: /home/Vince/cvs/local.d/se.d/libchangetty/Makefile,v 1.6 2002-11-25 21:42:59 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Revision 1.5  2002/11/23 19:01:16  Vincent
# Move CONFIG to $(HOME).
#
# Revision 1.4  1990/03/30 17:03:28  vrs
# Move CONFIG to /etc
#
# Version 1.3  87/02/13  10:07:49  vrs
# Use CONFIG
# 
# Version 1.2  87/02/07  20:27:48  vrs
# Look for ranlib in both /bin and /usr/bin
# 
# Revision 1.2  86/07/11  15:23:24  osadr
# Changed to ensure the proper 'ar' program is used.
# 
# Revision 1.1  86/05/06  13:31:30  osadr
# Initial revision
# 
# 
# 
# makefile for libchangetty.a -- terminal resetting functions
ALL	= libchangetty.a
MODEL	= -M2l
OPTIONS	= -O `cat ../flags`
include $(HOME)/CONFIG
SRCS	= changetty.c
OBJS	= changetty.o
PRINTS	= $(SRCS) Makefile


$(ALL): $(OBJS) ../flags
	ar ruv $(ALL) $(OBJS)
	if test -x /usr/bin/ranlib; then /usr/bin/ranlib $(ALL); fi
	if test -x /bin/ranlib; then /bin/ranlib $(ALL); fi

install: ;

clean:
	rm -fr $(OBJS) $(ALL)

print:
	prt $(PRINTS) | lpr -b 'tty lib'

print2: $(PRINTS)
	prt $? | lpr -b 'new tty lib'
