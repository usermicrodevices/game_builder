prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
INSTALL = /usr/bin/install -c
NM = nm
srcdir = src
top_srcdir = wxWidgets
wx_top_builddir = wxWidgets
BK_DEPS = wxWidgets/bk-deps
LIBS = -lz -lcurl -lm
LDFLAGS_GUI = 
CXX = g++
CXXFLAGS = -ggdb -std=c++20
CPPFLAGS = 
LDFLAGS = 
USE_DPI_AWARE_MANIFEST = 2
WX_LIB_FLAVOUR =
TOOLKIT = GTK
TOOLKIT_LOWERCASE = gtk
TOOLKIT_VERSION = 3
TOOLCHAIN_FULLNAME = gtk3-unicode-3.3
EXTRALIBS = -pthread -Wl,--version-script,$(wx_top_builddir)/version-script
# -lpcre2-32
EXTRALIBS_XML = -lexpat
EXTRALIBS_HTML = 
EXTRALIBS_GUI = -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lX11 -lgthread-2.0 -lglib-2.0 -lXxf86vm -lSM -lxkbcommon -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lXtst -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lfontconfig -lfreetype -lpng -ljpeg -ltiff -ljbig
WX_CPPFLAGS = -I${wx_top_builddir}/lib/wx/include/gtk3-unicode-3.3 -I${top_srcdir}/include -D_FILE_OFFSET_BITS=64 -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gio-unix-2.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/fribidi -I/usr/include/atk-1.0 -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/x86_64-linux-gnu -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

PLUGINS_PYTHON = 1
PY_ROOT_DIR = Python-3.13.0

ifeq ($(PLUGINS_PYTHON), 1)
WX_CPPFLAGS += -DPLUGINS_PYTHON -I$(PY_ROOT_DIR) -I$(PY_ROOT_DIR)/Include
#LDFLAGS += -L$(PY_ROOT_DIR)/build/lib.linux-x86_64-3.13 -llibpython3.13 -ldl -lm
LDFLAGS += -L$(PY_ROOT_DIR) -lpython3.13 -ldl -Wl,-rpath,$(PY_ROOT_DIR)
endif

HOST_SUFFIX = 
RPATH_FLAG = -Wl,-rpath,$(wx_top_builddir)/lib
#PIC_CXXFLAGS = -fPIC -DPIC

### Variables: ###

WX_RELEASE = 3.3
WX_VERSION = $(WX_RELEASE).0
LIBDIRNAME = $(wx_top_builddir)/lib
EDITOR_CXXFLAGS = $(WX_CPPFLAGS) -D__WX$(TOOLKIT)__ $(__WXUNIV_DEFINE_p) \
	$(__DEBUG_DEFINE_p) $(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) \
	$(__THREAD_DEFINE_p) -I$(srcdir) $(__DLLFLAG_p) \
	$(CPPFLAGS) $(CXXFLAGS)# $(PIC_CXXFLAGS)
EDITOR_OBJECTS = build/gb_gb.o

### Conditionally set variables: ###

CXXC = $(BK_DEPS) $(CXX)
PORTNAME = $(TOOLKIT_LOWERCASE)$(TOOLKIT_VERSION)
EXTRALIBS_FOR_BASE = $(EXTRALIBS)
EXTRALIBS_FOR_GUI = $(EXTRALIBS_GUI)
__DLLFLAG_p = -DWXUSINGDLL
__DLLFLAG_p_1 = --define WXUSINGDLL
COND_PLATFORM_OS2_1___gb___os2_emxbindcmd = $(NM) gb | if \
	grep -q pmwin.763 ; then emxbind -ep gb ; fi
COND_MONOLITHIC_0___WXLIB_PROPGRID_p = \
	-lwx_$(PORTNAME)$(WXUNIVNAME)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_propgrid-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_PROPGRID_p = $(COND_MONOLITHIC_0___WXLIB_PROPGRID_p)
COND_MONOLITHIC_0___WXLIB_AUI_p = \
	-lwx_$(PORTNAME)$(WXUNIVNAME)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_aui-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_AUI_p = $(COND_MONOLITHIC_0___WXLIB_AUI_p)
COND_MONOLITHIC_0___WXLIB_HTML_p = \
	-lwx_$(PORTNAME)$(WXUNIVNAME)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_html-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_HTML_p = $(COND_MONOLITHIC_0___WXLIB_HTML_p)
COND_MONOLITHIC_0___WXLIB_CORE_p = \
	-lwx_$(PORTNAME)$(WXUNIVNAME)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_core-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_CORE_p = $(COND_MONOLITHIC_0___WXLIB_CORE_p)
COND_MONOLITHIC_0___WXLIB_XML_p = \
	-lwx_base$(WXBASEPORT)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_xml-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_XML_p = $(COND_MONOLITHIC_0___WXLIB_XML_p)
COND_MONOLITHIC_0___WXLIB_BASE_p = \
	-lwx_base$(WXBASEPORT)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)-$(WX_RELEASE)$(HOST_SUFFIX)
__WXLIB_BASE_p = $(COND_MONOLITHIC_0___WXLIB_BASE_p)
COND_MONOLITHIC_1___WXLIB_MONO_p = \
	-lwx_$(PORTNAME)$(WXUNIVNAME)u$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)-$(WX_RELEASE)$(HOST_SUFFIX)
__LIB_SCINTILLA_p = \
	-lwxscintilla$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)-$(WX_RELEASE)$(HOST_SUFFIX)
__LIB_LEXILLA_p = \
	-lwxlexilla$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)-$(WX_RELEASE)$(HOST_SUFFIX)

### Targets: ###

all: clean bin/gb $(__gb_bundle___depname)

install: 

uninstall: 

install-strip: install

clean: 
	rm -f build/*.o
	rm -f bin/gb

distclean: clean
	rm -f config.cache config.log config.status bk-deps bk-make-pch

bin/gb: $(EDITOR_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(EDITOR_OBJECTS) -L$(LIBDIRNAME) $(LDFLAGS_GUI) $(LDFLAGS) $(__WXLIB_PROPGRID_p) $(__WXLIB_AUI_p) $(__WXLIB_HTML_p) $(EXTRALIBS_HTML) $(__WXLIB_CORE_p) $(__WXLIB_XML_p) $(EXTRALIBS_XML) $(__WXLIB_BASE_p) $(__WXLIB_MONO_p) $(__LIB_TIFF_p) $(__LIB_JPEG_p) $(__LIB_PNG_p) $(EXTRALIBS_FOR_GUI) $(__LIB_ZLIB_p) $(__LIB_REGEX_p) $(__LIB_EXPAT_p) $(EXTRALIBS_FOR_BASE) $(LIBS) $(RPATH_FLAG) $(__gb___os2_emxbindcmd)

build/gb_rc.o: $(srcdir)/gb.rc
	@mkdir -p $(@D)
	-i$< -o $@ --define __WX$(TOOLKIT)__ -DwxDEBUG_LEVEL=1 --include-dir $(__DLLFLAG_p_1)

build/gb_gb.o: $(srcdir)/gb.cpp
	@mkdir -p $(@D)
	$(CXXC) -c -o $@ $(EDITOR_CXXFLAGS) $(srcdir)/gb.cpp


# Include dependency info, if present:
-include ./.deps/*.d

.PHONY: all install uninstall clean distclean gb_bundle
