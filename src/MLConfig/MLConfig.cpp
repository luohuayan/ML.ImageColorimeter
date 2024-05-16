#include "MLConfig.h"
MLConfig* MLConfig::self = nullptr;
MLConfig::MLConfig()
{
}

MLConfig* MLConfig::instance()
{
	if (self == nullptr)
	{
		self = new MLConfig();
	}
	return self;
}