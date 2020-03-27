#include "QKCmd.h"
QKCmd::QKCmd() : MPxCommand()
{}

QKCmd::~QKCmd()
{}

MStatus QKCmd::doIt(const MArgList& args)
{
	MGlobal::executeCommand("print(\"Hello Maya\");");
	return MStatus::kSuccess;
}
MSyntax QKCmd::newSyntax()
{
	MSyntax syntax;
	//TODO: Add Flags here if you want  
	return syntax;
}
