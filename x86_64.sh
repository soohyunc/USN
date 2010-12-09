#!/bin/bash
# script for x86_64 systems
# $Id: x86_64.sh 12 2009-11-27 20:50:18Z soohyunc $

SVN=/usr/bin/svn

$SVN switch https://hackerslab.eu/svn/USN/tclcl-1.16/x86_64/config.guess tclcl-1.16/config.guess

$SVN switch https://hackerslab.eu/svn/USN/tclcl-1.16/x86_64/config.sub tclcl-1.16/config.sub

$SVN switch https://hackerslab.eu/svn/USN/otcl-1.9/x86_64/config.guess otcl-1.9/config.guess

$SVN switch https://hackerslab.eu/svn/USN/otcl-1.9/x86_64/config.sub otcl-1.9/config.sub

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/mac/phy.cc ns-2.28/mac/phy.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/mac/mac-802_11.cc ns-2.28/mac/mac-802_11.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/mac/mac-tdma.cc ns-2.28/mac/mac-tdma.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/classifier/classifier-hash.cc ns-2.28/classifier/classifier-hash.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/classifier/classifier-hash.h ns-2.28/classifier/classifier-hash.h

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/webtrace-conv/epa/tr-stat.cc ns-2.28/indep-utils/webtrace-conv/epa/tr-stat.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/webtrace-conv/ucb/logparse.cc ns-2.28/indep-utils/webtrace-conv/ucb/logparse.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/webtrace-conv/ucb/tr-stat.cc ns-2.28/indep-utils/webtrace-conv/ucb/tr-stat.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/webtrace-conv/nlanr/tr-stat.cc ns-2.28/indep-utils/webtrace-conv/nlanr/tr-stat.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/webtrace-conv/dec/tr-stat.cc ns-2.28/indep-utils/webtrace-conv/dec/tr-stat.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/usn/sst-rdr.h ns-2.28/usn/sst-rdr.h

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/webcache/mcache.h ns-2.28/webcache/mcache.h

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/webcache/mcache.cc ns-2.28/webcache/mcache.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/rap/raplist.cc ns-2.28/rap/raplist.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/pushback/ident-tree.h ns-2.28/pushback/ident-tree.h

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/cmu-scen-gen/setdest/calcdest.cc ns-2.28/indep-utils/cmu-scen-gen/setdest/calcdest.cc

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/cmu-scen-gen/setdest/setdest.h ns-2.28/indep-utils/cmu-scen-gen/setdest/setdest.h

$SVN switch https://hackerslab.eu/svn/USN/ns-2.28/x86_64/indep-utils/cmu-scen-gen/setdest/setdest.cc ns-2.28/indep-utils/cmu-scen-gen/setdest/setdest.cc
