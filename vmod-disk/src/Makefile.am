AM_CFLAGS = $(VARNISHAPI_CFLAGS) -Wall -Werror
AM_LDFLAGS = $(VARNISHAPI_LIBS) $(VMOD_LDFLAGS)

vmod_LTLIBRARIES = \
  libvmod_disk.la  \
  libvmod_static.la \
  libvmod_thres.la \
  libvmod_prob.la

libvmod_disk_la_SOURCES = \
	vmod_disk.c

libvmod_static_la_SOURCES = \
	vmod_static.c

libvmod_thres_la_SOURCES = \
	vmod_thres.c

libvmod_prob_la_SOURCES = \
	vmod_prob.c

nodist_libvmod_disk_la_SOURCES = \
	vcc_disk_if.c \
	vcc_disk_if.h

nodist_libvmod_static_la_SOURCES = \
	vcc_static_if.c \
	vcc_static_if.h

nodist_libvmod_thres_la_SOURCES = \
	vcc_thres_if.c \
	vcc_thres_if.h

nodist_libvmod_prob_la_SOURCES = \
	vcc_prob_if.c \
	vcc_prob_if.h

dist_man_MANS = \
	vmod_disk.3   \
	vmod_static.3 \
	vmod_thres.3  \
	vmod_prob.3

@BUILD_VMOD_DISK@
@BUILD_VMOD_STATIC@
@BUILD_VMOD_THRES@
@BUILD_VMOD_PROB@

AM_TESTS_ENVIRONMENT = \
	PATH="$(VMOD_TEST_PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -v

AM_VTC_LOG_FLAGS = -Dvmod_disk="$(VMOD_DISK)"

TESTS = @VMOD_TESTS@

EXTRA_DIST = \
	vtree.h \
	vmod_disk.vcc \
	vmod_static.vcc \
	vmod_thres.vcc  \
	vmod_prob.vcc  \
	$(VMOD_TESTS)
