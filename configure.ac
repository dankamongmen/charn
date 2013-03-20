AC_PREREQ([2.61])
AC_INIT([charn], [0.0.1-pre], [sprezzos-dev@sprezzatech.com], [charn], [https://www.sprezzatech.com/wiki/index.php/charn])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h:config.in])
AM_INIT_AUTOMAKE([-Werror -Wall foreign dist-xz std-options subdir-objects nostdinc color-tests])
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

AX_CHECK_GL
LIBS+=${GL_LIBS}
AX_CHECK_GLUT
LIBS+=${GLUT_LIBS}

PKG_CHECK_MODULES(GLEW, glew, [have_glew=yes])
	CFLAGS+=" $GLEW_CFLAGS"
	LIBS+=" $GLEW_LIBS"

PKG_CHECK_MODULES(XCBRANDR, xcb-randr, [have_xcb_randr=yes])
	CFLAGS+=" $XCBRANDR_CFLAGS"
	LIBS+=" $XCBRANDR_LIBS"

AX_PROG_XSLTPROC
if test "x$XSLTPROC" = "x" ; then
	AC_MSG_ERROR([Cannot find xsltproc.])
fi
AX_CHECK_DOCBOOK_XSLT

AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR([m4])

SRCDIR=${srcdir}

## output
AC_SUBST([CFLAGS])
AC_SUBST([SRCDIR])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
