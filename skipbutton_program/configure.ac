AC_INIT([skipbutton], 1.0)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PROG_CXX

AC_CHECK_LIB([curl], [main])
AC_CHECK_HEADERS([curl/curl.h],,AC_MSG_ERROR(["Missing a required header"]))

AC_OUTPUT([Makefile src/Makefile])
