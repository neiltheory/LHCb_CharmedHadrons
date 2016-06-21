from ROOT import TLorentzVector, TVector3, TRotation
from math import asin, atan2, sin, cos, acos, sqrt

def getResVars(parent, lab_lamc, lab_p, lab_h1, lab_h2):
    '''
    Takes the Lc decay and outputs a list containing the resonant variables.
    All arguments should be TLorentzVectors. If the Lc is prompt 'parent' is
    just the beam direction.
    '''
  
    lab_ph1  = lab_p + lab_h1
    lab_h1h2 = lab_h1 + lab_h2
    lab_ph2  = lab_p + lab_h2

    # Invariant mass combinations
    Lambdac_sph1  = sqrt(lab_ph1.M2())/1000.0
    Lambdac_sh1h2 = sqrt(lab_h1h2.M2())/1000.0
    Lambdac_sph2  = sqrt(lab_ph2.M2())/1000.0
    
    # Now set the parent direction.
    lab_beam = parent #TLorentzVector()
    #lab_beam.SetXYZM(0.0, 0.0, 1.0, 1.0)      # Beam direction

    lab_xhat = TLorentzVector(1.0, 0.0, 0.0, 1.0)
    lab_yhat = TLorentzVector(0.0, 1.0, 0.0, 1.0)
    lab_zhat = TLorentzVector(0.0, 0.0, 1.0, 1.0)
    
    # Rotation angles
    zeta    = atan2(lab_lamc.Z(), lab_lamc.X() )
    r2_lamc = TLorentzVector( lab_lamc )
    r2_lamc.RotateY( zeta )
    eta     = -asin( r2_lamc.Y() / r2_lamc.Rho() )
    
    R_lab = TRotation()
    R_lab.RotateY( zeta )
    R_lab.RotateZ( eta )

    lab_R1_p = TLorentzVector(lab_p)
    lab_R1_h1 = TLorentzVector(lab_h1)
    lab_R1_h2 = TLorentzVector(lab_h2)
    lab_R1_lamc = TLorentzVector(lab_lamc)
    lab_R1_beam = TLorentzVector(lab_beam)
    lab_R1_xhat = TLorentzVector(lab_xhat)
    lab_R1_yhat = TLorentzVector(lab_yhat)
    lab_R1_zhat = TLorentzVector(lab_zhat)
    
    lab_R1_p.Transform(R_lab)
    lab_R1_h1.Transform(R_lab)
    lab_R1_h2.Transform(R_lab)
    lab_R1_lamc.Transform(R_lab)
    lab_R1_beam.Transform(R_lab)
    lab_R1_xhat.Transform(R_lab)
    lab_R1_yhat.Transform(R_lab)
    lab_R1_zhat.Transform(R_lab)

    # Boost into the Lc rest frame.
    lcRest_R1_p = TLorentzVector(lab_R1_p)
    lcRest_R1_h1 = TLorentzVector(lab_R1_h1)
    lcRest_R1_h2 = TLorentzVector(lab_R1_h2)
    lcRest_R1_lamc = TLorentzVector(lab_R1_lamc)
    lcRest_R1_beam = TLorentzVector(lab_R1_beam)
    
    lcRest_boost = -lab_R1_lamc.BoostVector()
    lcRest_R1_p.Boost(lcRest_boost)
    lcRest_R1_h1.Boost(lcRest_boost)
    lcRest_R1_h2.Boost(lcRest_boost)
    lcRest_R1_lamc.Boost(lcRest_boost)
    lcRest_R1_beam.Boost(lcRest_boost)
      
    # One final transformation to bring the polarization axis,
    #  p_beam X xhat, along the z-axis.
    t3_lcRest_R1_polarize = lcRest_R1_beam.Vect().Cross( TVector3(1.0, 0.0, 0.0) )
    xi = atan2(t3_lcRest_R1_polarize.Y(), t3_lcRest_R1_polarize.Z())
    
    lcRest_R1_p.RotateX( xi )
    lcRest_R1_h1.RotateX( xi )
    lcRest_R1_h2.RotateX( xi )
    lcRest_R1_lamc.RotateX( xi )
    lcRest_R1_beam.RotateX( xi )
    
    # Proton rest frame angles defining the phase space
    proton_LcRest_theta    = lcRest_R1_p.Theta()
    proton_LcRest_costheta = lcRest_R1_p.CosTheta()
    proton_LcRest_phi      = lcRest_R1_p.Phi()
    proton_LcRest_cosphi   = cos(lcRest_R1_p.Phi())
    proton_LcRest_sinphi   = sin(lcRest_R1_p.Phi())
    
    # Decay plane angles
    t3_lcRest_R1_p     = lcRest_R1_p.Vect()
    t3_lcRest_R1_h1       = lcRest_R1_h1.Vect()
    t3_lcRest_R1_h2       = lcRest_R1_h2.Vect()
    t3_lcRest_R1_polarize = lcRest_R1_beam.Vect().Cross( TVector3(1.0, 0.0, 0.0) )
    
    pz_norm_hat = (t3_lcRest_R1_p.Cross(t3_lcRest_R1_polarize)).Unit()
    h1h2_norm_hat = (t3_lcRest_R1_h1.Cross(t3_lcRest_R1_h2)).Unit()
    Lambdac_LcRest_costhetah1h2 = pz_norm_hat.Dot(h1h2_norm_hat)

    sense_vector = h1h2_norm_hat.Cross(pz_norm_hat)
    Lambdac_LcRest_thetah1h2 = acos(Lambdac_LcRest_costhetah1h2)
    if( sense_vector.Dot( t3_lcRest_R1_polarize ) < 0.0) :
      Lambdac_LcRest_thetah1h2 = -Lambdac_LcRest_thetah1h2
    
    return ( Lambdac_sph1,
             Lambdac_sh1h2,
             Lambdac_sph2,
             proton_LcRest_costheta,
             proton_LcRest_cosphi,
             Lambdac_LcRest_thetah1h2 )
