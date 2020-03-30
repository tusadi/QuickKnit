#include "mayaIncludes.h"
#include "QKCmd.h"
#include "glm\glm.hpp"
#include<vector>
#include <glm\glm.hpp>


MStatus initializePlugin(MObject obj)
{
	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

	// Register Command
	status = plugin.registerCommand("QuickKnit", QKCmd::creator, QKCmd::newSyntax);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj);

	status = plugin.deregisterCommand("QuickKnit");
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
}

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



std::vector<Face> test() {
	///User defined
	int nRows = 4;
	
	std::vector<glm::vec3> verts;
	std::vector<std::vector<glm::vec3>> rows;
	
	for (int i = 0; i < nRows + 1; i++) {
		 
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
	
	std::vector<Face> faces;

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
	
	return faces;
}