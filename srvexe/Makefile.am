AM_CPPFLAGS = -D_PLAT_LINUX_ -Wall -g -O2

bin_PROGRAMS = fap2-srv

fap2_srv_SOURCES = \
		       server_main.cpp

fap2_srv_LDFLAGS= -L../server/.libs/  -lfap2srv -L../client/.libs/ -lfap2client -L../dmas/.libs -lfap2da -lrt -Wl,--no-as-needed
#fap2_srv_LDFLAGS= -lrt -Wl,--no-as-needed
