#include "MayaIncludes.h"
#include "QKCmd.h"
#include "Node.h"
#define NO_WARN_MBCS_MFC_DEPRECATION

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

	//Register Node 
	status = plugin.registerNode("LSystemNode", Node::NodeID, Node::creator, Node::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	// You can run a MEL script from initializePlugin() to auto-register your MEL dialog boxes and menus
	char buffer[2048];
	sprintf_s(buffer, 2048, "source \"%s/stitchMesh\";", plugin.loadPath().asChar());
	MGlobal::executeCommand(buffer, true);
	return status;
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
