bin_PROGRAMS = rcr
rcr_CFLAGS = $(MICROHTTPD_CFLAGS) $(LIBCJSON_CFLAGS)
rcr_LDADD = $(MICROHTTPD_LIBS) $(LIBCJSON_LIBS)
rcr_SOURCES = main.c service.c config.c helper.c screen_control.c

AM_CPPFLAGS = -DDATADIR='"$(datadir)/rcr"'