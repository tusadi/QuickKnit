#include "StitchLevelEdits.h"
#include "QuickKnitNode.h"

ChangeStitchTypeCmd::ChangeStitchTypeCmd() : MPxCommand()
{
}

ChangeStitchTypeCmd::~ChangeStitchTypeCmd() 
{
}

MStatus ChangeStitchTypeCmd::doIt( const MArgList& args )
{
	MString nodeName = "node";
	int faceIndex = 0, stitchType = 0;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kFaceIndexFlag))
		faceIndex = argData.flagArgumentString(kFaceIndexFlag, 0).asInt();
	if (argData.isFlagSet(kStitchTypeFlag))
		stitchType = argData.flagArgumentString(kStitchTypeFlag, 0).asInt();
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());

	return MStatus::kSuccess;
}

MSyntax ChangeStitchTypeCmd::syntaxCreator()
{
    MSyntax syntax;
	syntax.addFlag(kFaceIndexFlag,	kFaceIndexLongFlag,  MSyntax::kUnsigned);
	syntax.addFlag(kStitchTypeFlag,	kStitchTypeFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kNodeNameFlag,   kNodeNameFlagLong,   MSyntax::kString  );
    return syntax;
}   

