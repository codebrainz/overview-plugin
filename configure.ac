AC_INIT([overview], [1.0], [https://github.com/codebrainz/overview-plugin/issues])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11.2 -Wall foreign])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT([disable-static])

AC_PROG_CC_C99

PKG_CHECK_MODULES([GEANY], [geany])
AC_ARG_WITH([geany-libdir],
            [AS_HELP_STRING([--with-geany-libdir=DIR],
                            [Install the plugin library in DIR])],
            [],
            [geany_libdir=`$PKG_CONFIG --variable=libdir geany`
             with_geany_libdir="$geany_libdir/geany"])
AC_SUBST([GEANY_PLUGINDIR], [$with_geany_libdir])

AC_PATH_PROG([MARKDOWN], [markdown])
AM_CONDITIONAL([WITH_MARKDOWN], [test "x$MARKDOWN" != "x"])

AC_CONFIG_FILES([
 Makefile
 data/Makefile
 screenshots/Makefile
 overview/Makefile
])
AC_OUTPUT
