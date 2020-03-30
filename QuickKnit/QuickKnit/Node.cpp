#pragma once
#include "Node.h"
#include <vector>
#include<glm\glm.hpp>
#include<maya\MItMeshPolygon.h>
#include<maya\MStateManager.h>
#include<string>
MTypeId Node::NodeID(0x80000);
MObject Node::MOBJ_ipMesh;
MObject Node::MOBJ_opMesh;
MObject Node::MOBJ_stitchSize;
MObject Node::MOBJ_ipMeshName;
MObject Node::MOBJ_opMeshName;

Node::~Node()
{}



class Face {
public:
	///In anti-clockwise order
	std::vector<glm::vec3> vertices;
	bool isLeftSided = false;

	Face() {};
	Face(std::vector<glm::vec3> verts) {
		for (int i = 0; i < verts.size(); i++) {
			vertices.push_back(verts[i]);
		}
	}
};

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
		
		MIntArray vertices;

		//for (int i = 0; i < ipMesh->numPolygons(); i++)  //in case you want to loop over the faces of the input mesh, use this loop
		//{
			//numPolygons is the number of faces in the input mesh 
			
		//}

		//2) Tessellate		///Done
		getTessellatedFaces(ipMesh->object);
		

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

MPointArray vertices;
float vertArray[4][4];
std::vector<Face> faces;

///TESSELLATION
//Face iterator function
void getTessellatedFaces(MObject mesh)
{
	MStatus status;
	// Reset the faceEdges array
	//
	// Initialize a face iterator and function set
	//
	MItMeshPolygon faceIter(mesh, &status);
	//MCheckStatus(status, "MItMeshPolygon constructor failed");
	MFnMesh meshFn(mesh, &status);
	//MCheckStatus(status, "MFnMesh constructor failed");

	// Now parse the mesh for the given face and
	// return the resulting faces
	//
	for (; !faceIter.isDone(); faceIter.next())
	{
		///Storing vertices of face
		//faceIter.getVertices(vertices);
		faceIter.getPoints(vertices, MSpace::kWorld);
		vertices.get(vertArray);
		generateFaces();
	}
	//return status;
}

//Face generator function
void generateFaces() {
	///User defined
	int nRows = 4;

	std::vector<glm::vec3> verts;
	std::vector<std::vector<glm::vec3>> rows;

	//Storing points in vertex
	for (int i = 0; i < sizeof(vertArray[0])/sizeof(vertArray[0][0]); i++) {
		verts.push_back(glm::vec3(vertArray[i][0], vertArray[i][1], vertArray[i][2]));
	}

	bool isQuad = false;
	bool isLeftSided = false;

	if (verts.size() == 4) {
		isQuad = true;
	}

	///Identify L, R
	//considering v0, v1 to be edge L and v2-v3 to be edge R
	//Identify T, B

	//Define vertices
	for (int i = 0; i < nRows + 1; i++) {
		///Insert first vertex
		if (i == 0) {
			rows[0].push_back(verts[0]);
		}
		else if (i == nRows) {
			rows[i].push_back(verts[1]);
		}
		else {
			glm::vec3 newVert = (i * 1.0f / nRows) * verts[0] + (1.0f - (i * 1.0f / nRows)) * verts[1];
			rows[i].push_back(newVert);
		}

		///Add intermediate vertices
		//Number of vertices in top row:
		float dist = glm::distance(verts[0], verts[1]) * 1.0f / nRows;
		int nCols = glm::distance(verts[0], verts[verts.size()]) / dist;

		glm::vec3 endVert = (i * 1.0f / nRows) * verts[verts.size() - 1] + (1.0f - (i * 1.0f / nRows)) * verts[verts.size() - 2];

		for (int j = 1; j < nCols; j++) {
			glm::vec3 vertex = (j * 1.0f / nCols) * verts[i][0] + (1 - (j * 1.0f / nCols)) * endVert;
			rows[i].push_back(vertex);
		}

		//Add last vertex
		rows[i].push_back(endVert);
	}


	//Form faces
	for (int i = 0; i < nRows - 1; i++) {
		for (int j = 0; j < rows[0].size() - 1; j++) {
			Face f;
			f.vertices.push_back(rows[i][j]);
			f.vertices.push_back(rows[i + 1][j]);
			f.vertices.push_back(rows[i + 1][j + 1]);
			f.vertices.push_back(rows[i][j + 1]);

			faces.push_back(f);
		}
	}

}


void Node::replaceMesh()
{
	int x = 5;
	for (int i = 0; i < faces.size(); i++) {

		MGlobal::executeCommand("file -import -type \"OBJ\"  -ignoreVersion -rpr \"stitchLoop\" -options \"mo = 1\" \"C: / Users / tpura / Desktop / CIS - 660 / QuickKnit / Knit - pattern units / stitchLoop.obj\";");
		MGlobal::executeCommand("select -r polySurface5;");
		MGlobal::executeCommand("duplicate -rr;");
		glm::vec3 median = glm::vec3(0);
		for (int j = 0; j < faces[i].vertices.size(); j++) {
			median += faces[i].vertices[j];
		}

		median *= 1.0f / faces[i].vertices.size();
		MString s1 = "move -r ";
		MString s5 = ",";
		MString s6 = ";";
		s1 += (median[0]);
		s1 += s5;	//,
		s1 += (median[1]);
		s1 += s5;	//,
		s1 += (median[2]);
		s1 += s6;	//;
		MGlobal::executeCommand(s1);
	}
}
void Node::relax()
{

}



