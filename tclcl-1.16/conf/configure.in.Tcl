dnl autoconf rules to find tcl
dnl $Header: /home/narwhal/u0/soohyunc/CVS_SERV/USN/tclcl-1.16/conf/configure.in.Tcl,v 1.1.1.1 2005/06/16 23:35:06 soohyunc Exp $ (LBL)

AC_ARG_WITH(Tcl,	--with-Tcl=path	specify a pathname for special Tcl lib, d=$withval, d="")


LIBTCL_OLD_VERSION=1.0
LIBTCL_VERSION=1.0b2

if test "$d" != "" ; then 
	if test ! -d $d ; then 
		AC_MSG_ERROR($d is not a directory)
	fi
	V_INCLUDE_BTCL=FAIL
	AC_MSG_CHECKING(for Tcl.h)
        places="$d \
                $d/include"
        for f in $places; do
                if test -r $f/Tcl.h ; then
                        V_INCLUDE_BTCL=-I$f
                        break   
                fi      
        done    
	AC_MSG_RESULT($V_INCLUDE_BTCL)
        if test "$V_INCLUDE_BTCL" = FAIL; then
		AC_MSG_ERROR(cannot find Tcl.h)
        fi

	AC_MSG_CHECKING(for libTcl)
        places="$d \
                $d/lib"  
	V_LIB_BTCL=FAIL
	for dir in $places; do
		if test -r $dir/libTcl.so -o -r $dir/libTcl.a ; then
			V_LIB_BTCL="-L$dir -lTcl"
			break
		fi
	done
	AC_MSG_RESULT($V_LIB_BTCL)
	if test "$V_LIB_BTCL" = FAIL ; then
		AC_MSG_ERROR(cannot find libTcl)
	fi

	AC_MSG_CHECKING(for tcl2c++)
	places="$d \
		"
	V_TCL2CPP=FAIL
	for dir in $places; do
		if test -r $dir/tcl2c++ ; then
			V_TCL2CPP=$dir/tcl2c++
			break
		fi
	done
	AC_MSG_RESULT($V_TCL2CPP)
	if test "$V_TCL2CPP" = FAIL ; then
		AC_MSG_ERROR(cannot find tcl2c++)
	fi
else
	V_INCLUDE_BTCL=FAIL
	AC_MSG_CHECKING(for Tcl.h)
        places="../Tcl \
                /usr/src/local/Tcl \
                ../$LIBTCL_OLD_VERSION \
                ../$LIBTCL_VERSION \
                /import/Tcl/include \
                /usr/src/local/$LIBTCL_OLD_VERSION \
                /usr/src/local/$LIBTCL_VERSION \
                $prefix/include \
                /usr/local/include \
                /usr/contrib/include \
                /usr/include"
        for dir in $places; do
                if test -r $dir/Tcl.h ; then
                        V_INCLUDE_BTCL=-I$dir
                        break
                fi
        done
	if test "$V_INCLUDE_BTCL" = FAIL; then
		AC_TEST_CPP([#include <Tcl.h>], V_INCLUDE_BTCL="", V_INCLUDE_BTCL=FAIL)
	fi
	AC_MSG_RESULT($V_INCLUDE_BTCL)
	if test "$V_INCLUDE_BTCL" = FAIL; then
		AC_MSG_ERROR(cannot find Tcl.h)
	fi

	V_LIB_BTCL=FAIL
	AC_MSG_CHECKING(for libTcl)
        places="../Tcl \
                ../$LIBTCL_OLD_VERSION \
                ../$LIBTCL_VERSION \
                $prefix/lib \
                $x_libraries \
                /usr/contrib/lib \
                /usr/local/lib \
                /usr/lib \
                /usr/src/local/Tcl \
                /usr/src/local/$LIBTCL_OLD_VERSION \
                /usr/src/local/$LIBTCL_VERSION \
		"
        for dir in $places; do
                if test -r $dir/libTcl.so -o -r $dir/libTcl.a; then
                        V_LIB_BTCL="-L$dir -lTcl"
                        break
                fi
        done    
        if test "$V_LIB_BTCL" = FAIL; then
		AC_CHECK_LIB(Tcl, main, V_LIB_BTCL="-lTcl", V_LIB_BTCL="FAIL")
        fi
	AC_MSG_RESULT($V_LIB_BTCL)
        if test "$V_LIB_BTCL" = FAIL; then
		AC_MSG_ERROR(cannot find libTcl)
	fi
	if test $solaris ; then
		V_LIB_BTCL="-R$dir $V_LIB_BTCL"
	fi

	AC_MSG_CHECKING(for tcl2c++)
	V_TCL2CPP=FAIL
        places="\
                ../Tcl \
                ../$LIBTCL_OLD_VERSION \
                ../$LIBTCL_VERSION \
                $prefix/bin \
                $x_libraries \
                /usr/contrib/bin \
                /usr/local/bin \
                /usr/bin \
                /usr/src/local/Tcl \
                /usr/src/local/$LIBTCL_OLD_VERSION \
                /usr/src/local/$LIBTCL_VERSION \
                "       

	for dir in $places; do
		if test -x $dir/tcl2c++; then
			V_TCL2CPP=$dir/tcl2c++
			break
		fi
	done
	AC_MSG_RESULT($V_TCL2CPP)
	if test "$V_TCL2CPP" = FAIL; then
		AC_MSG_ERROR(cannot find tcl2c++)
	fi
fi
AC_SUBST(V_INCLUDE_BTCL)
AC_SUBST(V_LIB_BTCL)
AC_SUBST(V_TCL2CPP)
