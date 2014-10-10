
#include "map.h"
#include "tools.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// qss

qss::qss(char* data)
{
	safe_copy(_data, sizeof(_data), data);
}

bool qss::operator<(const qss& other) const
{
	int ret = strcmp(this->_data, other._data);
	if (ret == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

}
