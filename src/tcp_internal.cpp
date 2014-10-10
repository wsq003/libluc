
#include "tcp_internal.h"
#include "tools.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// quick_buf

quick_buf::quick_buf(int size /*= 8192*/)
{
	if (size < 1024)
	{
		err("buffer should not be too small");
		assert(false);
	}

	_buf = new char[size];
	_size = size;
	_offset = 0;
	_len = 0;
}

quick_buf::~quick_buf()
{
	if (_buf)
	{
		delete _buf;
		_offset = 0;
		_len = 0;
	}
}

bool quick_buf::put_data(char* data, int len)
{
	if (data == NULL || len <= 0)
	{
		return false;
	}

	if (cur_free_space() < len)
	{
		return false;
	}

	memcpy(_buf + _offset, data, len);
	_len += len;

	return true;
}

void quick_buf::peek_data(char*& data, int& len)
{
	data = _buf + _offset;
	len = _len;
}

bool quick_buf::remove_data(int len)
{
	if (len <= 0)
	{
		return false;
	}
	if (len > len)
	{
		return false;
	}

	_len -= len;
	_offset += len;

	try_truncate();

	return true;
}

int quick_buf::cur_free_space()
{
	int free = _size - _offset - _len;
	return free;
}

int quick_buf::data_len()
{
	return _len;
}

void quick_buf::try_truncate()
{
	if (_len == 0)
	{
		_offset = 0;
		return;
	}

	if (_len > _size/4)
	{
		return;
	}
	if (_offset < _size/2)
	{
		return;
	}

	//now truncate
	memcpy(_buf, _buf+_offset, _len);
	_offset = 0;
	_buf[_len] = 0;
}

}
