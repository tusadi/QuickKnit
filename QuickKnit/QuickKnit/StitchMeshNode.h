#pragma once
#include "mayaIncludes.h"


class StitchMeshNode :public MPxNode
{
	StitchMeshNode();
	virtual ~StitchMeshNode();

	//Node Functions 
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static MStatus	initialize();

	//Attributes 
	static MTypeId nodeID;
	static MObject time;
	static MObject in_Mesh;
	static MObject out_Mesh;
	static MObject stitchSize;
};
