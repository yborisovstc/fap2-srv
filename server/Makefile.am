AM_CPPFLAGS = -D_PLAT_LINUX_ -Wall -g -O2

#bin_PROGRAMS = sample-srv grayb-srv
bin_PROGRAMS = fap2-srv

#sample_srv_SOURCES = \
#		       server_example.c

fap2_srv_SOURCES = \
		       requests.h \
		       requests.cpp \
		       reqbase.h \
		       reqbase.cpp \
		       rgetmodelslist.h \
		       rgetmodelslist.cpp \
		       rcreateenv.h \
		       rcreateenv.cpp \
		       rexec.h \
		       rexec.cpp \
		       server_main.cpp \
		       sclient.h \
		       sclient.cpp \
		       sthread.h \
		       sthread.cpp \
		       sserver.h \
		       sserver.cpp

fap2_srv_LDFLAGS=  -L../client/.libs/ -lfap2client -L../dmas/.libs -lfap2da -lrt -Wl,--no-as-needed
#fap2_srv_LDFLAGS= -lrt -Wl,--no-as-needed
