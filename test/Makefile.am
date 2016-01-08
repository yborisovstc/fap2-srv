bin_PROGRAMS = ut-fap2srv-lib

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

ut_fap2srv_lib_SOURCES = \
		       ut_main.cpp \
		       ut_base.cpp

ut_fap2srv_lib_LDFLAGS=-L../client/.libs/ -lfap2client -L../dmas/.libs -lfap2da -lcppunit -lrt
ut_fap2srv_lib_CXXFLAGS=-I@srcdir@/../dmas

check: test_fap2srv_lib

test_fap2srv_lib:
	 ./ut-fap2srv-lib


