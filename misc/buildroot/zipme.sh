#!/bin/sh
#set -x

WD=`pwd`
VERSION=$1

TAR="tar cvf"
ZIP=gzip

# Make sure we know what is going on

if [ -z ${VERSION} ] ; then
   echo "You must supply a version like xx.yy.zz as a parameter"
   exit 1;
fi

# Find the directory we were executed from and where we expect to
# see the directory to tar up

MYNAME=`basename $0`

if [ -x ${WD}/${MYNAME} ] ; then
   MISCDIR=`dirname ${WD}`
else
   if [ -x ${WD}/buildroot/${MYNAME} ] ; then
     MISCDIR=${WD}
   else
     echo "You must cd into the misc/ or misc/buildroot/ directory to execute this script."
     exit 1
   fi
fi

# Get the path to the parent directory

SUBDIR=`basename ${MISCDIR}`/buildroot
PARENT=`dirname ${MISCDIR}`

# The name of the directory must match the version number

cd ${PARENT} || \
   { echo "Failed to cd to ${PARENT}" ; exit 1 ; }

if [ ! -d ${SUBDIR} ] ; then
   echo "${PARENT}/${SUBDIR} does not exist!"
   exit 1
fi

TAR_NAME=buildroot-${VERSION}.tar
ZIP_NAME=${TAR_NAME}.gz

# Prepare the buildroot directory -- Remove editor garbage

find ${SUBDIR} -name '*~' -exec rm -f '{}' ';' || \
      { echo "Removal of emacs garbage failed!" ; exit 1 ; }
find ${SUBDIR} -name '*.swp' -exec rm -f '{}' ';' || \
      { echo "Removal of VI garbage failed!" ; exit 1 ; }

# Remove any previous tarballs

if [ -f ${TAR_NAME} ] ; then
   echo "Removing ${PARENT}/${TAR_NAME}"
   rm -f ${TAR_NAME} || \
      { echo "rm ${TAR_NAME} failed!" ; exit 1 ; }
fi

if [ -f ${ZIP_NAME} ] ; then
   echo "Removing ${PARENT}/${ZIP_NAME}"
   rm -f ${ZIP_NAME} || \
      { echo "rm ${ZIP_NAME} failed!" ; exit 1 ; }
fi

# Then zip it

${TAR} ${TAR_NAME} ${SUBDIR} || \
      { echo "tar of ${TAR_NAME} failed!" ; exit 1 ; }
${ZIP} ${TAR_NAME} || \
      { echo "zip of ${TAR_NAME} failed!" ; exit 1 ; }

cd ${WD}