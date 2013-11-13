#!/bin/tcsh
 
# This file was generated by the script "mk_setenv.csh"
#
# Generation date: Wed Nov 13 10:35:03 EST 2013
# User: marki
# Host: jlabl1.jlab.org
# Platform: Linux jlabl1.jlab.org 2.6.18-348.12.1.el5 #1 SMP Mon Jul 1 17:55:31 EDT 2013 i686 i686 i386 GNU/Linux
# BMS_OSNAME: Linux_RHEL5-i686-gcc4.1.2
 
# Make sure LD_LIBRARY_PATH is set
if ( ! $?LD_LIBRARY_PATH ) then
   setenv LD_LIBRARY_PATH
endif
 
# HALLD
setenv HALLD_HOME /group/halld/Software/builds/sim-recon/sim-recon-2013-10-17
setenv HDDS_HOME /group/halld/Software/builds/hdds/hdds-2.0
setenv BMS_OSNAME Linux_RHEL5-i686-gcc4.1.2
setenv PATH ${HALLD_HOME}/bin/${BMS_OSNAME}:$PATH
 
# JANA
setenv JANA_HOME /group/12gev_phys/builds/jana_0.6.6/Linux_RHEL5-i686-gcc4.1.2
setenv JANA_CALIB_URL mysql://ccdb_user@hallddb.jlab.org/ccdb
setenv JANA_GEOMETRY_URL xmlfile://${HDDS_HOME}/main_HDDS.xml
setenv JANA_PLUGIN_PATH ${JANA_HOME}/lib
setenv PATH ${JANA_HOME}/bin:$PATH
 
# CCDB
setenv CCDB_HOME /group/halld/Software/builds/ccdb/Linux_RHEL5-i686-gcc4.1.2/ccdb_0.08
if ( -e $CCDB_HOME/environment.csh ) then
  source $CCDB_HOME/environment.csh
endif
setenv CCDB_CONNECTION mysql://ccdb_user@hallddb.jlab.org/ccdb
 
# ROOT
setenv ROOTSYS /group/halld/Software/ExternalPackages/ROOT/v5.34.01/root_Linux_RHEL5-i686-gcc4.1.2
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:$LD_LIBRARY_PATH
setenv PATH ${ROOTSYS}/bin:$PATH
 
# CERNLIB
setenv CERN /group/halld/Software/ExternalPackages/cernlib/Linux_RHEL5-i686-gcc4.1.2
setenv CERN_LEVEL 2005
setenv LD_LIBRARY_PATH ${CERN}/${CERN_LEVEL}/lib:$LD_LIBRARY_PATH
setenv PATH ${CERN}/${CERN_LEVEL}/bin:$PATH
 
# Xerces
setenv XERCESCROOT /group/halld/Software/ExternalPackages/xerces-c-3.1.1.Linux_RHEL5-i686-gcc4.1.2
setenv LD_LIBRARY_PATH ${XERCESCROOT}/lib:$LD_LIBRARY_PATH
 