/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2001-08-17 16:28:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasConcr1Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasConcr1Material. FedeasConcr1Material wraps the FEDEAS
// 1d material subroutine Concr_1.

#include <FedeasConcr1Material.h>

FedeasConcr1Material::FedeasConcr1Material(int tag,
					 double fc, double ec, double fu, double eu):
// 3 history variables and 4 material parameters
FedeasMaterial(tag, MAT_TAG_FedeasConcrete1, 3, 4)
{
	data[0]  = fc;
	data[1]  = ec;
	data[2]  = fu;
	data[3]  = eu;
}

FedeasConcr1Material::FedeasConcr1Material(void):
FedeasMaterial(0, MAT_TAG_FedeasConcrete1, 3, 4)
{
	// Does nothing
}

FedeasConcr1Material::~FedeasConcr1Material(void)
{
	// Does nothing
}
