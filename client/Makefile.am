AM_CPPFLAGS = -D_PLAT_LINUX_ -Wall -g -O2

lib_LTLIBRARIES = libfap2client.la
#bin_PROGRAMS = fap2-client

libfap2client_la_SOURCES = \
                       bclient.h \
                       bclient.cpp \
		       ../server/requests.h \
		       ../server/requests.cpp

include_HEADERS = \
		  bclient.h

libfap2client_la_LDFLAGS = -rdynamic -no-undefined
