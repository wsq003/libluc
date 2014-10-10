
#pragma once

#include "platform.h"
#include "mem_pool.h"

namespace luc {

//quick_short_string for quick map string to int
//allocate quickly, construct quickly
//this class must kept to be POD for default copy_constructor
class qss
{
public:
	qss(char* data);
	bool operator < (const qss& other) const;

	DECLARE_MEM_POOL(qss)


private:
	//DISALLOW_COPY_AND_ASSIGN(qss);
	char _data[256];
};


//class TKEY must impl operator <
//class vAL must impl operator =
template <class TKEY, class TVAL>
class map_x
{
public:
	typedef typename map<TKEY, TVAL>::iterator iterator_x;

	map_x()
	{
	}
	~map_x()
	{
		_mm.clear();
	}

	bool add(TKEY key, TVAL val)
	{
		if (exist(key))
		{
			return false;
		}
		_mm[key] = val;
		return true;
	}

	void set(TKEY key, TVAL val)
	{
		_mm[key] = val;
	}

	bool exist(TKEY key)
	{
		map<int, int>::iterator it = _mm.find(key);
		if (it == _mm.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool find(TKEY key, TVAL& val)
	{
		map<TKEY, TVAL>::iterator it = _mm.find(key);
		if (it == _mm.end())
		{
			return false;
		}
		else
		{
			val = (*it).second;
			return true;
		}
	}

	void remove(TKEY key)
	{
		_mm.erase(key);
	}

	//use '_it' to go through
	void get_start_pos()
	{
		_it = _mm.begin();
	}

	bool get_next_assoc(TKEY& key, TVAL& val)
	{
		_it++;

		if (_it == _mm.end())
		{
			return false;
		}
		else
		{
			key = (*_it).first;
			val = (*_it).second;
			return true;
		}
	}

private:
	DISALLOW_COPY_AND_ASSIGN(map_x);

private:
	map<TKEY, TVAL> _mm;
	iterator_x _it;
};

typedef map_x<int, int> map_ii;		//map int to int
typedef map_x<qss, int> map_si;		//map quick_short_string to int

}
