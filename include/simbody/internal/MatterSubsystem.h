#ifndef SimTK_SIMBODY_MATTER_SUBSYSTEM_H_
#define SimTK_SIMBODY_MATTER_SUBSYSTEM_H_

/* Copyright (c) 2006 Stanford University and Michael Sherman.
 * Contributors:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "SimTKcommon.h"
#include "simbody/internal/common.h"
#include "simbody/internal/State.h"
#include "simbody/internal/Subsystem.h"

namespace SimTK {

/// The still-abstract parent of all MatterSubsystems (such as the
/// one generated by Simbody). This is derived from Subsystem.
class SimTK_SIMBODY_API MatterSubsystem : public Subsystem {
public:
    MatterSubsystem() { }

    void setForceSubsystemIndex(int subsys);
    int  getForceSubsystemIndex() const;

    // Topological information (no state)
    int getNBodies()      const;    // includes ground, also # tree joints+1
    int getNMobilities()  const;
    int getNConstraints() const;    // i.e., constraint elements (multiple equations)

    int        getParent  (int bodyNum) const;
    Array<int> getChildren(int bodyNum) const;

    const Transform&  getJointFrame(const State&, int bodyNum) const;
    const Transform&  getJointFrameOnParent(const State&, int bodyNum) const;

    const Vec3&       getBodyCenterOfMass(const State&, int bodyNum) const;


    // This can be called at any time after construction. It sizes a set of
    // force arrays (if necessary) and then sets them to zero. The "addIn"
    // operators below can then be used to accumulate forces.
    void resetForces(Vector_<SpatialVec>& bodyForces,
                     Vector_<Vec3>&       particleForces,
                     Vector&              mobilityForces) const 
    {
        bodyForces.resize(getNBodies()); bodyForces.setToZero();
        particleForces.resize(0); // TODO
        mobilityForces.resize(getNMobilities()); mobilityForces.setToZero();
    }


    /// Add in gravity to a body forces vector. Be sure to call this only once
    /// per evaluation! Must be realized to Configured stage prior to call.
    void addInGravity(const State&, const Vec3& g, Vector_<SpatialVec>& bodyForces) const;

    /// Apply a force to a point on a body (a station). Provide the
    /// station in the body frame, force in the ground frame. Must
    /// be realized to Configured stage prior to call.
    void addInPointForce(const State&, int body, const Vec3& stationInB, 
                         const Vec3& forceInG, Vector_<SpatialVec>& bodyForces) const;

    /// Apply a torque to a body. Provide the torque vector in the
    /// ground frame.
    void addInBodyTorque(const State&, int body, const Vec3& torqueInG, 
                         Vector_<SpatialVec>& bodyForces) const;

    /// Apply a scalar joint force or torque to an axis of the
    /// indicated body's inboard joint.
    void addInMobilityForce(const State&, int body, int axis, const Real& f,
                            Vector& mobilityForces) const;

    // Kinematic information.
    const Transform&  getBodyConfiguration(const State&, int bodyNum) const;
    const SpatialVec& getBodyVelocity(const State&, int bodyNum) const;

    const Real& getJointQ(const State&, int body, int axis) const;
    const Real& getJointU(const State&, int body, int axis) const;

    void setJointQ(State&, int body, int axis, const Real&) const;
    void setJointU(State&, int body, int axis, const Real&) const;


    /// This is available at Stage::Configured. These are *absolute* constraint
    /// violations qerr=g(t,q), that is, they are unweighted.
    const Vector& getQConstraintErrors(const State&) const;

    /// This is the weighted norm of the errors returned by getQConstraintErrors(),
    /// available whenever this subsystem has been realized to Stage::Configured.
    /// This is the scalar quantity that we need to keep below "tol"
    /// during integration.
    const Real&   getQConstraintNorm(const State&) const;

    /// This is available at Stage::Moving. These are *absolute* constraint
    /// violations verr=v(t,q,u), that is, they are unweighted.
    const Vector& getUConstraintErrors(const State&) const;

    /// This is the weighted norm of the errors returned by getQConstraintErrors().
    /// That is, this is the scalar quantity that we need to keep below "tol"
    /// during integration.
    const Real&   getUConstraintNorm(const State&) const;

    /// This is a solver you can call after the State has been realized
    /// to stage Timed (i.e., Configured-1). It will project the Q constraints
    /// along the error norm so that getQConstraintNorm() <= tol, and will
    /// project out the corresponding component of y_err so that y_err's Q norm
    /// is reduced. Returns true if it does anything at all to State or y_err.
    bool projectQConstraints(State&, Vector& y_err, Real tol, Real targetTol) const;

    /// This is a solver you can call after the State has been realized
    /// to stage Configured (i.e., Moving-1). It will project the U constraints
    /// along the error norm so that getUConstraintNorm() <= tol, and will
    /// project out the corresponding component of y_err so that y_err's U norm
    /// is reduced.
    bool projectUConstraints(State&, Vector& y_err, Real tol, Real targetTol) const;

    SimTK_PIMPL_DOWNCAST(MatterSubsystem, Subsystem);
private:
    class MatterSubsystemRep& updRep();
    const MatterSubsystemRep& getRep() const;
};

} // namespace SimTK

#endif // SimTK_SIMBODY_MATTER_SUBSYSTEM_H_
