//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              19AUg2003
//# UPDATE HISTORY:
//#
//#
//===============================================================================

#ifndef SimoPisterWEnergy_H
#define SimoPisterWEnergy_H

#include <Vector.h>
#include <Tensor.h>
//#include <BJvector.h>
//#include <BJtensor.h>

#include <W.h>


class SimoPisterWEnergy : public WEnergy
{
  private:
    double E;
    double nu;
    double G;
    double K;
  public:
    SimoPisterWEnergy( double , double  );
//    SimoPisterWEnergy(  );
    ~SimoPisterWEnergy( ) ;
    WEnergy *newObj( );
    const double  wE(const double &, const Vector & )  ;
//  const Vector   disowOdlambda(const Vector &lambda_wave_in ) ;
//  const Vector const d2isowOdlambda2(const Vector &lambda_wave_in )  ;
//  const Tensor const d2isowOdlambda1dlambda2( const Vector &lambda_wave_in)  ;
    const double   dvolwOdJ( const double &J_in) ;
    const double  d2volwOdJ2( const double &J_in) ;

//    friend OPS_Stream& operator<< (OPS_Stream& os, const LogWEnergy &W);

};

#endif

