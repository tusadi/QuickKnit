#include "QuickKnitNode.h"

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
MObject QuickKnitNode::attr_inMesh;
MObject QuickKnitNode::attr_outMesh;
MObject QuickKnitNode::attr_nodeStage;
MObject QuickKnitNode::attr_stitchSize;
MObject QuickKnitNode::inputMeshName;
MObject QuickKnitNode::outputMeshName;
MTypeId QuickKnitNode::id( 0x00004 );
MString inputShapeName;
MString outputShapeName;


MStatus QuickKnitNode::initialize()
{
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	QuickKnitNode::attr_stitchSize = nAttr.create("stitchSize", "ss", MFnNumericData::kFloat, 1.0, &returnStatus);
	nAttr.setMin(0.25);
	nAttr.setMax(8.0);
	nAttr.setSoftMin(0.25);
	nAttr.setSoftMax(8.0);
	
	QuickKnitNode::attr_nodeStage = nAttr.create("nodeStage", "sg", MFnNumericData::kInt, LOOP_SELECTION, &returnStatus);

	QuickKnitNode::attr_inMesh = tAttr.create("inputMesh", "in", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	QuickKnitNode::inputMeshName = tAttr.create("inputMeshName", "inname", MFnData::kString, MObject::kNullObj, &returnStatus);

	QuickKnitNode::attr_outMesh = tAttr.create("outputMesh", "out", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	QuickKnitNode::outputMeshName = tAttr.create("outputMeshName", "outname", MFnData::kString, MObject::kNullObj, &returnStatus);
	tAttr.setStorable(false);

	// Add input mesh attribute
	returnStatus = addAttribute(QuickKnitNode::attr_inMesh);
	returnStatus = addAttribute(QuickKnitNode::inputMeshName);

	// Add stitch size attribute
	returnStatus = addAttribute(QuickKnitNode::attr_stitchSize);

	// Add stitch size attribute
	returnStatus = addAttribute(QuickKnitNode::attr_nodeStage);

	// Add mesh attribute
	returnStatus = addAttribute(QuickKnitNode::attr_outMesh);
	returnStatus = addAttribute(QuickKnitNode::outputMeshName);

	returnStatus = attributeAffects(QuickKnitNode::attr_inMesh,	 QuickKnitNode::attr_outMesh);
	returnStatus = attributeAffects(QuickKnitNode::attr_nodeStage,  QuickKnitNode::attr_outMesh);
	returnStatus = attributeAffects(QuickKnitNode::attr_stitchSize, QuickKnitNode::attr_outMesh);

	
	return MStatus::kSuccess;
}

void* QuickKnitNode::creator()
{
	QuickKnitNode *node = new QuickKnitNode();
	node->numLoopFaces = 0;
	node->callbackId = -1;
	node->inputMeshFn = NULL;
	node->oMeshFnShape = NULL;
	node->outputMeshObj = MFnMeshData().create();
	node->inputMeshObj = MFnMeshData().create();
	
	
	// create cage vertices for stitch "p"
	MPointArray p_cage;
	p_cage.append(-0.5, 0.0, -0.5);
	p_cage.append(-0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0, -0.5);

	// create stitch "p" from cage
	Stitch p_stitch(p_cage);

	// create each of p_stitch's 3 yarns
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
	// get the current selection list from maya
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	if (selected.length() == 0)
		return -1;
	
	// get the dagPath to the current selection
	MDagPath dagPath;
	selected.getDagPath(0,dagPath);

	// if the selection is a mesh geometry component
	if (dagPath.apiType() == 296) {

		// get the name of the selection
		MStringArray selectedEdges;
		selected.getSelectionStrings(selectedEdges);
		char *selectedName = (char *)selectedEdges[0].asChar();

		// split the name of the selection to get the edge index
		char* pch;
		pch = strtok(selectedName, ".[]");
		vector<char*> selectedNameTokens;
		while (pch != NULL) {
			selectedNameTokens.push_back(pch);
			pch = strtok(NULL, ".[]");
		}

		// return the edge index
		if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], "e") == 0) {
			index = atoi(selectedNameTokens[2]);
			MString meshShapeName = MString(selectedNameTokens[0]);
			// TODO: test if meshShapeName equals inputShapeName
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

	// input stitch mesh node from client data pointer
	QuickKnitNode *stitchnode = (QuickKnitNode*) data;


	MFnMesh *inputMeshFn = stitchnode->inputMeshFn;
	MItMeshEdge inputMeshEdgeIt(stitchnode->inputMeshObj);

	int p;								// used in setting index of iterator
	int2 courseBkwdVtxs;				// array for storing indices of PolyMeshFace's bkwd course vertices
	int2 courseFwrdVtxs;				// array for storing indices of PolyMeshFace's fwrd course vertices
	bool inverted = false;				// does the current edge need to be flipped to match direction?
	MIntArray connectedEdges;			// array for storing indices of edge's adjacent edges
	MIntArray connectedFaces;			// array for storing indices of edge's adjacent faces
	PolyMeshFaceLoop faceLoop;			// declare new faceLoop
	int nextCourseEdgeIndex = -1;		// store next course edge index
	
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

	//------------------------------------------------------------------------------------------------------------------//
	// CASE 1: This edge is already bounded by face loops and there is no need to create a new one, or it is an			//
	//		   interior edge with no adjacent loops																		//
	//------------------------------------------------------------------------------------------------------------------//
	
	if ((numBoundingRows == 2) || (numBoundingRows == 1 && inputMeshEdgeIt.onBoundary())
		|| (numBoundingRows == 0 && !inputMeshEdgeIt.onBoundary()))
	{
		//stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, stitchnode);
		LabelEdgeRows(stitchnode);
		MString cmd = "select " + stitchnode->name();
		MGlobal::executeCommand(cmd);
		return;
	}

	//------------------------------------------------------------------------------------------------------------------//
	// CASE 2: This edge is part of a boundary edge loop that is not yet associated with a face loop					//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 0 && inputMeshEdgeIt.onBoundary())
	{
		do {
			// get vertex endpoints of selected edge
			inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseBkwdVtxs);
			if (inverted) { 
				int temp = courseBkwdVtxs[0];
				courseBkwdVtxs[0] = courseBkwdVtxs[1];
				courseBkwdVtxs[1] = temp;
			}

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int faceIndex = connectedFaces[0];
			
			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);

				// if the connected edge is on the boundary, then
				// it could be the next course edge in the loop
				if (inputMeshEdgeIt.onBoundary())
				{
					int2 courseVertices;
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), courseVertices);
					
					// connected to edge in correct direction
					if (courseVertices[0] != courseBkwdVtxs[0] && courseVertices[1] != courseBkwdVtxs[0]) {
						nextCourseEdgeIndex = connectedEdges[i];
						if (courseVertices[0] == courseBkwdVtxs[1]) { inverted = false; }
						if (courseVertices[1] == courseBkwdVtxs[1]) { inverted = true;  }
					}
				}

				// if it is not on the boundary of the mesh, it
				// cannot be a course edge and must be a wale edge
				else
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

					// connected to left, direction is forward
					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					// connected to left, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					// connected to right, direction is forward
					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					// connected to right, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}

			}

			// Make a face with connectedVertices and faceTopVertices
			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			// continue to the next course edge for the loop
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
	}
		
	//------------------------------------------------------------------------------------------------------------------//
	// CASE 3: This edge is part of an interior edge loop and is bounded by existing face loops on only one side		//
	//------------------------------------------------------------------------------------------------------------------//
	
	else if (numBoundingRows == 1 && !inputMeshEdgeIt.onBoundary())
	{
		do {
			// get course vertex endpoint indices in same direction as
			// adjacent face's forward edge
			stitchnode->MPolyMeshFaceLoops[adjFaceLoop][adjFaceIndex].getCourseEdgeFwrd(courseBkwdVtxs);
			int nextAdjFaceIndex = incrementWithWrap(adjFaceIndex, stitchnode->MPolyMeshFaceLoops[adjFaceLoop].size());

			// get connected edges and faces for current edge
			connectedEdges.clear();
			connectedFaces.clear();
			int numConnectedEdges = inputMeshEdgeIt.getConnectedEdges(connectedEdges);
			int numConnectedFaces = inputMeshEdgeIt.getConnectedFaces(connectedFaces);
			int currentLoop = stitchnode->MPolyMeshFaceLoops.size();
			int faceIndex;
			for (int i = 0; i < numConnectedFaces; i++) {
				if (stitchnode->faceLoopNumber[connectedFaces[i]] != adjFaceLoop) { faceIndex = connectedFaces[i]; }
			}

			// go through connected edges
			for (int i = 0; i < connectedEdges.length(); i++)
			{
				inputMeshEdgeIt.setIndex(connectedEdges[i], p);
				inputMeshEdgeIt.getConnectedFaces(connectedFaces);
				int faceId0 = connectedFaces[0];
				int faceId1 = connectedFaces[1];

				// if the connected edge has one adjacent face equal to the next adjacent face in the bkwd loop
				// and one face not currently in a loop, then it is the next bkwd course edge
				if ((faceId0 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId1 == -1) ||
					(faceId1 == stitchnode->MPolyMeshFaceLoops[adjFaceLoop][nextAdjFaceIndex].faceIndex && faceId0 == -1))
				{
						nextCourseEdgeIndex = connectedEdges[i];
				}

				// if it is not the next backward course edge, then it must have two adjacent faces not in a loop 
				// or one (or two, in the case of the last face) adjacent face(s) in the current loop to be one of the wale edges
				else if ((stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == -1			 && stitchnode->faceLoopNumber[faceId1] == currentLoop) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == -1		  ) ||
						 (stitchnode->faceLoopNumber[faceId0] == currentLoop && stitchnode->faceLoopNumber[faceId1] == currentLoop) )
				{
					int2 waleVertices; 
					inputMeshFn->getEdgeVertices(inputMeshEdgeIt.index(), waleVertices);

					// connected to left, direction is forward
					if (waleVertices[0] == courseBkwdVtxs[0])	   { courseFwrdVtxs[0] = waleVertices[1]; } 

					// connected to left, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[0]) { courseFwrdVtxs[0] = waleVertices[0]; }

					// connected to right, direction is forward
					else if (waleVertices[0] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[1]; }

					// connected to right, direction is inverted
					else if (waleVertices[1] == courseBkwdVtxs[1]) { courseFwrdVtxs[1] = waleVertices[0]; }
				}
			}

			// Make a face with connectedVertices and faceTopVertices
			MIntArray courseBwkd, courseFwrd;
			courseBwkd.append(courseBkwdVtxs[0]); courseBwkd.append(courseBkwdVtxs[1]);
			courseFwrd.append(courseFwrdVtxs[0]); courseFwrd.append(courseFwrdVtxs[1]);
			PolyMeshFace face = PolyMeshFace(courseBwkd, courseFwrd, faceIndex);
			stitchnode->faceLoopIndex[faceIndex] = (int) faceLoop.size();
			stitchnode->faceLoopNumber[faceIndex] = (int) stitchnode->MPolyMeshFaceLoops.size();
			faceLoop.push_back(face);
			
			// continue to the next course edge for the loop
			inputMeshEdgeIt.setIndex(nextCourseEdgeIndex, p);
			adjFaceIndex = nextAdjFaceIndex;
		
		} while (inputMeshEdgeIt.index() != selectedEdgeIndex);
	}
	
	//------------------------------------------------------------------------------------------------------------------//
	// Push new face loop onto node's vector and update the number of faces already accounted for by loops				//
	//------------------------------------------------------------------------------------------------------------------//

	stitchnode->MPolyMeshFaceLoops.push_back(faceLoop);
	stitchnode->numLoopFaces += (int) faceLoop.size();
	
	//------------------------------------------------------------------------------------------------------------------//
	// Display colors for selected face loop																			//
	//------------------------------------------------------------------------------------------------------------------//

	for (int i = 0; i < faceLoop.size(); i++) {

		// clear current vertex-face selection
		MGlobal::executeCommand("select -d");
		
		// get current polymeshface of loop for coloring
		PolyMeshFace currentFace = faceLoop[i];
		MString cmd = "select " + inputShapeName + ".f[" + currentFace.faceIndex + "];";
		MGlobal::executeCommand(cmd);

		// color per vertex
		MGlobal::executeCommand("polyColorPerVertex -r 0 -g 1 -b 0");
		MGlobal::executeCommand("polyOptions -colorShadedDisplay true");
		MGlobal::executeCommand("polyOptions -colorMaterialChannel \"DIFFUSE\"");
		
		// clear current vertex-face selection
		MGlobal::executeCommand("select -d");
	}
	
	//------------------------------------------------------------------------------------------------------------------//
	// Re-add callback to program if it is still needed																	//
	//------------------------------------------------------------------------------------------------------------------//

	if (stitchnode->numLoopFaces < inputMeshFn->numPolygons())
		//stitchnode->callbackId = MEventMessage::addEventCallback("SelectionChanged", LabelEdgeRows, stitchnode);
		LabelEdgeRows(stitchnode);
	//------------------------------------------------------------------------------------------------------------------//
	// Return from callback function																					//
	//------------------------------------------------------------------------------------------------------------------//
	
	MString cmd = "select " + stitchnode->name();
	MGlobal::executeCommand(cmd);
	return;
}


// Tessellate the Input Mesh
MStatus QuickKnitNode::TessellateInputMesh(float stitchSizeData, MFnMesh &outputMeshFn)
{

	MSubFaces.clear();
	int subfaceId = 0;
	int &IDRef = subfaceId;

	int numPolyMeshFaceLoops = (int) MPolyMeshFaceLoops.size();
	for (int n = 0; n < numPolyMeshFaceLoops; n++)
	{
		
		// get current poly mesh face loop
		PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[n];

		// local variables
		int2 vertices; 
		MPoint w0, w1;
		double totalLength = 0.0;
			
		// loop through all faces to examine wale edge lengths
		for (int i = 0; i < currentLoop.size(); i++) {

			// get first wale edge
			PolyMeshFace currentFace = currentLoop[i];
			currentFace.getWaleEdge1(vertices);
			inputMeshFn->getPoint(vertices[0], w0);
			inputMeshFn->getPoint(vertices[1], w1);
			totalLength += (w1 - w0).length();

			// get second wale edge only for final face (assuming potential for unclosed loops)
			if (i == currentLoop.size()-1) {
				currentFace.getWaleEdge2(vertices);
				inputMeshFn->getPoint(vertices[0], w0);
				inputMeshFn->getPoint(vertices[1], w1);
				totalLength += (w1 - w0).length();
			}
		} 

		// divide total sum by number of edges
		double avgLength = totalLength / (currentLoop.size() + 1);
		int numWaleDivisions = ceil(avgLength / (stitchSizeData));

		for (int f = 0; f < currentLoop.size(); f++)
		{
			PolyMeshFace currentFace = currentLoop[f];

			MPoint v0, v1;
			int2 waleVtxs;
			MIntArray courseVtxs;
				
			// get backwards-first corner as origin
			MPoint origin;
			inputMeshFn->getPoint(currentFace.courseEdgeBkwd[0], origin);

			// vector corresponding to first wale edge
			MFloatVector wale1Dir;
			currentFace.getWaleEdge1(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale1Dir = v1-v0;

			// vector corresponding to second wale edge
			MFloatVector wale2Dir;
			currentFace.getWaleEdge2(waleVtxs); 
			inputMeshFn->getPoint(waleVtxs[0], v0);
			inputMeshFn->getPoint(waleVtxs[1], v1);
			wale2Dir = v1-v0;

			// vector corresponding to backwards course edge
			MFloatVector course1Dir;
			currentFace.getCourseEdgeBkwd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course1Dir = v1-v0;
				
			// vector corresponding to forwards course edge
			MFloatVector course2Dir;
			currentFace.getCourseEdgeFwrd(courseVtxs);
			inputMeshFn->getPoint(courseVtxs[0], v0);
			inputMeshFn->getPoint(courseVtxs[1], v1);
			course2Dir = v1-v0;

			double courseEdgeBkwdLength = course1Dir.length();
			double courseEdgeFwrdLength = course2Dir.length();
			int numCourseDivisionsBkwd = ceil(courseEdgeBkwdLength / (stitchSizeData));
			int numCourseDivisionsFwrd = ceil(courseEdgeFwrdLength / (stitchSizeData));

			// allocate stitch row point arrays
			vector<MPointArray> stitchRowPts;
			for (int i = 0; i < stitchRowPts.size(); i++) { stitchRowPts[i].clear(); }
			stitchRowPts.resize(numWaleDivisions + 1);
				
			// populate stitch row point arrays
			for (int u = 0; u <= numWaleDivisions; u++) 
			{
				// u-direction percentage
				double uPct = (double) u / numWaleDivisions;

				// determine number of points to add for current stitch row r
				int numRowPts = LerpInt(numCourseDivisionsBkwd, numCourseDivisionsFwrd, uPct);

				// for each point in row
				for (int v = 0; v <= numRowPts; v++) 
				{
					// v direction percentage
					double vPct = (double) v / numRowPts;

					// determine point location
					MPoint pt = origin + wale1Dir*uPct + LerpVec(course1Dir*vPct, course2Dir*vPct, uPct);
						
					// add point to row-specific MPointArray
					stitchRowPts[u].append(pt);
				}
			}

			MPointArray vertexLoop;

			for (int u = 0; u < numWaleDivisions; u++) 
			{

				// find smaller number of points between two consecutive stitch rows
				int numPts1 = stitchRowPts[ u ].length();
				int numPts2 = stitchRowPts[u+1].length();
				int minRowPts = (numPts1 < numPts2) ? numPts1 : numPts2; //min(numPts1, numPts2);

				// use this number to add quad stitch faces
				for (int v = 0; v < minRowPts-1; v++)
				{
					// clear current subface vertex loop
					vertexLoop.clear();

					// get points in counterclockwise order
					vertexLoop.append(stitchRowPts[ u ][ v ]);
					vertexLoop.append(stitchRowPts[ u ][v+1]);
					vertexLoop.append(stitchRowPts[u+1][v+1]);
					vertexLoop.append(stitchRowPts[u+1][ v ]);
					
					// create output subface
					SubFace subface(2, 2, subfaceId);
					subface.bkwdPoints.clear();
					subface.fwrdPoints.clear();
					subface.bkwdPoints.append(stitchRowPts[ u ][ v ]);
					subface.bkwdPoints.append(stitchRowPts[ u ][v+1]);
					subface.fwrdPoints.append(stitchRowPts[u+1][ v ]);
					subface.fwrdPoints.append(stitchRowPts[u+1][v+1]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);

					// add polygon to mesh
					subfaceId++;
					outputMeshFn.addPolygon(vertexLoop, IDRef, true, 1.0e-10, MObject::kNullObj, NULL);
				}

				// backward edge has more points
				if (numPts1 > numPts2) {
					vertexLoop.clear();
					for (int v = numPts2-1; v < numPts1; v++)
						vertexLoop.append(stitchRowPts[u][v]);
					vertexLoop.append(stitchRowPts[u+1][numPts2-1]);
					
					// create output subface
					SubFace subface(2, 1, subfaceId);
					for (int v = numPts2-1; v < numPts1; v++)
						subface.bkwdPoints.append(stitchRowPts[u][v]);
					subface.fwrdPoints.append(stitchRowPts[u+1][numPts2-1]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);
					
					// add polygon to mesh
					subfaceId++;
					outputMeshFn.addPolygon(vertexLoop, IDRef, true, 1.0e-10, MObject::kNullObj, NULL);
				}

				// forward edge has more points
				else if (numPts2 > numPts1) {
					vertexLoop.clear();
					vertexLoop.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts2-1; v >= numPts1-1; v--)
						vertexLoop.append(stitchRowPts[u+1][v]);
					
					// create output subface
					SubFace subface(1, 2, subfaceId);
					subface.bkwdPoints.append(stitchRowPts[u][numPts1-1]);
					for (int v = numPts1; v < numPts2; v++)
						subface.fwrdPoints.append(stitchRowPts[u+1][v]);
					subface.stitchType = P;
					MSubFaces.push_back(subface);
					
					// add polygon to mesh
					subfaceId++;
					outputMeshFn.addPolygon(vertexLoop, IDRef, true, 1.0e-10, MObject::kNullObj, NULL);
				}
			} 
		} // per PolyMeshFace loop
	} // per PolyMeshFaceLoop loop
	
	for (int i = 0; i < inputMeshFn->numPolygons(); i++)
		outputMeshFn.deleteFace(0);
	outputMeshFn.updateSurface();
	return MStatus::kSuccess;
}

MStatus QuickKnitNode::GenerateStitchCurves(float stitchSize)
{
	for (int i = 0; i < MSubFaces.size(); i++)
	{
		// determine correct stitch type to generate
		SubFace subface = MSubFaces[i];
		Stitch stitch = stitches[subface.stitchType];		

		// create face specific cage from subface vertices
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

MStatus QuickKnitNode::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug == attr_outMesh) {
	
		// get object names for input/output
		MDataHandle inputMeshNameHandle  = data.inputValue(inputMeshName);
		MDataHandle outputMeshNameHandle = data.inputValue(outputMeshName);
		inputShapeName  = inputMeshNameHandle.asString();
		outputShapeName = outputMeshNameHandle.asString();

		// Get default stitch size
		MDataHandle stitchSizeHandle = data.inputValue(attr_stitchSize);
		float stitchSize = stitchSizeHandle.asFloat();

		// Get stitch generation stage
		MDataHandle nodeStageHandle = data.inputValue(attr_nodeStage);
		int nodeStage = nodeStageHandle.asInt();

		// Get input mesh object
		MDataHandle inMeshHandle = data.inputValue(attr_inMesh);
		MObject inputMesh = inMeshHandle.asMesh();

		// delete pointers to old input function sets
		if (inputMeshFn	  != NULL) delete inputMeshFn;
		inputMeshFn	  = new MFnMesh(inputMesh);
		inputMeshFn->copy(inputMesh, inputMeshObj);

		// get handle to output attribute
		MDataHandle outMeshHandle = data.outputValue(attr_outMesh);
		MObject outputMesh  = MFnMeshData().create();
		MFnMesh outputMeshFn  = MFnMesh().copy(inputMesh, outputMesh);
		outputMeshFn.setObject(outputMesh);

		// initialize variables
		if (callbackId == -1) {
			for (int i = 0; i < inputMeshFn->numPolygons(); i++) {
				faceLoopIndex.append(-1);
				faceLoopNumber.append(-1);
			}
		}
		
		switch (nodeStage) {

			// User interaction stage for selecting edge loops in direction of stitch

		case (LOOP_SELECTION):
			MGlobal::executeCommand("selectType -pe 1");
			break;

			// Tessellate mesh according to stitch size
		case (TESSELLATION):
			if (numLoopFaces == inputMeshFn->numPolygons() && nodeStage) {
				TessellateInputMesh(stitchSize, outputMeshFn);
				outMeshHandle.set(outputMesh);
				if (oMeshFnShape == NULL) oMeshFnShape = new MFnMesh;
				oMeshFnShape->copy(outputMesh, outputMeshObj);
			}
			MGlobal::executeCommand("select " + this->name());
			break;

			// Replace tessellated faces with stitch models			
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
			GenerateStitchCurves(1.0 / maxEdgeLength);
			MGlobal::executeCommand(MString("hide ") + outputShapeName);
			MGlobal::executeCommand("select " + this->name());
			break; }

		// Default switch case; should never reach this
		default:
			break;
		}

		data.setClean( plug );
		return MStatus::kSuccess;
	} 
	else return MS::kUnknownParameter;
}
