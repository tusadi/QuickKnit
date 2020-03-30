#pragma once
#include "MayaIncludes.h"

class QKCmd : public MPxCommand
{
public:

	QKCmd();
	virtual ~QKCmd();

	static void* creator()
	{
		return new QKCmd();
	}

	virtual MStatus doIt(const MArgList& args);

	static MSyntax newSyntax();
};

