
#include "tools.h"
#include "map.h"

namespace luc {

map_si		g_map_append_files;			//map from filename to FILE*
sync_obj	g_append_files_lock;

bool quick_append_file(char* data, int len, char* file_name)
{
	FILE* file = NULL;

	lockit lock(&g_append_files_lock);

	if (!g_map_append_files.find(file_name, (int&)file))
	{
		file = fopen(file_name, "ab+");
		g_map_append_files.set(file_name, (int)file);
	}

	if (file == NULL)
	{
		assert(false);
		return false;
	}

	int ret = fwrite(data, len, 1, file);
	fflush(file);

	return (ret == 1);
}

int readfile(char* filename, char* output_buf, int output_buf_size)
{
	int ret = 0;
	FILE* file = fopen(filename, "rb");
	if (file != NULL)
	{
		ret = 	fread(output_buf, output_buf_size-1, 1, file);
		fclose(file);
	}
	output_buf[ret] = 0;
	return ret;
}

void strrpl(char* buf, char old_char, char new_char)
{
	while (*buf)
	{
		if (*buf == old_char)
		{
			*buf = new_char;
		}
		buf++;
	}
}

int safe_copy(char* dst, int dst_size, char* src)
{
	if (dst == NULL || dst_size <= 1)
	{
		assert(false);
		return 0;
	}

	char* temp = dst;
	int max_len = dst_size-1;

	int len = 0;
	while (*src)
	{
		*temp = *src;
		len++;
		if (len == max_len)
		{
			break;
		}
		src++;
		temp++;
	}
	dst[len] = 0;

	return len;
}

int my_itoa(int val, char* buf)
{
	const unsigned int radix = 10;

	char* p;
	unsigned int a;        //every digit
	int len;
	char* b;            //start of the digit char
	char temp;
	unsigned int u;

	p = buf;

	if (val < 0)
	{
		*p++ = '-';
		val = 0 - val;
	}
	u = (unsigned int)val;

	b = p;

	do
	{
		a = u % radix;
		u /= radix;

		*p++ = a + '0';

	} while (u > 0);

	len = (int)(p - buf);

	*p-- = 0;

	//swap
	do
	{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;

	} while (b < p);

	return len;
}

int easy_split(char* src, char* dst_buf, int dst_unit_count, int dst_unit_len, char splitter, bool ignore_empty_subs/*=true*/)
{
	int count = 0;
	for (int i = 0; i < dst_unit_count; i++)
	{
		dst_buf[dst_unit_len*i] = '\0';
	}

	char* cur = src;
	while (count < dst_unit_count)
	{
		char* next = strchr(cur, splitter);
		if (next != NULL)
		{
			*next = '\0';
		}

		if (ignore_empty_subs && (*cur == '\0'))
		{
			//ignore empty sub string
		}
		else
		{
			safe_copy(dst_buf + dst_unit_len*count, dst_unit_len, cur);
			count++;
		}

		if (next != NULL)
		{
			*next = splitter;
			cur = next + 1;
		}
		else
		{
			break;
		}
	}
	return count;
}

void trim_left(char* str)
{
	int len = strlen(str);
	int count = 0;
	while (count < len && str[count] <= ' ')
	{
		count++;
	}
	memmove(str, str+count, len-count);
	str[len-count] = 0;
}

void trim_right(char* str)
{
	int len = strlen(str);
	while (len >= 0 && str[len-1] <= ' ')
	{
		len--;
	}
	str[len] = 0;
}

void trim(char* str)
{
	trim_left(str);
	trim_right(str);
}

void get_time( SYSTEMTIME* t, bool local/*=true*/ )
{
#ifdef _WIN32
	if (local)
	{
		GetLocalTime(t);
	}
	else
	{
		GetSystemTime(t);
	}
#else
	//linux
	get_time_of_day();
#endif
}

void get_module_file_name(char* buf, int buf_size)
{
#ifdef _WIN32
	GetModuleFileName(NULL, buf, buf_size);
#else
//see my blog for linux
#error "not implemented yet"
#endif
}

void get_module_path(char* buf, int buf_size)
{
	char full[1024];
	get_module_file_name(full, sizeof(full));
	strrpl(full, '\\', '/');

	char* last = strrchr(full, '/');
	*(last+1) = 0;

	safe_copy(buf, buf_size, full);
}

void get_module_name_short(char* buf, int buf_size)
{
	char full[1024];
	get_module_file_name(full, sizeof(full));
	strrpl(full, '\\', '/');

	char* last = strrchr(full, '/');
	last++;
	char* last_pt = strrchr(last, '.');
	*last_pt = 0;

	safe_copy(buf, buf_size, last);
}

void get_data_string(char* buf, int buf_size)
{
	if (buf_size < 20)
	{
		err("buffer too small");
		assert(false);
	}

	SYSTEMTIME t;
	get_time(&t);

	sprintf(buf, "%d%02d%02d", t.wYear, t.wMonth, t.wDay);
}

void get_time_string(char* buf, int buf_size)
{
	if (buf_size < 20)
	{
		err("buffer too small");
		assert(false);
	}

	SYSTEMTIME t;
	get_time(&t);

	sprintf(buf, "%02d:%02d:%02d.%03d", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
}

//like "20100517 13:15:24.234 - test msg\r\n"
void write_info_with_time(char* data, char* file_name)
{
	char date[100];
	get_data_string(date, sizeof(date));
	char time[100];
	get_time_string(time, sizeof(time));

	char buf[10240];
	int len = sprintf(buf, "%s %s - %.8000s", date, time, data);

	//append "\r\n" if necessary
	if (buf[len-1] != '\n')
	{
#ifdef _WIN32
		buf[len++] = '\r';
#endif

		buf[len++] = '\n';
		buf[len] = 0;
	}

	quick_append_file(buf, len, file_name);
}

void write_err_log(char* info)
{
	char path[1024];
	get_module_path(path, sizeof(path));
	char name[100];
	get_module_name_short(name, sizeof(name));
	char date[100];
	get_data_string(date, sizeof(date));

	char full[1024];
	sprintf(full, "%s/%s_%s_error.log", path, name, date);

	write_info_with_time(info, full);
}

void write_normal_log(char* info)
{
	char path[1024];
	get_module_path(path, sizeof(path));
	char name[100];
	get_module_name_short(name, sizeof(name));
	char date[100];
	get_data_string(date, sizeof(date));

	char full[1024];
	sprintf(full, "%s/%s_%s_normal.log", path, name, date);

	write_info_with_time(info, full);
}

void _err(char* info, char* file, int line)
{
	char buf[10240];
	sprintf(buf, "%.8000s (file:%.1000s, line:%d)", info, file, line);
	write_err_log(buf);
}

double get_milli_seconds()
{
	static bool first = true;
	static LARGE_INTEGER freq;
	if (first)
	{
		QueryPerformanceFrequency(&freq);
		first = false;
	}

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	double milli = (double)now.QuadPart/freq.QuadPart;
	milli *= 1000;
	return milli;
}

void set_non_block(SOCKET sock)
{
#ifdef _WIN32
	u_long one = 1;
	ioctlsocket(sock, FIONBIO, &one);
#else
	int flags = fcntl(m_sock, F_GETFL, 0);
	fcntl(m_sock, F_SETFL, flags|O_NONBLOCK);
#endif
}

}
