# echo "Setting tests Close_to_Hell in /afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online/GaudiOnline"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=tests -version=Close_to_Hell -path=/afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online/GaudiOnline  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

