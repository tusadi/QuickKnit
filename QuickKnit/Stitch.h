#include <vector>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>

typedef MFloatArray MVCWeights;


class YarnCurve 
{
public:
				YarnCurve();
				YarnCurve(MPointArray corners);
	virtual	   ~YarnCurve();
	
	MStatus		addKnot(int k);
	MStatus		addCV(MPoint cv);
	MStatus		addCV(float cvX, float cvY, float cvZ);

	MPointArray cageVertices;			
	std::vector<int> knotVector;		
	std::vector<float> CVoffsets;		
	std::vector<MVCWeights> CVweights;	
};

class Stitch
{
public:
				Stitch(void);
				Stitch(MPointArray corners);
	virtual	   ~Stitch();

	MStatus		addYarnCurve(YarnCurve curve);

	MPointArray cageVertices;
	std::vector<YarnCurve> YarnCurves;
};