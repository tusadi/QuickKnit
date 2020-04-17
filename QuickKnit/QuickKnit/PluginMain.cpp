#include "MayaIncludes.h"
#include "QKCmd.h"
#include "Node.h"
#define NO_WARN_MBCS_MFC_DEPRECATION

MStatus initializePlugin(MObject obj)
{
	MStatus  status = MStatus::kSuccess;
	MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

	// Register Command
	status = plugin.registerCommand("QuickKnit", Node::creator);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	// Remove existing menu if it still exists
	MGlobal::executeCommand("if(`menu -ex StitchMeshesMenu`) deleteUI -m StitchMeshesMenu");

	// Add menu to main Maya window
	MGlobal::executeCommand("menu -l \"Stitch Meshes\" -p MayaWindow StitchMeshesMenu");

	// You can run a MEL script from initializePlugin() to auto-register your MEL dialog boxes and menus
	char buffer[2048];
	sprintf_s(buffer, 2048, "source \"%s/stitchMesh\";", plugin.loadPath().asChar());
	MGlobal::executeCommand(buffer, true);

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status = MStatus::kSuccess;
	MFnPlugin plugin(obj);

	status = plugin.deregisterCommand("QuickKnit");
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
}
