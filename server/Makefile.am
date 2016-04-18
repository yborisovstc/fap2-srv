AM_CPPFLAGS = -D_PLAT_LINUX_ -Wall -g -O2

#bin_PROGRAMS = libfap2srv.la
lib_LTLIBRARIES = libfap2srv.la

libfap2srv_la_SOURCES = \
		       requests.h \
		       requests.cpp \
		       fapcsbase.h \
		       fapcsbase.cpp \
		       sclient.h \
		       sclient.cpp \
		       sthread.h \
		       sthread.cpp \
		       sserver.h \
		       sserver.cpp

include_HEADERS = \
		  requests.h \
		  fapcsbase.h

libfap2srv_la_LDFLAGS=  -L../client/.libs/ -lfap2client -L../dmas/.libs -lfap2da -lrt -Wl,--no-as-needed
#fap2_srv_LDFLAGS= -lrt -Wl,--no-as-needed
