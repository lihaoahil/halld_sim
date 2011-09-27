#!/bin/tcsh
 
# This file was generated by the script "mk_setenv.csh"
#
# Generation date: Tue Sep 27 12:04:09 EDT 2011
# User: gluex
# Host: ifarm1102
# Platform: Linux ifarm1102 2.6.18-128.7.1.el5 #1 SMP Mon Aug 24 08:21:56 EDT 2009 x86_64 x86_64 x86_64 GNU/Linux
# BMS_OSNAME: Linux_CentOS5-x86_64-gcc4.1.2
 
# Make sure LD_LIBRARY_PATH is set
if ( ! $?LD_LIBRARY_PATH ) then
   setenv LD_LIBRARY_PATH
endif
 
# HALLD
setenv HALLD_HOME /group/halld/Software/builds/sim-recon/sim-recon-2011-09-23
setenv HDDS_HOME /group/halld/Software/builds/hdds/hdds-1.2
setenv BMS_OSNAME Linux_CentOS5-x86_64-gcc4.1.2
setenv PATH ${HALLD_HOME}/bin/${BMS_OSNAME}:$PATH
 
# JANA
setenv JANA_HOME /group/12gev_phys/builds/jana_0.6.3/Linux_CentOS5-x86_64-gcc4.1.2
setenv JANA_CALIB_URL file:///group/halld/Software/calib/latest
setenv JANA_GEOMETRY_URL xmlfile://${HDDS_HOME}/main_HDDS.xml
setenv JANA_PLUGIN_PATH ${JANA_HOME}/lib
setenv PATH ${JANA_HOME}/bin:$PATH
 
# ROOT
setenv ROOTSYS /apps/root/5.26-00-64bit/root
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:$LD_LIBRARY_PATH
setenv PATH ${ROOTSYS}/bin:$PATH
 
# CERNLIB
setenv CERN /apps/cernlib/x86_64_rhel5
setenv CERN_LEVEL 2005
setenv LD_LIBRARY_PATH ${CERN}/${CERN_LEVEL}/lib:$LD_LIBRARY_PATH
setenv PATH ${CERN}/${CERN_LEVEL}/bin:$PATH
 
# Xerces
setenv XERCESCROOT /group/halld/Software/ExternalPackages/xerces-c-src_2_7_0.Linux_CentOS5-x86_64-gcc4.1.2
setenv LD_LIBRARY_PATH ${XERCESCROOT}/lib:$LD_LIBRARY_PATH
 
