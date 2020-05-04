#include "StitchMeshNode.h"

#define McheckErr(stat,msg)				\
	if ( MS::kSuccess != stat ) {		\
		cerr << stat << ": " << msg;	\
		return MStatus::kFailure;		\
	}

#define LerpInt(v1,v2,pct)				\
	v1 + floor(pct*(v2-v1))

#define LerpVec(v1,v2,pct)				\
	v1 + pct*(v2-v1)

#define incrementWithWrap(index, size)	\
	(index + 1 < size) ? (index + 1) : 0


MStatus returnStatus;
MObject StitchMeshNode::attr_inMesh;
MObject StitchMeshNode::attr_outMesh;
MObject StitchMeshNode::attr_nodeStage;
MObject StitchMeshNode::attr_stitchSize;
MObject StitchMeshNode::inputMeshName;
MObject StitchMeshNode::outputMeshName;
MTypeId StitchMeshNode::id( 0x00004 );
MString inputShapeName;
MString outputShapeName;


MStatus StitchMeshNode::initialize()
{

	
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;



	StitchMeshNode::attr_stitchSize = nAttr.create("stitchSize", "ss", MFnNumericData::kFloat, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode stitchSize attribute\n");
	nAttr.setMin(0.25);
	nAttr.setMax(8.0);
	nAttr.setSoftMin(0.25);
	nAttr.setSoftMax(8.0);
	


	StitchMeshNode::attr_nodeStage = nAttr.create("nodeStage", "sg", MFnNumericData::kInt, LOOP_SELECTION, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode nodeStage attribute\n");
	

	
	StitchMeshNode::attr_inMesh = tAttr.create("inputMesh", "in", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	StitchMeshNode::inputMeshName = tAttr.create("inputMeshName", "inname", MFnData::kString, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");
	


	StitchMeshNode::attr_outMesh = tAttr.create("outputMesh", "out", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	StitchMeshNode::outputMeshName = tAttr.create("outputMeshName", "outname", MFnData::kString, MObject::kNullObj, &returnStatus);
	tAttr.setStorable(false);
	McheckErr(returnStatus, "ERROR creating StitchMeshNode output attribute\n");



	returnStatus = addAttribute(StitchMeshNode::attr_inMesh);
	returnStatus = addAttribute(StitchMeshNode::inputMeshName);
	McheckErr(returnStatus, "ERROR adding input attributes\n");

	returnStatus = addAttribute(StitchMeshNode::attr_stitchSize);
	McheckErr(returnStatus, "ERROR adding stitchSize attribute\n");

	returnStatus = addAttribute(StitchMeshNode::attr_nodeStage);
	McheckErr(returnStatus, "ERROR adding nodeStage attribute\n");

	returnStatus = addAttribute(StitchMeshNode::attr_outMesh);
	returnStatus = addAttribute(StitchMeshNode::outputMeshName);
	McheckErr(returnStatus, "ERROR adding outputMesh attributes\n");


	
	returnStatus = attributeAffects(StitchMeshNode::attr_inMesh,	 StitchMeshNode::attr_outMesh);
	returnStatus = attributeAffects(StitchMeshNode::attr_nodeStage,  StitchMeshNode::attr_outMesh);
	returnStatus = attributeAffects(StitchMeshNode::attr_stitchSize, StitchMeshNode::attr_outMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	

	return MStatus::kSuccess;
}



void* StitchMeshNode::creator()
{
	StitchMeshNode *node = new StitchMeshNode();
	node->numLoopFaces = 0;
	node->callbackId = -1;
	node->inputMeshFn = NULL;
	node->oMeshFnShape = NULL;
	node->outputMeshObj = MFnMeshData().create();
	node->inputMeshObj = MFnMeshData().create();
	

	
	MPointArray p_cage;
	p_cage.append(-0.5, 0.0, -0.5);
	p_cage.append(-0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0, -0.5);

	Stitch p_stitch(p_cage);

	YarnCurve p_curve0(p_cage);
	p_curve0.addCV( 0.500000000,  0.00000000, -0.167040);
	p_curve0.addCV( 0.500000000,  0.00000000, -0.167040);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.049043700, -0.04998820, -0.136713);
	p_curve0.addCV( 0.024515900, -0.02760550, -0.153349);
	p_curve0.addCV(-1.1953e-005, -0.00522291, -0.169985);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002350680,  0.07519160, -0.258189);
	p_curve0.addCV( 0.000817694,  0.07519160, -0.278117);
	p_curve0.addCV(-0.000715295,  0.07519160, -0.298046);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.000000000,  0.00000000, -0.500000);
	p_curve0.addCV(-0.000000000,  0.00000000, -0.500000);
	p_curve0.addKnot(0); p_curve0.addKnot(0); p_curve0.addKnot(0);
	p_curve0.addKnot(1); p_curve0.addKnot(1); p_curve0.addKnot(1);
	p_curve0.addKnot(2); p_curve0.addKnot(2); p_curve0.addKnot(2);
	p_curve0.addKnot(3); p_curve0.addKnot(3); p_curve0.addKnot(3);
	p_curve0.addKnot(4); p_curve0.addKnot(4); p_curve0.addKnot(4);
	p_curve0.addKnot(5); p_curve0.addKnot(5); p_curve0.addKnot(5);
	p_curve0.addKnot(6); p_curve0.addKnot(6); p_curve0.addKnot(6);
	p_curve0.addKnot(7); p_curve0.addKnot(7); p_curve0.addKnot(7);
	p_stitch.addYarnCurve(p_curve0);
	
	YarnCurve p_curve1(p_cage);
	p_curve1.addCV( 0.5000000000,  0.00000000, 0.167040);
	p_curve1.addCV( 0.5000000000,  0.00000000, 0.167040);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.0490437000, -0.04998820, 0.136713);
	p_curve1.addCV( 0.0245159000, -0.02760550, 0.153349);
	p_curve1.addCV(-1.19531e-005, -0.00522291, 0.169985);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0023506800,  0.07519160, 0.258188);
	p_curve1.addCV( 0.0008176940,  0.07519160, 0.278117);
	p_curve1.addCV(-0.0007152950,  0.07519160, 0.298046);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0000000000,  0.00000000, 0.500000);
	p_curve1.addCV(-0.0000000000,  0.00000000, 0.500000);
	p_curve1.addKnot(0); p_curve1.addKnot(0); p_curve1.addKnot(0);
	p_curve1.addKnot(1); p_curve1.addKnot(1); p_curve1.addKnot(1);
	p_curve1.addKnot(2); p_curve1.addKnot(2); p_curve1.addKnot(2);
	p_curve1.addKnot(3); p_curve1.addKnot(3); p_curve1.addKnot(3);
	p_curve1.addKnot(4); p_curve1.addKnot(4); p_curve1.addKnot(4);
	p_curve1.addKnot(5); p_curve1.addKnot(5); p_curve1.addKnot(5);
	p_curve1.addKnot(6); p_curve1.addKnot(6); p_curve1.addKnot(6);
	p_curve1.addKnot(7); p_curve1.addKnot(7); p_curve1.addKnot(7);
	p_stitch.addYarnCurve(p_curve1);
	
	YarnCurve p_curve2(p_cage);
	p_curve2.addCV(-0.500000, 0.00, -0.16715500);
	p_curve2.addCV(-0.500000, 0.00, -0.16715500);
	p_curve2.addCV(-0.337119, 0.00, -0.15809600);
	p_curve2.addCV(-0.258357, 0.00, -0.25074600);
	p_curve2.addCV(-0.179595, 0.00, -0.34339600);
	p_curve2.addCV( 0.163595, 0.00, -0.35230100);
	p_curve2.addCV( 0.161119, 0.00,  0.00181916);
	p_curve2.addCV( 0.158643, 0.00,  0.35593900);
	p_curve2.addCV(-0.192924, 0.00,  0.30888800);
	p_curve2.addCV(-0.257387, 0.00,  0.24945600);
	p_curve2.addCV(-0.321850, 0.00,  0.19002300);
	p_curve2.addCV(-0.351489, 0.00,  0.16670800);
	p_curve2.addCV(-0.500000, 0.00,  0.16773600);
	p_curve2.addKnot(0); p_curve2.addKnot(0); p_curve2.addKnot(0);
	p_curve2.addKnot(1); p_curve2.addKnot(1); p_curve2.addKnot(1);
	p_curve2.addKnot(2); p_curve2.addKnot(2); p_curve2.addKnot(2);
	p_curve2.addKnot(3); p_curve2.addKnot(3); p_curve2.addKnot(3);
	p_curve2.addKnot(4); p_curve2.addKnot(4); p_curve2.addKnot(4);
	p_stitch.addYarnCurve(p_curve2);

	node->stitches.push_back(p_stitch);
	
	// end
	return node;
}


bool GetSelectedEdge(int &index)
{ 	 
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	if (selected.length() == 0)
		return -1;
	
	MDagPath dagPath;
	selected.getDagPath(0,dagPath);

	if (dagPath.apiType() == 296) {

		MStringArray selectedEdges;
		selected.getSelectionStrings(selectedEdges);
		char *selectedName = (char *)selectedEdges[0].asChar();


		char* pch;
		pch = strtok(selectedName, ".[]");
		vector<char*> selectedNameTokens;
		while (pch != NULL) {
			selectedNameTokens.push_back(pch);
			pch = strtok(NULL, ".[]");
		}

		if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], "e") == 0) {
			index = atoi(selectedNameTokens[2]);
			MString meshShapeName = MString(selectedNameTokens[0]);
			return true;
		}
	}
 	return false;
}

void LabelEdgeRows(void * data)
{

	
	int selectedEdgeIndex;
	if (!GetSelectedEdge(selectedEdgeIndex))
		return;
	
	StitchMeshNode *stitchnode = (StitchMeshNode*) data;


	MFnMesh *inputMeshFn = stitchnode->inputMeshFn;
	MItMeshEdge inputMeshEdgeIt(stitchnode->inputMeshObj);

	int p;								
	int2 courseBkwdVtxs;				
	int2 courseFwrdVtxs;				
	bool inverted = false;				
	MIntArray connectedEdges;			
	MIntArray connectedFaces;			
	PolyMeshFaceLoop faceLoop;			
	int nextCourseEdgeIndex = -1;		

	
	int numBoundingRows = 0;
	int adjFaceLoop = -1;
	int adjFaceIndex = -1;
	inputMeshEdgeIt.setIndex(selectedEdgeIndex, p);
	inputMeshEdgeIt.getConnectedFaces(connectedFaces);
	int numConnectedFaces = connectedFaces.length();
	for (int i = 0; i < numConnectedFaces; i++) {
		int loop = stitchnode->faceLoopNumber[connectedFaces[i]];
		if (loop >= 0) {
			adjFaceLoop = loop;
			adjFaceIndex = stitchnode->faceLoopIndex[connectedFaces[i]];
			numBoundingRows++;
		}
	}
	
	if ((numBoundingRows == 2) || (numBoundingRows == 1 && inputMeshEdgeIt.onBoundary())
		|| (numBoundingRows == 0 && !inputMeshEdgeIt.onBoundary()))
	{
		MString cmd = "select " + stitchnode->name();
		MGlobal::executeCommand(cmd);
		return;
	}

	
	else if (numBoundingRows == 0 && inputMeshEdgeIt.onBoundary())
	{
		do {
			inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseBkwdVtxs);
			if (inverted) { 
				int temp = courseBkwdVtxs[0];
				courseBkwdVtxs[0] = courseBkwdVtxs[1];
				courseBkwdVtxs[1] = temp;
			}

			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int faceIndex = connectedFaces[0];
			
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);

				if (inputMeshEdgeIt.onBoundary())
				{
					int2 courseVertices;
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseVertices);
					
					if (courseVertices[0] != courseBkwdVtxs[0] && courseVertices[1] != courseBkwdVtxs[0]) {
						nextCourseEdgeIndex = connectedEdges[i];
						if (courseVertices[0] == courseBkwdVtxs[1]) { inverted = false; }
						if (courseVertices[1] == courseBkwdVtxs[1]) { inverted = true;  }
					}
				}

				else
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}

			}

			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
	}
		
	
	else if (numBoundingRows == 1 && !inputMeshEdgeIt.onBoundary())
	{
		do {
			stitchnode->MPolyMeshFaceLoops[adjFaceLoop][adjFaceIndex].getCourseEdgeFwrd(courseBkwdVtxs);
			int nextAdjFaceIndex = incrementWithWrap(adjFaceIndex, stitchnode->MPolyMeshFaceLoops[adjFaceLoop].size());

			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int currentLoop = stitchnode->MPolyMeshFaceLoops.size();
			int faceIndex;
			for (int i = 0; i < numConnectedFaces; i++) {
				if (stitchnode->faceLoopNumber[connectedFaces[i]] != adjFaceLoop) { faceIndex = connectedFaces[i]; }
			}

			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);
				inputMeshEdgeIt.getConnectedFaces(connectedFaces);
				int faceId0 = connectedFaces[0];
				int faceId1 = connectedFaces[1];

				if ((faceId0 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId1 == -1) ||
					(faceId1 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId0 == -1))
				{
						nextCourseEdgeIndex = connectedEdges[i];
				}

				else if ((stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == currentLoop) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == currentLoop) )
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}
			}

			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
			adjFaceIndex = nextAdjFaceIndex;
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
	}

	stitchnode->MPolyMeshFaceLoops.push_back(faceLoop);
	stitchnode->numLoopFaces += (int) faceLoop.size();


	for (int i = 0; i < faceLoop.size(); i++) {

		MGlobal::executeCommand("select -d");
		
		PolyMeshFace currentFace = faceLoop[i];
		MString cmd = "select " + inputShapeName + ".f[" + currentFace.faceIndex + "];";
		MGlobal::executeCommand(cmd);

		MGlobal::executeCommand("polyColorPerVertex -r 0 -g 1 -b 0");
		MGlobal::executeCommand("polyOptions -colorShadedDisplay true");
		MGlobal::executeCommand("polyOptions -colorMaterialChannel \"DIFFUSE\"");
		
		MGlobal::executeCommand("select -d");
	}
	

	//if (stitchnode->numLoopFaces < inputMeshFn->numPolygons())
		//stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, stitchnode);

	
	MString cmd = "select " + stitchnode->name();
	//MGlobal::executeCommand(cmd);
	return;
}


MStatus StitchMeshNode::TessellateInputMesh(float stitchSizeData, MFnMesh &outputMeshFn)
{
	

	MSubFaces.clear();
	int subfaceId = 0;
	int numPolyMeshFaceLoops = (int) MPolyMeshFaceLoops.size();
	for (int n = 0; n < numPolyMeshFaceLoops; n++)
	{
		
			
		PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[n];

		int2 vertices; 
		MPoint w0, w1;
		double totalLength = 0.0;
			
		for (int i = 0; i < currentLoop.size(); i++) {

			PolyMeshFace currentFace = currentLoop[i];
			currentFace.getWaleEdge1(vertices);
			inputMeshFn->getPoint(vertices[0], w0);
			inputMeshFn->getPoint(vertices[1], w1);
			totalLength += (w1 - w0).length();

			if (i == currentLoop.size()-1) {
				currentFace.getWaleEdge2(vertices);
				inputMeshFn->getPoint(vertices[0], w0);
				inputMeshFn->getPoint(vertices[1], w1);
				totalLength += (w1 - w0).length();
			}
		} 

		double avgLength = totalLength / (currentLoop.size() + 1);
		int numWaleDivisions = ceil(avgLength / (stitchSizeData));


		for (int f = 0; f < currentLoop.size(); f++)
		{


			PolyMeshFace currentFace = currentLoop[f];


			MPoint v0, v1;
			int2 waleVtxs;
			MIntArray courseVtxs;
				
			MPoint origin;
			inputMeshFn->getPoint(currentFace.courseEdgeBkwd[0], origin);

			MFloatVector wale1Dir;
			currentFace.getWaleEdge1(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale1Dir = v1-v0;

			MFloatVector wale2Dir;
			currentFace.getWaleEdge2(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale2Dir = v1-v0;

			MFloatVector course1Dir;
			currentFace.getCourseEdgeBkwd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course1Dir = v1-v0;
				
			MFloatVector course2Dir;
			currentFace.getCourseEdgeFwrd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course2Dir = v1-v0;

			double courseEdgeBkwdLength = course1Dir.length();
			double courseEdgeFwrdLength = course2Dir.length();
			int numCourseDivisionsBkwd = ceil(courseEdgeBkwdLength / (stitchSizeData));
			int numCourseDivisionsFwrd = ceil(courseEdgeFwrdLength / (stitchSizeData));

			vector<MPointArray> stitchRowPts;
			for (int i = 0; i < stitchRowPts.size(); i++) { stitchRowPts[i].clear(); }
			stitchRowPts.resize(numWaleDivisions + 1);
				
			for (int u = 0; u <= numWaleDivisions; u++) 
			{
				double uPct = (double) u / numWaleDivisions;

				int numRowPts = LerpInt(numCourseDivisionsBkwd, numCourseDivisionsFwrd, uPct);

				for (int v = 0; v <= numRowPts; v++) 
				{
					double vPct = (double) v / numRowPts;

					MPoint pt = origin + wale1Dir*uPct + LerpVec(course1Dir*vPct, course2Dir*vPct, uPct);
						
					stitchRowPts[u].append(pt);
				}
			}

			MPointArray vertexLoop;

			for (int u = 0; u < numWaleDivisions; u++) 
			{

				int numPts1 = stitchRowPts[ u ].length();
				int numPts2 = stitchRowPts[u+1].length();
				int minRowPts = numPts1 < numPts2 ? numPts1 : numPts2;	

				for (int v = 0; v < minRowPts-1; v++)
				{
					vertexLoop.clear();

					vertexLoop.append(stitchRowPts[ u ][ v ]);
					vertexLoop.append(stitchRowPts[ u ][v+1]);
					vertexLoop.append(stitchRowPts[u+1][v+1]);
					vertexLoop.append(stitchRowPts[u+1][ v ]);
					
					SubFace subface(2, 2, subfaceId);
					subface.bkwdPoints.clear();
					subface.fwrdPoints.clear();
					subface.bkwdPoints.append(stitchRowPts[ u ][ v ]);
					subface.bkwdPoints.append(stitchRowPts[ u ][v+1]);
					subface.fwrdPoints.append(stitchRowPts[u+1][ v ]);
					subface.fwrdPoints.append(stitchRowPts[u+1][v+1]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);

					outputMeshFn.addPolygon(vertexLoop, true, 1.0e-3, MObject::kNullObj, NULL);

				}


				if (numPts1 > numPts2) {
					vertexLoop.clear();
					for (int v = numPts2-1; v < numPts1; v++)
						vertexLoop.append(stitchRowPts[u][v]);
					vertexLoop.append(stitchRowPts[u+1][numPts2-1]);
					
					SubFace subface(2, 1, subfaceId);
					for (int v = numPts2-1; v < numPts1; v++)
						subface.bkwdPoints.append(stitchRowPts[u][v]);
					subface.fwrdPoints.append(stitchRowPts[u+1][numPts2-1]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);
					

					outputMeshFn.addPolygon(vertexLoop, true, 1.0e-3, MObject::kNullObj, NULL);

				}

				else if (numPts2 > numPts1) {
					vertexLoop.clear();
					vertexLoop.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts2-1; v >= numPts1-1; v--)
						vertexLoop.append(stitchRowPts[u+1][v]);
					
					SubFace subface(1, 2, subfaceId);
					subface.bkwdPoints.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts1; v < numPts2; v++)
						subface.fwrdPoints.append(stitchRowPts[u+1][v]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);

					outputMeshFn.addPolygon(vertexLoop, true, 1.0e-3, MObject::kNullObj, NULL);

				}
			} 
		} 
	} 
	
	for (int i = 0; i < inputMeshFn->numPolygons(); i++)
		outputMeshFn.deleteFace(0);
	outputMeshFn.updateSurface();
	return MStatus::kSuccess;
}


MStatus StitchMeshNode::ColorByStitchType(void)
{
	for (int i = 0; i < MSubFaces.size(); i++)
	{
		SubFace subface = MSubFaces[i];

		MColor color; int p;
		MItMeshPolygon subfaceIter(outputMeshObj);
		subfaceIter.setIndex(subface.faceId, p);
		subfaceIter.getColor(color);

		if (subface.nBkwd == 2 && subface.nFwrd == 2)
			oMeshFnShape->setFaceColor(MColor(1,1,0), subface.faceId);
		if (subface.nBkwd == 2 && subface.nFwrd == 1)
			oMeshFnShape->setFaceColor(MColor(0,0,1), subface.faceId);
		if (subface.nBkwd == 1 && subface.nFwrd == 2)
			oMeshFnShape->setFaceColor(MColor(1,0,0), subface.faceId);
		MGlobal::executeCommand("setAttr " + outputShapeName + ".displayColors true");
	} 
	return MStatus::kSuccess;
}


MStatus StitchMeshNode::GenerateStitchCurves(float stitchSize)
{
	for (int i = 0; i < MSubFaces.size(); i++)
	{
		SubFace subface = MSubFaces[i];
		Stitch stitch = stitches[subface.stitchType];		

		MPointArray subfaceCage;
		for (int j = 0; j < subface.nBkwd; j++)
			subfaceCage.append(subface.bkwdPoints[j]);
		for (int j = subface.nFwrd-1; j >= 0; j--)
			subfaceCage.append(subface.fwrdPoints[j]);

		// build all yarns
		for (int j = 0; j < stitch.YarnCurves.size(); j++)
		{
			YarnCurve yc = stitch.YarnCurves[j];
			MString curveCmd = "curve -bezier ";
			for (int k = 0; k < yc.CVweights.size(); k++)
			{
				// position in plane
				MPoint cv(0,0,0);
				MVCWeights weights = yc.CVweights[k];
				for (int w = 0; w < weights.length(); w++)
					cv += weights[w] * MVector(subfaceCage[w]);
						
				// offset from plane
				MVector subfaceNormal;
				if (subface.nBkwd == 1) 
					subfaceNormal = (subface.bkwdPoints[0] - subface.fwrdPoints[0]) ^ (subface.fwrdPoints[1] - subface.fwrdPoints[0]);
				else					
					subfaceNormal = (subface.bkwdPoints[1] - subface.bkwdPoints[0]) ^ (subface.fwrdPoints[0] - subface.bkwdPoints[0]);
				cv += yc.CVoffsets[k] * subfaceNormal * stitchSize * 3.0f;
						
				// add cv
				curveCmd += MString("-p ") + cv.x + " " + cv.y + " " + cv.z + " ";
			}

			// add knots
			for (int k = 0; k < yc.knotVector.size(); k++)
				curveCmd += MString("-k ") + yc.knotVector[k] + " ";
					
			// create curve
			MGlobal::executeCommand(curveCmd);
		}
	}

	return MStatus::kSuccess;
}


MStatus StitchMeshNode::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug == attr_outMesh) {
	
		MDataHandle inputMeshNameHandle  = data.inputValue(inputMeshName);
		MDataHandle outputMeshNameHandle = data.inputValue(outputMeshName);
		inputShapeName  = inputMeshNameHandle.asString();
		outputShapeName = outputMeshNameHandle.asString();

		MDataHandle stitchSizeHandle = data.inputValue(attr_stitchSize);
		float stitchSize = stitchSizeHandle.asFloat();

		MDataHandle nodeStageHandle = data.inputValue(attr_nodeStage);
		int nodeStage = nodeStageHandle.asInt();

		MDataHandle inMeshHandle = data.inputValue(attr_inMesh);
		MObject inputMesh = inMeshHandle.asMesh();

		if (inputMeshFn	  != NULL) delete inputMeshFn;
		inputMeshFn	  = new MFnMesh(inputMesh);
		inputMeshFn->copy(inputMesh, inputMeshObj);

		MDataHandle outMeshHandle = data.outputValue(attr_outMesh);
		MObject outputMesh  = MFnMeshData().create();
		MFnMesh outputMeshFn  = MFnMesh().copy(inputMesh, outputMesh);
		outputMeshFn.setObject(outputMesh);

		if (callbackId == -1) {
			for (int i = 0; i < inputMeshFn->numPolygons(); i++) {
				faceLoopIndex.append(-1);
				faceLoopNumber.append(-1);
			}
		}

		
		switch (nodeStage) {

		case (LOOP_SELECTION):
			if (callbackId == -1)
			MGlobal::executeCommand("selectType -pe 1");
			break;

		case (TESSELLATION):
			if (numLoopFaces == inputMeshFn->numPolygons() && nodeStage) {
				TessellateInputMesh(stitchSize, outputMeshFn);
				outMeshHandle.set(outputMesh);
				if (oMeshFnShape == NULL) oMeshFnShape = new MFnMesh;
				oMeshFnShape->copy(outputMesh, outputMeshObj);
			}
			MGlobal::executeCommand("select " + this->name());
			break;

		case (STITCH_EDITING):
			StitchMeshNode::ColorByStitchType();
			outMeshHandle.setMObject(outputMeshObj);
			MGlobal::executeCommand("select " + this->name());
			break;

		case (YARN_GENERATION): {
			float maxEdgeLength = -1;
			MItMeshEdge subfaceEdges(outputMeshObj);
			while (!subfaceEdges.isDone()) {
				double length;
				subfaceEdges.getLength(length);
				if (maxEdgeLength < length || maxEdgeLength < 0)
					maxEdgeLength = length;
				subfaceEdges.next();
			}
			GenerateStitchCurves(1.0/maxEdgeLength);
			MGlobal::executeCommand(MString("hide ") + outputShapeName);
			MGlobal::executeCommand("select " + this->name());
			break;}

		default:
			break;
		}

		data.setClean( plug );
		return MStatus::kSuccess;
	} 
	else return MS::kUnknownParameter;
}
