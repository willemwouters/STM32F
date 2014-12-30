#! /usr/bin/env bash
# Copyright (c) 2011-2014, ARM Limited
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of ARM nor the names of its contributors may be used
#       to endorse or promote products derived from this software without
#       specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

set -e
#set -x
set -u
set -o pipefail

umask 022

exec < /dev/null

script_path=`cd $(dirname $0) && pwd -P`
. $script_path/build-common.sh

# This file contains the sequence of commands used to build the ARM EABI toolchain.
usage ()
{
cat<<EOF
Usage: $0 [--build_type=...] [--build_tools=...] [--skip_steps=...]

This script will build gcc arm embedded toolchain.

OPTIONS:
  --build_type=TYPE     specify build type to either ppa or native.
                        If followed by keyword debug, the produced binaries
                        will be debuggable.  The default case will be
                        non-debug native build.

                        Example usages are as:
                        --build_type=native
                        --build_type=ppa
                        --build_type=native,debug
                        --build_type=ppa,debug

  --build_tools=TOOLS   specify where to find the native build tools that
                        will be used for building gcc arm embedded toolchain
                        and related dependent libraries.  If not specified,
                        the ones in your system will be used.

                        The prebuilt ones provided by arm embedded toolchain
                        team are supposed to run on 32bit build platform, thus
                        not suitable for 64bit platform.

  --skip_steps=STEPS    specify which build steps you want to skip.  Concatenate
                        them with comma for skipping more than one steps.  Available
                        steps are: gdb-with-python, mingw32, mingw32-gdb-with-python
                        and manual.
EOF
}

if [ $# -gt 3 ] ; then
    usage
fi

skip_mingw32=yes
DEBUG_BUILD_OPTIONS=
is_ppa_release=no
is_native_build=yes

skip_manual=yes
skip_steps=
skip_gdb_with_python=no
skip_mingw32_gdb_with_python=no
build_type=
build_tools=

MULTILIB_LIST="--with-multilib-list=armv6-m,armv7-m,armv7e-m,armv7-r"

for ac_arg; do
    case $ac_arg in
      --skip_steps=*)
	      skip_steps=`echo $ac_arg | sed -e "s/--skip_steps=//g" -e "s/,/ /g"`
	      ;;
      --build_type=*)
	      build_type=`echo $ac_arg | sed -e "s/--build_type=//g" -e "s/,/ /g"`
	      ;;
      --build_tools=*)
	      build_tools=`echo $ac_arg | sed -e "s/--build_tools=//g"`
	      build_tools_abs_path=`cd $build_tools && pwd -P`
	      if [ -d $build_tools_abs_path ]; then
	        if [ -d $build_tools_abs_path/gcc ]; then
	          export PATH=$build_tools_abs_path/gcc/bin:$PATH
	        fi
		if [ -d $build_tools_abs_path/mingw-w64-gcc ]; then
		  export PATH=$build_tools_abs_path/mingw-w64-gcc/bin:$PATH
		fi
		if [ -d $build_tools_abs_path/installjammer ]; then
		  export PATH=$build_tools_abs_path/installjammer:$PATH
		fi
		if [ -d $build_tools_abs_path/python ]; then
		  export PATH=$build_tools_abs_path/python/bin:$PATH
		  export LD_LIBRARY_PATH="$build_tools_abs_path/python/lib"
		  export PYTHONHOME="$build_tools_abs_path/python"
		fi
	      else
	        echo "The specified folder of build tools don't exist: $build_tools" 1>&2
		exit 1
	      fi
	      ;;
      *)
        usage
        exit 1
        ;;
    esac
done



if [ "x$is_ppa_release" != "xyes" ]; then
  ENV_CFLAGS=" -I$BUILDDIR_NATIVE/host-libs/zlib/include -O2 "
  ENV_CPPFLAGS=" -I$BUILDDIR_NATIVE/host-libs/zlib/include "
  ENV_LDFLAGS=" -L$BUILDDIR_NATIVE/host-libs/zlib/lib
                -L$BUILDDIR_NATIVE/host-libs/usr/lib "


  NEWLIB_CONFIG_OPTS=" --build=$BUILD --host=$HOST_NATIVE"

  
fi

mkdir -p $BUILDDIR_NATIVE
mkdir -p $INSTALLDIR_NATIVE

cd $SRCDIR

echo Task [III-2] /$HOST_NATIVE/newlib/
saveenv
prepend_path PATH $INSTALLDIR_NATIVE/bin
saveenvvar CFLAGS_FOR_TARGET '-g -O2 -ffunction-sections -fdata-sections -fshort-wchar'
mkdir -p $BUILDDIR_NATIVE/newlib
pushd $BUILDDIR_NATIVE/newlib

 $SRCDIR/$NEWLIB/configure  \
    $NEWLIB_CONFIG_OPTS \
    --target=$TARGET \
    --prefix=$INSTALLDIR_NATIVE \
    --infodir=$INSTALLDIR_NATIVE_DOC/info \
    --mandir=$INSTALLDIR_NATIVE_DOC/man \
    --htmldir=$INSTALLDIR_NATIVE_DOC/html \
    --pdfdir=$INSTALLDIR_NATIVE_DOC/pdf \
    --enable-newlib-io-long-long \
    --enable-newlib-register-fini \
    --disable-newlib-supplied-syscalls \
    --disable-nls

 make -j$JOBS 

 make install


popd
restoreenv


