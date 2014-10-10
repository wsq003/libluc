
#pragma once

#include "platform.h"

namespace luc {


#define err(x)	_err(x, __FILE__, __LINE__)

//keep the file opened, append to the file then fflush.
//this function is thread safe.
bool quick_append_file(char* data, int len, char* file_name);
//open file and read all data into output_buf.  max to (output_buf_size-1) bytes.
//last byte of output_buf is grantee to be zero for safty.
//return the bytes read.
int readfile(char* filename, char* output_buf, int output_buf_size);

//replace old_char by new_char
void strrpl(char* buf, char old_char, char new_char);
//safe copy, truncate if too long. ended with zero.
//return result length.
int safe_copy(char* dst, int dst_size, char* src);
//return the length of result string. support only 10 radix for easy use and better performance
int my_itoa(int val, char* buf);
//return count of the sub-strings. 
//sample:
//char buf[20][100];
//int count = easy_split("hello,world!", buf[0], 20, 100, ',');
int easy_split(char* src, char* dst_buf, int dst_unit_count, int dst_unit_len, char splitter, bool ignore_empty_subs=true);
//trim all those <= ' ' from left
void trim_left(char* str);
//trim all those <= ' ' from right
void trim_right(char* str);
//trim all those <= ' ' from left and right
void trim(char* str);

//fill the fields with human readable data, like 2010 year, 17th date, 5th month, etc
void get_time(SYSTEMTIME* t, bool local=true);

//return full path name like "d:\\my_app\\test.exe"
void get_module_file_name(char* buf, int buf_size);
//return like "d:\\my_app\\"
void get_module_path(char* buf, int buf_size);
//return like "test" for test.exe
void get_module_name_short(char* buf, int buf_size);

//return like "20100517" for 2010 May 17
void get_data_string(char* buf, int buf_size);
//return like "11:47:32.234"
void get_time_string(char* buf, int buf_size);

//write log to default log file, prefix with date-time
//'info' should not be longer than 8k, or will be truncated. "\r\n" will be auto add.
void write_err_log(char* info);
void write_normal_log(char* info);

//write err log, followed by code file name and code line
void _err(char* info, char* file, int line);

//return the milliseconds since one specific time.
//precision to microseconds.
double get_milli_seconds();

//
void set_non_block(SOCKET sock);


}
