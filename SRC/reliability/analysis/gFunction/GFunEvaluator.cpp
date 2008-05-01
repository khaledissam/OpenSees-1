/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.16 $
// $Date: 2008-05-01 18:45:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/gFunction/GFunEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <GFunEvaluator.h>
#include <ReliabilityDomain.h>
#include <RandomVariableIter.h>
#include <tcl.h>

#include <fstream>
#include <limits>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;

GFunEvaluator::GFunEvaluator(Tcl_Interp *passedTclInterp, ReliabilityDomain *passedReliabilityDomain)
{
	theTclInterp = passedTclInterp;
	theReliabilityDomain = passedReliabilityDomain;
	numberOfEvaluations = 0;
}

GFunEvaluator::~GFunEvaluator()
{
}


double
GFunEvaluator::getG()
{
	return g;
}

int
GFunEvaluator::initializeNumberOfEvaluations()
{
	numberOfEvaluations = 0;
	return 0;
}

int
GFunEvaluator::getNumberOfEvaluations()
{
	return numberOfEvaluations;
}


int 
GFunEvaluator::evaluateG(const Vector &x)
{
	numberOfEvaluations++;



	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(lsf);


	// Get the limit-state function expression
	char *theExpression = theLimitStateFunction->getExpression();


	// Set value of GFun-specific parameters in the limit-state function
	int result = this->tokenizeSpecials(theExpression);

	////////////////////////////////////////////////////////////
	// From here to next line of slashes should go away -- MHS

	// Initial declarations
	int i;
	double fileValue = 0.0;

	char buf[500]="";
	char tclAssignment[500]="";
	char tempchar[100]="";
	char temp[120];

	char separators[5] = "}{";
	char *dollarSign = "$";
	char *underscore = "_";

	char lsf_forTokenizing[500];
	strcpy(lsf_forTokenizing,theExpression);


	// Set values of basic random variables and file quantities
	// (Other quantities will have to be set by specific implementations of this class)
	char *tokenPtr = strtok( lsf_forTokenizing, separators);
	while ( tokenPtr != NULL ) {

		// Copy the token pointer over to a temporary storage
		strcpy(tempchar,tokenPtr);

		if ( strncmp(tokenPtr, "x",1) == 0) {
			int rvNum;
			sscanf(tempchar,"x_%i",&rvNum);
			int index = theReliabilityDomain->getRandomVariableIndex(rvNum);
			sprintf(tclAssignment , "set x_%d  %15.5f", rvNum, x(index) );
			Tcl_Eval( theTclInterp, tclAssignment);
		}
		else if ( strncmp(tokenPtr, "file",4) == 0) {
			int rowNum = 0;
			int colNum = 0;
			char fileName[256];
			sscanf(tempchar,"file_%s",fileName);
			int rowloc = strcspn(fileName,"_");
			char rowstr[10] = "";
			int rowcnt = 0;
			for (i=rowloc+1; fileName[i]!='\0'; i++) {
				rowstr[rowcnt] = fileName[i];
				rowcnt++;
			}
			rowstr[rowcnt] = '\0';
			sscanf(rowstr,"%i_%i",&rowNum,&colNum);
			fileName[rowloc] = '\0';

			ifstream inputFile(fileName,ios::in);
			if (!inputFile) {
				opserr << "Could not open file with quantities for limit-state function." << endln;
			}
			for (i=1; i<rowNum; i++) {
				// requires limits, just skips the lines that are not needed
				inputFile.ignore(INT_MAX,'\n');
			}
			for (i=1; i<=colNum; i++) {
				inputFile >> temp;
			}
			
			// check for error in stream
			if ( inputFile.fail() ) {
				opserr << "ERROR: Could not find quantity (" << rowNum << ", " << colNum 
					   << ") in performance function file." << endln;
				fileValue = 0.0;
			} else {
				fileValue = atof(temp);
				//opserr << "Found quantity (" << rowNum << ", " << colNum << ") = " << fileValue << endln;
			}
			
			inputFile.close();
			sprintf(tclAssignment , "set file_%s_%d_%d  %15.5f",fileName,rowNum,colNum,fileValue);

			if (Tcl_Eval(theTclInterp, tclAssignment) == TCL_ERROR) {
				opserr << "ERROR GFunEvaluator -- Tcl_Eval returned error in limit state function" << endln;
				return -1;
			}
		}
		
		tokenPtr = strtok( NULL, separators);
	}

	////////////////////////////////////////////////////////////

	// Set random variable values in Tcl namespace
	int ss = x.Size();
	char buffer[120];
	for (int i = 0; i < ss; i++) {
	  RandomVariable *theRV = theReliabilityDomain->getRandomVariablePtrFromIndex(i);
	  int rvTag = theRV->getTag();
	  sprintf(buffer, "set xrv(%d) %15.5f", rvTag, x(i) );
	  if (Tcl_Eval(theTclInterp, buffer) == TCL_ERROR) {
	    opserr << "GFunEvaluator::evaluateG -- could not set xrv(" << rvTag << ")" << endln;
	    return -1;
	  }
	}


	// Compute value of g-function
	char *theTokenizedExpression = theLimitStateFunction->getTokenizedExpression();
	g = 0.0;
	if (Tcl_ExprDouble( theTclInterp, theTokenizedExpression, &g) != TCL_OK) {
	  opserr << "GFunEvaluator::evaluateG -- could not evaluate expression" << endln;
	  opserr << theTokenizedExpression << endln;

	  return -1;
	}

	return 0;
}



void
GFunEvaluator::setNsteps(int nsteps)
{
	opserr << "GFunEvaluator::set_nsteps() -- This method is not " << endln
		<< " implemented for the chosen type of GFunEvaluator." << endln;
}


double
GFunEvaluator::getDt()
{
	opserr << "GFunEvaluator::getDt() -- This method is not " << endln
		<< " implemented for the chosen type of GFunEvaluator." << endln;
	return 0;
}



void GFunEvaluator::activateSensitivty(void)
{

}

void GFunEvaluator::inactivateSensitivty(void)
{

}

void GFunEvaluator::setGFunEachStepEvaluator(GFunEachStepEvaluator *pGFunEachStepEvaluator)
{

}

void GFunEvaluator::inactivateGFunEachStepEvaluator()
{

}

Matrix * GFunEvaluator::getEachStepResult()
{
	return NULL;
}

Matrix * GFunEvaluator::getEachStepConvFlag()
{
	return NULL;
}

void GFunEvaluator::setThreshold(double a)
{

}

double GFunEvaluator::getThreshold()
{
	return 0.0;
}
void GFunEvaluator::setPerformFuncCoeffs(TaggedObjectStorage* a) 
{

}

void GFunEvaluator::setPerformFuncCoeffIter(PerformanceFunctionCoefficientIter* a)
{

}

double GFunEvaluator::getG2(double g, double littleDt)   //Quan
{
	opserr<<"Fatal::GFunEvaluator::getG2() not implemented yet!"<<endln; exit(-1);

	return 0.0;
}
