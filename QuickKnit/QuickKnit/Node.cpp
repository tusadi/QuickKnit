#pragma once
#include "Node.h"

MTypeId Node::NodeID(0x80000);
MObject Node::MOBJ_ipMesh;
MObject Node::MOBJ_opMesh;
MObject Node::MOBJ_stitchSize;
MObject Node::MOBJ_ipMeshName;
MObject Node::MOBJ_opMeshName;

Node::~Node()
{}


void* Node::creator()
{
	Node* node = new Node();
	node->MOBJ_ipMesh = MFnMeshData().create();
	node->MOBJ_opMesh = MFnMeshData().create();
	return node;
}

MStatus	Node::initialize()
{
	MStatus returnStatus;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	//1) Stitch Size 
	Node::MOBJ_stitchSize = nAttr.create("stitchSize", "ss", MFnNumericData::kFloat, 1.0, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating Node::MOBJ_stitchSize attribute\n");
	nAttr.setMin(1.0);
	nAttr.setMax(10.0);
	nAttr.setWritable(true);
	//2)Input Mesh 
	Node::MOBJ_ipMesh = tAttr.create("inputMesh", "inMesh", MFnData::kMesh, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating Node::MOBJ_ipMesh attribute\n");
	//3)Output Mesh 
	Node::MOBJ_opMesh = tAttr.create("outputMesh", "outMesh", MFnData::kMesh, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating Node::MOBJ_opMesh attribute\n");
	tAttr.setStorable(false);
	//4)Input Mesh Name 
	Node::MOBJ_ipMeshName = tAttr.create("inputMeshName", "inMeshName", MFnData::kString, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating Node::MOBJ_ipMeshName attribute\n");	
	//5)Output Mesh Name 
	Node::MOBJ_opMeshName = tAttr.create("outputMeshName", "outMeshName", MFnData::kString, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating Node::MOBJ_opMeshName attribute\n");

	//Add Attribute and set AttributeAffects 
	returnStatus = addAttribute(Node::MOBJ_stitchSize);
	returnStatus = attributeAffects(Node::MOBJ_stitchSize, Node::MOBJ_opMesh);
	returnStatus = addAttribute(Node::MOBJ_ipMesh);
	returnStatus = attributeAffects(Node::MOBJ_ipMesh, Node::MOBJ_opMesh);
	returnStatus = addAttribute(Node::MOBJ_opMesh);

	return returnStatus;
}

//Compute is overridden by all child nodes 
//This function recomputes the given output based on the nodes inputs
//plug represents the output that needs to be recomputed 
//Data block is the block containing storage for the node's attributes
MStatus Node::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug == MOBJ_opMesh)
	{
		//Data handles 
		MDataHandle ipMeshHandle = data.inputValue(MOBJ_ipMesh);
		MDataHandle outMeshHandle = data.outputValue(MOBJ_opMesh);
		MDataHandle inputMeshNameHandle = data.inputValue(MOBJ_ipMeshName);
		MDataHandle outputMeshNameHandle = data.inputValue(MOBJ_opMeshName);
		MDataHandle stitchSizeHandle = data.inputValue(MOBJ_stitchSize);
		
		//Get values from handles 
		MString ipName = inputMeshNameHandle.asString();
		MString opName = outputMeshNameHandle.asString();	
		float stitchSize = stitchSizeHandle.asFloat();	
		MObject inputMesh = ipMeshHandle.asMesh();

		//1) Get the selected indices - If useful (or) delete it 

		MSelectionList selected;                    //holds the list of currently selected items 
		MGlobal::getActiveSelectionList(selected);  //returns the MObjects selected 
		MIntArray selectedIndices;                 //to store the indices of selected edges 
		MStringArray selectedEdges;                //to store the names of the selected edges - parsed to get indices 
		if (selected.length() == 0)
			goto p;
		selected.getSelectionStrings(selectedEdges);
		for (int i = 0; i < selected.length(); i++)
		{
			char *selectedName = (char *)selectedEdges[0].asChar();

			// split the name of the selection to
			// get the edge index
			char* pch;
			pch = strtok(selectedName, ".[]");
			std::vector<char*> selectedNameTokens;
			while (pch != NULL) {
				selectedNameTokens.push_back(pch);
				pch = strtok(NULL, ".[]");
			}

			// return the edge index
			if (selectedNameTokens.size() > 2 && strcmp(selectedNameTokens[1], "e") == 0) {
				selectedIndices[i] = atoi(selectedNameTokens[2]);
			}
		}
		/* for (int i = 0; i < selectedIndices.length(); i++)
		{
		std::cout << selectedIndices[i] << std::endl;
		} */
		
		//Delete the current selection 
		MGlobal::executeCommand("doDelete;"); 

		//To draw some geometry on the screen
		MGlobal::executeCommand("circle - c 0 0 0 - nr 0 1 0 - sw 360 - r 1");

		for (int i = 0; i < ipMesh->numPolygons(); i++)  //in case you want to loop over the faces of the input mesh, use this loop
		{
			//numPolygons is the number of faces in the input mesh 
		}

		//FYI (Some functions I came across that might be helpful)
		//1)addPolygon is a fn that adds a new polygon to this polygonal mesh. Return an index of the created polygon.
		//MObject addPolygon (const MPointArray &vertexArray, int &faceIndex, bool mergeVertices=true, 
		//double pointTolerance=1.0e-10, MObject parentOrOwner=MObject::kNullObj, MStatus *ReturnStatus=NULL)
		//or
		//2)MFnMesh.create(...) creates a new mesh object, check online for signature
		//3)Use MFnMesh.setFaceColor(MColor &color, int index) to set the face's color 

		//face-relative indices - they range between 0..(polygonVertexCount(faceId)-1)
		//just get them by calling ipMesh->polygonVertexCount
	
		//MItMeshPolygon is an iterator that lets you iterate over all the faces 
		//https://help.autodesk.com/view/MAYAUL/2016/ENU/?guid=__files_Polygon_API_The_five_basic_polygonal_API_classes_htm
		//for more info 

			//2) Tessellate 
		Tessellate();
			//3) Edit Faces 
		replaceMesh();
			//4) Relax Mesh 
		relax();
	p:
		data.setClean(plug);
		return MStatus::kSuccess;
	}
	else return MS::kUnknownParameter;
	
}
MStatus Node::doIt(const MArgList& args)
{
	MGlobal::displayInfo("QuickKnit Node!");
	return MStatus::kSuccess;
}

void Node::Tessellate()
{

}
void Node::replaceMesh()
{

}
void Node::relax()
{

}