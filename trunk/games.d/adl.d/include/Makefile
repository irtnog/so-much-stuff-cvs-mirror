include $(HOME)/CONFIG
# AR = shar -c -v
AR = post -c -v -p 6 17
B = ..
D = include/

shar		: include.shar

include.shar	: README Makefile *.h
	( cd ${B} ; ${AR}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	${D}*.h \
	> ${D}include.shar )

clean		:
	rm -f core include.shar

install:
