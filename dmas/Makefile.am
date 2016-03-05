AM_CPPFLAGS = -D_PLAT_LINUX_ -Wall -g -O2

lib_LTLIBRARIES = libfap2da.la

libfap2da_la_SOURCES = \
                       mproxy.h \
                       renv.h \
                       renv.cpp \
                       daaprov.h \
                       daaprov.cpp \
                       daaproxy.h \
                       daaproxy.cpp \
                       melempx.h \
                       melempx.cpp \
                       mvertpx.h \
                       mvertpx.cpp \
                       elempx.h \
                       elempx.cpp

libfap2da_la_LDFLAGS = -rdynamic -no-undefined
libfap2da_la_CXXFLAGS=-I@srcdir@/../client
