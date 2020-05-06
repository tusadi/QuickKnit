#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MIOStream.h>
#include <maya/MPointArray.h>


class SubFace : public MObject
{
public:
	SubFace(int numBackward, int numForward, int faceIndex);
	virtual	   ~SubFace();

	int	nBkwd;
	int	nFwrd;
	int	faceId;
	MPointArray bkwdPoints;
	MPointArray fwrdPoints;
	int	stitchType;
};