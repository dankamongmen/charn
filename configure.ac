AC_PREREQ([2.61])
AC_INIT([charn], [0.0.1-pre], [sprezzos-dev@sprezzatech.com], [charn], [https://www.sprezzatech.com/wiki/index.php/charn])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h:config.in])
# FIXME restore -Werror!
AM_INIT_AUTOMAKE([-Wall foreign dist-xz std-options subdir-objects nostdinc color-tests])
AC_CONFIG_SRCDIR([src/bin/charn.c])

## compiler check
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99
AC_C_INLINE
AC_C_RESTRICT

AM_PROG_AR
AM_PROG_CC_C_O

AX_PTHREAD

# Require autoconf-gl-macros: http://code.google.com/p/autoconf-gl-macros/
AX_CHECK_GL
LIBS+=${GL_LIBS}
AX_CHECK_GLUT
LIBS+=${GLUT_LIBS}

AC_CHECK_LIB([m],[sqrt])

PKG_CHECK_MODULES(X11, x11, [have_x11=yes])
	CFLAGS+=" $X11_CFLAGS"
	LIBS+=" $X11_LIBS"

PKG_CHECK_MODULES(X11XCB, x11-xcb, [have_x11_xcb=yes])
	CFLAGS+=" $X11XCB_CFLAGS"
	LIBS+=" $X11XCB_LIBS"

PKG_CHECK_MODULES(XI, xi, [have_xi=yes])
	CFLAGS+=" $XI_CFLAGS"
	LIBS+=" $XI_LIBS"

PKG_CHECK_MODULES(DBUS, dbus-1, [have_dbus=yes])
	CFLAGS+=" $DBUS_CFLAGS"
	LIBS+=" $DBUS_LIBS"

PKG_CHECK_MODULES(FREETYPE, freetype2, [have_freetype2=yes])
	CFLAGS+=" $FREETYPE_CFLAGS"
	LIBS+=" $FREETYPE_LIBS"

PKG_CHECK_MODULES(FTGL, ftgl, [have_ftgl=yes])
	CFLAGS+=" $FTGL_CFLAGS"
	LIBS+=" $FTGL_LIBS"

PKG_CHECK_MODULES(GLEW, glew, [have_glew=yes])
	CFLAGS+=" $GLEW_CFLAGS"
	LIBS+=" $GLEW_LIBS"

PKG_CHECK_MODULES(XCBAUX, xcb-aux, [have_xcb_aux=yes])
	CFLAGS+=" $XCBAUX_CFLAGS"
	LIBS+=" $XCBAUX_LIBS"

PKG_CHECK_MODULES(XCBKEYSYMS, xcb-keysyms, [have_xcb_keysyms=yes])
	CFLAGS+=" $XCBKEYSYMS_CFLAGS"
	LIBS+=" $XCBKEYSYMS_LIBS"

PKG_CHECK_MODULES(XCBRANDR, xcb-randr, [have_xcb_randr=yes])
	CFLAGS+=" $XCBRANDR_CFLAGS"
	LIBS+=" $XCBRANDR_LIBS"

PKG_CHECK_MODULES(XDGBASEDIR, libxdg-basedir, [have_xdg_basedir=yes])
	CFLAGS+=" $XDGBASEDIR_CFLAGS"
	LIBS+=" $XDGBASEDIR_LIBS"

AC_SEARCH_LIBS([SOIL_load_OGL_texture],[SOIL],[],AC_MSG_ERROR([Cannot find libsoil.]))
LIBS+=" $ac_cv_search_SOIL_load_OGL_texture"

AX_PROG_XSLTPROC
if test "x$XSLTPROC" = "x" ; then
	AC_MSG_ERROR([Cannot find xsltproc.])
fi
AX_CHECK_DOCBOOK_XSLT

AC_PROG_LIBTOOL

SRCDIR=${srcdir}

## output
AC_SUBST([CFLAGS])
AC_SUBST([SRCDIR])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
