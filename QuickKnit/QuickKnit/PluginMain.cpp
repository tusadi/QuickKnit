#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "StitchMeshNode.h"
#include "StitchLevelEdits.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "QuickKnit", "1.0", "Any");
	
	// Register Command
    status = plugin.registerCommand( "ChangeStitchTypeCmd", ChangeStitchTypeCmd::creator, ChangeStitchTypeCmd::syntaxCreator );
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

	status = plugin.registerNode("StitchMeshNode", StitchMeshNode::id,
						 StitchMeshNode::creator, StitchMeshNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	
	MGlobal::executeCommand("if(`menu -ex StitchMeshesMenu`) deleteUI -m StitchMeshesMenu");

	MGlobal::executeCommand("menu -l \"Stitch Meshes\" -p MayaWindow StitchMeshesMenu");
	
	MString menuCmd2 = "menuItem -l \"Create Stitch Mesh Node\" -command \"source \\\"" + plugin.loadPath() + "/StitchMeshNode.mel\\\"\"";
	MGlobal::executeCommand(menuCmd2);


    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	status = plugin.deregisterNode(StitchMeshNode::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	MGlobal::executeCommand("if(`menu -ex StitchMeshesMenu`) deleteUI -m StitchMeshesMenu");

    return status;
}


