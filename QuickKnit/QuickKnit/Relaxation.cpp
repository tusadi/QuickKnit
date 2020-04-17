#include "MayaIncludes.h"

MStatus RelaxMesh()
{
	// TODO: 

	// for each face f in the mesh
	// for each subface s in f
	// for each vertex i in s
	// for each other vertex j in s
	// force on i += computeStretchForce(j)
	// for each vertex j in s
	// for each pair of neighbors i and k in s
	// shearForce = computeShearForce(i,j,k)
	// force on j += shearForce
	// force on i -= shearForce / 2.0
	// force on k -= shearForce / 2.0
	// for each wale edge w1 connecting vertex i & j
	// find next wale edge w2 connecting vertex j & k
	// wale strut force = computeWaleStrutForce(i,j,k)
	// force on i += wale strut force
	// force on k -= wale strut force
	// TODO: Solve System
	return MStatus::kSuccess;
}