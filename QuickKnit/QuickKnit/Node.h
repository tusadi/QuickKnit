#pragma once
#pragma warning (disable : 4996)
#include <iostream>
#include <vector>
#include "MayaIncludes.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

/*This class represents the Stitch Mesh Data structure */
class Node : public MPxNode
{
public:
	Node() : MPxNode()
	{}
	virtual ~Node();

	//Base class Overridden functions 
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static MStatus	initialize();
	virtual MStatus doIt(const MArgList& args);

	//functions 
	void Tessellate();
	void replaceMesh();
	void relax();

	//Attributes 
	static MTypeId NodeID;
	static MObjectArray selectedFaces;
	static MFnMesh* ipMesh;
	static MFnMesh* opMesh;
	static MFnNumericAttribute stitchSize;
	static MCallbackId callbackId;
	MIntArray faceLoopIndex;		// index of each face in its face loop
	MIntArray faceLoopNumber;		// index of loop that each face is in

	static MObject MOBJ_ipMesh;
	static MObject MOBJ_opMesh;
	static MObject MOBJ_stitchSize;
	static MObject MOBJ_ipMeshName;
	static MObject MOBJ_opMeshName;

};