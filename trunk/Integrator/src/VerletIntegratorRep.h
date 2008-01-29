#ifndef SimTK_SIMMATH_VERLET_INTEGRATOR_REP_H_
#define SimTK_SIMMATH_VERLET_INTEGRATOR_REP_H_

/* -------------------------------------------------------------------------- *
 *                      SimTK Core: SimTK Simmath(tm)                         *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK Core biosimulation toolkit originating from      *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2007 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon.h"

#include "simmath/internal/common.h"
#include "simmath/Integrator.h"

#include "IntegratorRep.h"

namespace SimTK {

class VerletIntegratorRep : public IntegratorRep {
public:
    VerletIntegratorRep(Integrator* handle, const System& sys);
    void methodInitialize(const State&);
    Integrator::SuccessfulStepStatus stepTo(Real reportTime, Real scheduledEventTime);
    Real getActualInitialStepSizeTaken() const;
    Real getPreviousStepSizeTaken() const;
    Real getPredictedNextStepSize() const;
    long getNStepsAttempted() const;
    long getNStepsTaken() const;
    long getNErrorTestFailures() const;
    void resetMethodStatistics();
    void createInterpolatedState(Real t);
    const char* getMethodName() const;
    int getMethodMinOrder() const;
    int getMethodMaxOrder() const;
    bool methodHasErrorControl() const;
private:
    bool attemptAStep(Real t0, Real t1, 
                      const Vector& q0, const Vector& qdot0, const Vector& qdotdot0, 
                      const Vector& u0, const Vector& udot0, const Vector& z0, 
                      const Vector& zdot0, 
                      Vector& yErrEst);
    bool adjustStepSize(Real err, bool hWasArtificiallyLimited);
    bool takeOneStep(Real t0, Real tMax, Real tReport);
    void backUpAdvancedStateByInterpolation(Real t);
    bool initialized;
    long statsStepsTaken, statsStepsAttempted, statsErrorTestFailures;
    Real currentStepSize, lastStepSize, actualInitialStepSizeTaken;
};

} // namespace SimTK

#endif // SimTK_SIMMATH_VERLET_INTEGRATOR_REP_H_
