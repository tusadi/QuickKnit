#include "mayaIncludes.h"
#include "QKCmd.h"

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