
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#include "TLorentzVector.h"
#include "TLorentzRotation.h"

#include "IUAmpTools/Kinematics.h"
#include "AMPTOOLS_AMPS/TwoPiAngles.h"
#include "AMPTOOLS_AMPS/clebschGordan.h"
#include "AMPTOOLS_AMPS/wignerD.h"

#include "UTILITIES/BeamProperties.h"

TwoPiAngles::TwoPiAngles( const vector< string >& args ) :
UserAmplitude< TwoPiAngles >( args )
{
	assert( args.size() == 11 );
	
	rho000  = AmpParameter( args[0] );
	rho100  = AmpParameter( args[1] );
	rho1m10 = AmpParameter( args[2] );
  
	rho111  = AmpParameter( args[3] );
	rho001  = AmpParameter( args[4] );
	rho101  = AmpParameter( args[5] );
	rho1m11 = AmpParameter( args[6] );
	
	rho102  = AmpParameter( args[7] );
	rho1m12 = AmpParameter( args[8] );

	polAngle = AmpParameter( args[9] );

	polFraction = atof(args[10].c_str());

	// need to register any free parameters so the framework knows about them
	registerParameter( rho000 );
	registerParameter( rho100 );
	registerParameter( rho1m10 );

	registerParameter( rho111 );
	registerParameter( rho001 );
	registerParameter( rho101 );
	registerParameter( rho1m11 );

	registerParameter( rho102 );
	registerParameter( rho1m12 );

	registerParameter( polAngle );

	// BeamProperties configuration file
	TString beamConfigFile = args[10].c_str();
	BeamProperties beamProp(beamConfigFile);
	polFrac_vs_E = (TH1D*)beamProp.GetPolFrac();
}


complex< GDouble >
TwoPiAngles::calcAmplitude( GDouble** pKin ) const {
  
	TLorentzVector beam   ( pKin[0][1], pKin[0][2], pKin[0][3], pKin[0][0] ); 
	TLorentzVector recoil ( pKin[1][1], pKin[1][2], pKin[1][3], pKin[1][0] ); 
	TLorentzVector p1     ( pKin[2][1], pKin[2][2], pKin[2][3], pKin[2][0] ); 
	TLorentzVector p2     ( pKin[3][1], pKin[3][2], pKin[3][3], pKin[3][0] ); 
	
	TLorentzVector resonance = p1 + p2;
	TLorentzRotation resonanceBoost( -resonance.BoostVector() );
	
	TLorentzVector beam_res = resonanceBoost * beam;
	TLorentzVector recoil_res = resonanceBoost * recoil;
	TLorentzVector p1_res = resonanceBoost * p1;
	
	// normal to the production plane
        TVector3 y = (beam.Vect().Unit().Cross(-recoil.Vect().Unit())).Unit();

        // choose helicity frame: z-axis opposite recoil proton in rho rest frame
        TVector3 z = -1. * recoil_res.Vect().Unit();
        TVector3 x = y.Cross(z).Unit();
        TVector3 angles( (p1_res.Vect()).Dot(x),
                         (p1_res.Vect()).Dot(y),
                         (p1_res.Vect()).Dot(z) );

        GDouble cosTheta = angles.CosTheta();
        GDouble sinSqTheta = sin(angles.Theta())*sin(angles.Theta());
        GDouble sin2Theta = sin(2.*angles.Theta());

        GDouble phi = angles.Phi();

        TVector3 eps(cos(polAngle*TMath::DegToRad()), sin(polAngle*TMath::DegToRad()), 0.0); // beam polarization vector
        GDouble Phi = atan2(y.Dot(eps), beam.Vect().Unit().Dot(eps.Cross(y)));
	
	// vector meson production from K. Schilling et. al.
	GDouble Pgamma;
	if(polFraction > 0.) { // for fitting with constant polarization 
		Pgamma = polFraction;
	}
	else{
		int bin = polFrac_vs_E->GetXaxis()->FindBin(pKin[0][0]);
		if (bin == 0 || bin > polFrac_vs_E->GetXaxis()->GetNbins()){
			Pgamma = 0.;
		}
		else Pgamma = polFrac_vs_E->GetBinContent(bin);
	}
	
	// vector meson production from K. Schilling et. al.
	GDouble W = 0.5*(1. - rho000) + 0.5*(3.*rho000 - 1.)*cosTheta*cosTheta - sqrt(2.)*rho100*sin2Theta*cos(phi) - rho1m10*sinSqTheta*cos(2.*phi);
	
	W -= Pgamma*cos(2.*Phi) * (rho111*sinSqTheta + rho001*cosTheta*cosTheta - sqrt(2.)*rho101*sin2Theta*cos(phi) - rho1m11*sinSqTheta*cos(2.*phi));
	
	W -= Pgamma*sin(2.*Phi) * (sqrt(2.)*rho102*sin2Theta*sin(phi) + rho1m12*sinSqTheta*sin(2.*phi));
	
	W *= 3./(4.*PI);

	return complex< GDouble > ( sqrt(fabs(W)) );
}

