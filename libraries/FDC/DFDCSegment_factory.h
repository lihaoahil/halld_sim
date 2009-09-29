//******************************************************************
// DFDCSegment_factory.h: class definition for a factory creating
// track segments from pseudopoints
//******************************************************************
#ifndef DFACTORY_DFDCSEGMENT_H
#define DFACTORY_DFDCSEGMENT_H

#include "JANA/JFactory.h"
#include "JANA/JException.h"
#include "JANA/JStreamLog.h"

#include "DFDCSegment.h"
#include "DFDCPseudo.h"
#include "DFDCHit.h"

#include "HDGEOMETRY/DMagneticFieldMap.h"
#include "HDGEOMETRY/DLorentzDeflections.h"
#include <TDecompLU.h>

#include <algorithm>
#include <map>
#include <cmath>

/* The folowing are for interpreting grid of Lorentz deflection data */
/*#define PACKAGE_Z_POINTS 10 
#define LORENTZ_X_POINTS 21
#define LORENTZ_Z_POINTS 4*PACKAGE_Z_POINTS
*/

///
/// class DFDCSegment_factory: definition for a JFactory that
/// produces space points from pseudopoints.
/// 
class DFDCSegment_factory : public JFactory<DFDCSegment> {
	public:
		
		///
		/// DFDCSegment_factory::DFDCSegment_factory():
		/// default constructor -- initializes log file
		///
		DFDCSegment_factory();
		
		///
		/// DFDCSegment_factory::~DFDCSegment_factory():
		/// default destructor -- closes log file
		///
		~DFDCSegment_factory();	

		jerror_t FindSegments(vector<DFDCPseudo*>points);
//		jerror_t CorrectPoints(vector<DFDCPseudo*>point, DMatrix XYZ);
		jerror_t GetHelicalTrackPosition(double z,
						 const DFDCSegment *segment,
						 double &xpos,
						 double &ypos);
		jerror_t RiemannHelicalFit(vector<DFDCPseudo*>points,
					   DMatrix &CR,
					   DMatrix &XYZ);
	        jerror_t RiemannCircleFit(vector<DFDCPseudo*>points,
			DMatrix CRPhi);
		jerror_t RiemannLineFit(vector<DFDCPseudo *>points,
					DMatrix CR,DMatrix &XYZ);
	        jerror_t UpdatePositionsAndCovariance(unsigned int n,
						      double r1sq,
			DMatrix &XYZ, DMatrix &CRPhi,DMatrix &CR);
		double GetProcessNoise(unsigned int i, DMatrix XYZ);
		double GetCharge(unsigned int n,
				 DMatrix XYZ, DMatrix CR, DMatrix CRPhi);

	protected:
		///
		/// DFDCSegment_factory::brun():
		///
		jerror_t brun(JEventLoop *eventLoop, int eventNo);

		///
		/// DFDCSegment_factory::evnt():
		/// this is the place that finds track segments and  
		/// converts pseudopoints into space points.
		///
		jerror_t evnt(JEventLoop *eventLoop, int eventNo);

	private:
		JStreamLog* _log;
		ofstream* logFile;

		double N[3];
	        double varN[3][3];
	 	double dist_to_origin,xc,yc,rc;
		double xavg[3],var_avg;
		
		// Track parameters
		double tanl,z0,zvertex,D,kappa,phi0;
		double var_tanl,Phi1;
		double charge;
		unsigned int ref_plane;
	
		vector<fdc_track_t>fdc_track;
		double chisq;

                const DMagneticFieldMap *bfield;
		const DLorentzDeflections *lorentz_def;
//		double ref_time;
//		bool use_tof,use_sc;

		int myeventno;
};

#endif // DFACTORY_DFDCSEGMENT_H

