#include <stdarg.h>
#include <string>
#include "android_wstring.h"

enum fmt_arg_type
{
    fat_none,
    fat_float,
    fat_interger,
    fat_pointer,
    fat_char,
};

int xfind_fmt_tag(std::string& _fmt , const wchar_t *_buf , fmt_arg_type& _type)
{
     const wchar_t* _base = _buf;
     _type = fat_none;
     //prefix
     if(*_buf == '-' || *_buf == '+' || *_buf == ' ' || *_buf == '0' || *_buf == '#'  )
     {
         _fmt += (char)(*_buf);
         _buf ++;
     }

     //width
     if(*_buf <='9' && *_buf >= '0' )
     {
         _fmt += (char)(*_buf);
         _buf ++;
     }

     //.
     if(*_buf =='.')
     {
         _fmt += (char)(*_buf);
         _buf ++;
     }

     //precision 
     if(*_buf <='9' && *_buf >= '0' )
     {
         _fmt += (char)(*_buf);
         _buf ++;
     }

     //[{h | l | I64 | L}]
     if(*_buf == 'I' || *_buf == 'l' || *_buf == 'h' || *_buf == 'L' )
     {
         _fmt += (char)(*_buf);
         _buf ++;
     }
     else if(*_buf == 'c' || *_buf == 'C')
     {
         _type = fat_char;
     }     
     else if(*_buf == 'd' || *_buf == 'i' || *_buf == 'o' || *_buf == 'u' || *_buf == 'x' || *_buf == 'X' || *_buf == 'e' || *_buf == 'E')
     {
         _type = fat_interger;
     }
     else if( *_buf == 'f' || *_buf == 'g' || *_buf == 'G' || *_buf == 'n' || *_buf == 'p'  )
     {
         _type = fat_float;
     }
     else if( *_buf == 'n' || *_buf == 'p'  )
     {
         _type = fat_pointer;
     }

     if(_type != fat_none)
     {
         _fmt += (char)(*_buf);
         _buf ++;
         return _buf - _base;
     }

     return 0;
     
}

#define GUARD_COUNT  {idx ++ ;  if(idx >= ( (int)count - 1) ) { _out[idx] = 0 ; return idx; } }
int vsnwprintf_android(wchar_t* _out , size_t count , const wchar_t * wcsfmtStr , va_list ap)
{
	int   idx              = 0;
    wchar_t* _ret          = _out;
    const wchar_t *_fmtStr = wcsfmtStr;
    while(*_fmtStr!=0)
    {
        if(*_fmtStr!='%')
        {
            *_ret = *_fmtStr;
            _ret ++;
            GUARD_COUNT(idx);
             _fmtStr++;
        }
        else
        {
            switch(*(++_fmtStr))
            {
            case 's':
            case 'S':
                {
                    const wchar_t* wcsValue = va_arg(ap , const wchar_t*);
                    while( *wcsValue)
                    {
                        *_ret = *wcsValue;
                        wcsValue ++;
                        _ret ++;
                        GUARD_COUNT(idx);
                    }
                     _fmtStr++;
                }
                break;
            default:
                {
                    std::string fmtStr = "%";
                    fmt_arg_type _type;
                    int nSkip = xfind_fmt_tag(fmtStr , _fmtStr , _type);
                    if(nSkip > 0)
                    {
                        char ansiBuf[128] = {0};
                        switch(_type)
                        {
                        case fat_char:
                            {
                                char n = (char)va_arg(ap , int);
                                sprintf(ansiBuf , fmtStr.c_str() , n ); 
                            }
                            break;
                        case fat_interger:
                            {
                                int n = va_arg(ap , int);
                                sprintf(ansiBuf , fmtStr.c_str() , n ); 
                            }
                            break;
                        case fat_float:
                            {
                                float n = va_arg(ap , double);
                                sprintf(ansiBuf , fmtStr.c_str() , n ); 
                            }
                            break;
                        case fat_pointer:
                            {
                                void* n = va_arg(ap , void*);
                                sprintf(ansiBuf , fmtStr.c_str() , n ); 
                            }
                            break;
                        }
                        int i = 0;
                        while(ansiBuf[i])
                        {
                            *_ret = ansiBuf[i];
                            _ret ++; i ++;
                            GUARD_COUNT(idx);
                        }

                        _fmtStr += nSkip;
                    }
                    else
                    {
                        *_ret = *_fmtStr;
                        _ret ++;
                        _fmtStr++;
                        GUARD_COUNT(idx);
                    }
                }
            }
        }
       
    }

	_out[idx] = 0;
    return idx;
}


int swprintf_x(wchar_t* _out , size_t count , const wchar_t *buf , ...)
{
    va_list ap;
    va_start(ap,buf);
    int _ret = vsnwprintf_android(_out , count , buf , ap);
    va_end(ap);
    return _ret;
}