# Flags for CC
MODEL	= -M2l
OPTIONS	= -I${INC} -O '-DADL_NAME="${BIN}"'
ALL		= adlcomp$(EXE)
include $(HOME)/CONFIG

# Source directories - INC is the include directory; UTL is the
# utilities directory.
INC = ../include
UTL = ../util

# shar options.  B is the directory from which to unpack; D is the
# name of the directory we're unpacking.
AR1 = post -c -v -p 2 17
AR2 = post -c -v -p 3 17
B = ..
D = adlcomp/

# Where adlcomp will be installed, with make install.
#BIN = /users/cunniff/bin
BIN		= ${GAMLIB}

# Flags for lint
LFLAGS = -DLINT -I${INC} -C

# Header files used by adlcomp and associated modules
HDRS =	${INC}/adltypes.h	${INC}/adldef.h		${INC}/adlprog.h \
	${INC}/adlcomp.h	${INC}/virtmem.h	${INC}/vstring.h

# Utility files used by adlcomp
UTIL =	${UTL}/virtmem.o ${UTL}/vstring.o ${UTL}/mytime.o

# The object modules from which to buld adlcomp
OBJS =	adlcomp.o codegen.o adllex.o adlmisc.o \
	adlmsg.o adlobj.o routcomp.o predef.o \
	compdict.o adlglob.o

# The rules.

shar		: adlcomp1.shar	adlcomp2.shar

adlcomp1.shar	: README Makefile adlcomp.c adlglob.c adllex.c adlmisc.c
	( cd ${B} ; ${AR1}					\
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}adlcomp.c	${D}adlglob.c	${D}adllex.c	\
		${D}adlmisc.c					\
	> ${D}adlcomp1.shar )

adlcomp2.shar	: adlmsg.c	adlobj.c	codegen.c	\
		  compdict.c	predef.c	routcomp.c
	( cd ${B} ; ${AR2}					\
		${D}adlmsg.c	${D}adlobj.c	${D}codegen.c	\
		${D}compdict.c	${D}predef.c	${D}routcomp.c	\
	> ${D}adlcomp2.shar )

install		: ${ALL} ${BIN}
	strip adlcomp$(EXE)
	cp adlcomp$(EXE) ${BIN}
	chmod 755 ${BIN}/adlcomp$(EXE)

clean		:
	rm -f ${ALL} ${OBJS} core adlcomp1.shar adlcomp2.shar adlcomp

lint		:
	lint ${LFLAGS} adlcomp.c codegen.c predef.c \
		adllex.c adlmisc.c adlmsg.c adlobj.c \
		routcomp.c compdict.c adlglob.c ${UTL}/*.c

adlcomp$(EXE)	: ${OBJS} ${UTIL}
	$(CC) $(CFLAGS) -o $@ ${OBJS} ${UTIL}

codegen.o	: codegen.c	${HDRS}

adlcomp.o	: adlcomp.c	${HDRS}

adllex.o	: adllex.c	${HDRS}

adlmisc.o	: adlmisc.c	${HDRS}

adlmsg.o	: adlmsg.c	${HDRS}

adlobj.o	: adlobj.c	${HDRS}

predef.o	: predef.c	${HDRS}

routcomp.o	: routcomp.c	${HDRS}

compdict.o	: compdict.c	${HDRS}

adlglob.o	: adlglob.c	${HDRS}
