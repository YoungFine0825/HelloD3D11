#include "FrameData.h"

namespace Framework
{
	FrameData::FrameData()
	{

	}

	FrameData::~FrameData()
	{

	}

	void FrameData::Reset() 
	{
		cameras.clear();
		renderers.clear();
		lights.clear();
	}
}