#include "StitchLevelEdits.h"
#include "StitchMeshNode.h"

ChangeStitchTypeCmd::ChangeStitchTypeCmd() : MPxCommand()
{
}

ChangeStitchTypeCmd::~ChangeStitchTypeCmd() 
{
}

MStatus ChangeStitchTypeCmd::doIt( const MArgList& args )
{
    cout<<"Successfully Called Function!"<<endl;

	// Setup Argument Database
	MString nodeName = "poop";
	int faceIndex = 0, stitchType = 0;
	MArgDatabase argData = MArgDatabase(syntax(), args);
	if (argData.isFlagSet(kFaceIndexFlag))
		faceIndex = argData.flagArgumentString(kFaceIndexFlag, 0).asInt();
	if (argData.isFlagSet(kStitchTypeFlag))
		stitchType = argData.flagArgumentString(kStitchTypeFlag, 0).asInt();
	if (argData.isFlagSet(kNodeNameFlag))
		nodeName = MString(argData.flagArgumentString(kNodeNameFlag, 0).asChar());


	cout << "faceIndex = " << faceIndex << endl;
	cout << "stitchType = " << stitchType << endl;
	cout << "nodeName = " << nodeName << endl;
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

