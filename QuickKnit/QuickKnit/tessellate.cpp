
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tessellate.h"
#include "MayaIncludes.h"

/*
int ComputeNumWaleDivisions(PolyMeshFaceLoop &currentLoop)
{
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

		// get second wale edge only for final face
		// (assuming potential for unclosed loops)
		if (i == currentLoop.size() - 1) {
			currentFace.getWaleEdge2(vertices);
			inputMeshFn->getPoint(vertices[0], w0);
			inputMeshFn->getPoint(vertices[1], w1);
			totalLength += (w1 - w0).length();
		}
	}

	// divide total sum by number of edges
	double avgLength = totalLength / (currentLoop.size() + 1);
	int numWaleDivisions = ceil(avgLength / (stitchSize));
	return numWaleDivisions;
}



MStatus InterpolatePoints(vector<MPointArray> &stitchRowPts, PolyMeshFace &currentFace)
{
	// Determine the uniform number of wale divisions along the face loop
	PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[faceLoopNumber[currentFace.faceIndex]];
	int numWaleDivisions = ComputeNumWaleDivisions(currentLoop);

	// determine number of divisions along fwrd & bkwd course edges
	MPointArray pts;
	oMeshFnShape->getPoints(pts);
	int numCourseDivisionsBkwd = 0;
	int numCourseDivisionsFwrd = 0;
	float courseLengthBkwd = 0.0;
	float courseLengthFwrd = 0.0;

	MIntArray courseVerticesBkwd;
	currentFace.getCourseEdgeBkwd(courseVerticesBkwd);
	for (int i = 0; i < courseVerticesBkwd.length() - 1; i++) {
		MVector courseEdgeBkwd = pts[courseVerticesBkwd[i + 1]] - pts[courseVerticesBkwd[i]];
		numCourseDivisionsBkwd += ceil(courseEdgeBkwd.length() / (stitchSize));
		courseLengthBkwd += courseEdgeBkwd.length();
	}

	MIntArray courseVerticesFwrd;
	currentFace.getCourseEdgeFwrd(courseVerticesFwrd);
	for (int i = 0; i < courseVerticesFwrd.length() - 1; i++) {
		MVector courseEdgeFwrd = pts[courseVerticesFwrd[i + 1]] - pts[courseVerticesFwrd[i]];
		numCourseDivisionsFwrd += ceil(courseEdgeFwrd.length() / (stitchSize));
		courseLengthFwrd += courseEdgeFwrd.length();
	}

	// create mean-value coordinate instance for tessellation
	MPointArray cagePoints;
	currentFace.getCage(cagePoints, *oMeshFnShape);
	MVC mvc(cagePoints);

	// find and store all interpolated points
	for (int u = 0; u <= numWaleDivisions; u++)
	{
		double uPct = (double)u / numWaleDivisions;
		int numRowPts = LerpInt(numCourseDivisionsBkwd, numCourseDivisionsFwrd, uPct);

		for (int v = 0; v <= numRowPts; v++)
		{
			double vPct = (double)v / numRowPts;
			MVCWeights weights;
			int i;

			// determine backward weights
			i = 0;
			MVCWeights bkwdWeights;
			MPoint bkwdOrigin = pts[courseVerticesBkwd[0]];
			float bkwdDistance = vPct*courseLengthBkwd;
			while (bkwdDistance > 1.0e-3)
			{
				MVector courseEdgeBkwd = pts[courseVerticesBkwd[i + 1]] - pts[courseVerticesBkwd[i++]];
				float factor = std::fmin(courseEdgeBkwd.length(), bkwdDistance) / courseEdgeBkwd.length();
				bkwdOrigin += factor * courseEdgeBkwd;
				bkwdDistance -= factor * courseEdgeBkwd.length();
			}
			mvc.computeMVCWeights(bkwdWeights, bkwdOrigin);

			// determine forward weights
			i = 0;
			MVCWeights fwrdWeights;
			MPoint fwrdOrigin = pts[courseVerticesFwrd[0]];
			float fwrdDistance = vPct*courseLengthFwrd;
			while (fwrdDistance > 1.0e-3)
			{
				MVector courseEdgefwrd = pts[courseVerticesFwrd[i + 1]] - pts[courseVerticesFwrd[i++]];
				float factor = std::fmin(courseEdgefwrd.length(), fwrdDistance) / courseEdgefwrd.length();
				fwrdOrigin += factor * courseEdgefwrd;
				fwrdDistance -= factor * courseEdgefwrd.length();
			}
			mvc.computeMVCWeights(fwrdWeights, fwrdOrigin);

			weights.clear();
			for (int i = 0; i < courseVerticesBkwd.length(); i++)
				weights.append(bkwdWeights[i] * (1.0 - uPct));
			for (int i = courseVerticesBkwd.length(); i < cagePoints.length(); i++)
				weights.append(fwrdWeights[i] * uPct);

			MPoint pt;
			mvc.computeMVCPosition(weights, pt);
			stitchRowPts[u].append(pt);
		}
	}
	return MStatus::kSuccess;
}

int min(int a, int b) {
	return a > b ? b : a;
}

int max(int a, int b) {
	return a < b ? b : a;
}

MStatus BuildSubfaces(vector<MPointArray> &stitchRowPts)
{
	int subfaceId = 0;
	MPointArray vertexLoop;

	for (int u = 0; u < stitchRowPts.size() - 1; u++)
	{
		int numPts1 = stitchRowPts[u].length();
		int numPts2 = stitchRowPts[u + 1].length();
		int minRowPts = min(numPts1, numPts2);

		// use this number to add quad stitch faces
		for (int v = 0; v < minRowPts - 1; v++)
		{
			// clear current subface vertex loop
			vertexLoop.clear();

			// get points in counterclockwise order
			vertexLoop.append(stitchRowPts[u][v]);
			vertexLoop.append(stitchRowPts[u][v + 1]);
			vertexLoop.append(stitchRowPts[u + 1][v + 1]);
			vertexLoop.append(stitchRowPts[u + 1][v]);

			// create output subface
			SubFace subface(2, 2);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			subface.bkwdPoints.append(stitchRowPts[u][v]);
			subface.bkwdPoints.append(stitchRowPts[u][v + 1]);
			subface.fwrdPoints.append(stitchRowPts[u + 1][v]);
			subface.fwrdPoints.append(stitchRowPts[u + 1][v + 1]);
			subface.stitchType = P;
			subface.nBkwd = 2; subface.nFwrd = 2;
			MSubFaces.push_back(subface);

			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++, true, 1.0e-3);
		}

		// backward edge has more points
		if (numPts1 > numPts2) {
			vertexLoop.clear();
			for (int v = numPts2 - 1; v < numPts1; v++)
				vertexLoop.append(stitchRowPts[u][v]);
			vertexLoop.append(stitchRowPts[u + 1][numPts2 - 1]);

			// create output subface
			SubFace subface(2, 1);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			for (int v = numPts2 - 1; v < numPts1; v++)
				subface.bkwdPoints.append(stitchRowPts[u][v]);
			subface.fwrdPoints.append(stitchRowPts[u + 1][numPts2 - 1]);
			subface.stitchType = X;
			subface.nBkwd = 2; subface.nFwrd = 1;
			MSubFaces.push_back(subface);

			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++);
		}

		// forward edge has more points
		else if (numPts2 > numPts1) {
			vertexLoop.clear();
			vertexLoop.append(stitchRowPts[u][numPts1 - 1]);
			for (int v = numPts2 - 1; v >= numPts1 - 1; v--)
				vertexLoop.append(stitchRowPts[u + 1][v]);

			// create output subface
			SubFace subface(1, 2);
			subface.bkwdPoints.clear();
			subface.fwrdPoints.clear();
			subface.bkwdPoints.append(stitchRowPts[u][numPts1 - 1]);
			for (int v = numPts1 - 1; v < numPts2; v++)
				subface.fwrdPoints.append(stitchRowPts[u + 1][v]);
			subface.stitchType = Y1;
			subface.nBkwd = 1; subface.nFwrd = 2;
			MSubFaces.push_back(subface);

			// add polygon to mesh
			oMeshFnShape->addPolygon(vertexLoop, subfaceId++);
		}
	}
	return MStatus::kSuccess;
}

MStatus StitchMeshNode::TessellateInputMesh(void)
{
	MSubFaces.clear();
	int numPolyMeshFaceLoops = (int)MPolyMeshFaceLoops.size();

	for (int n = 0; n < numPolyMeshFaceLoops; n++)
	{
		PolyMeshFaceLoop currentLoop = MPolyMeshFaceLoops[n];
		int numWaleDivisions = ComputeNumWaleDivisions(currentLoop);

		for (int f = 0; f < currentLoop.size(); f++)
		{
			PolyMeshFace currentFace = currentLoop[f];

			// Create vector of MPointArrays to store interpolated points
			vector<MPointArray> stitchRowPts;
			stitchRowPts.resize(numWaleDivisions + 1);
			for (int i = 0; i < stitchRowPts.size(); i++) { stitchRowPts[i].clear(); }

			// Compute the interpolated points used for building tessellated faces
			InterpolatePoints(stitchRowPts, currentFace);

			// Add interior faces to polygon based on stored stitch row points
			BuildSubfaces(stitchRowPts);
		}
	}

	// delete original un-tesselated face
	for (int i = 0; i < inputMeshFn->numPolygons(); i++)
		oMeshFnShape->deleteFace(0);
	oMeshFnShape->updateSurface();

	return MStatus::kSuccess;
}*/