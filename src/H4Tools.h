/*
Creative Commons: Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode

You are free to:

Share — copy and redistribute the material in any medium or format
Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms. Under the following terms:

Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions 
under the same license as the original.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others 
from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use is 
permitted by an applicable exception or limitation. To discuss an exception, contact the author:

philbowles2012@gmail.com

No warranties are given. The license may not give you all of the permissions necessary for your intended use. 
For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
*/

#pragma once
#ifdef EMBEDDED_PLATFORM
#include<Arduino.h>
#endif
#include"h4tools_config.h"

#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<functional>
#define CSTR(x) x.c_str()


#include"mbx.h"

#define RECORD_SEPARATOR "|"
#define UNIT_SEPARATOR "~"

using H4T_HEAP_LIMITS   = std::pair<size_t,size_t>;
using H4T_FILE_HANDLER  = std::function<void(const char*,size_t n)>;
using H4T_NVP_MAP       = std::unordered_map<std::string,std::string>;
using H4T_VS            = std::vector<std::string>;
using H4T_FN_RFC_START  = std::function<void(size_t)>;
using H4T_FN_RFC_CHUNK  = std::function<void(const uint8_t*,size_t)>;
using H4T_FN_RFC_END    = std::function<void(void)>;
using H4T_FN_LOOKUP     = std::function<std::string(const std::string&)>;

#if (defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)) && !defined(EMBEDDED_PLATFORM)
#define EMBEDDED_PLATFORM
#endif

#ifdef EMBEDDED_PLATFORM

#ifdef ESP32
#define H4T_DEFAULT_CAPS    MALLOC_CAP_DEFAULT
#else
#define H4T_DEFAULT_CAPS    0
#endif

#include<LittleFS.h>
#define HAL_FS LittleFS
void            _HAL_analogFrequency(uint8_t pin,size_t f=H4T_PWM_DEFAULT);
void            _HAL_analogWrite(uint8_t pin, uint32_t value);
void            _HAL_attachAnalogPin(uint8_t pin);
void            _HAL_detachAnalogPin(uint8_t pin);
void            _HAL_feedWatchdog();
uint32_t        _HAL_freeHeap(uint32_t caps=H4T_DEFAULT_CAPS);
bool            _HAL_isAnalogInput(uint8_t p);
bool            _HAL_isAnalogOutput(uint8_t p);
uint32_t        _HAL_maxHeapBlock(uint32_t caps=H4T_DEFAULT_CAPS);
uint32_t        _HAL_minHeapBlock(uint32_t caps=H4T_DEFAULT_CAPS);
std::string     _HAL_uniqueName(const std::string& prefix);
std::string     _HAL_macAddress();

#if H4T_DEBUG
void            dumpvs(const H4T_VS& vs);
void            dumpnvp(const H4T_NVP_MAP& ms);
#endif
void                    dumphex(const uint8_t* mem, size_t len);
H4T_HEAP_LIMITS         heapLimits();
namespace h4t{
std::string             readFile(const char* path);
void                    readFileChunks(const char* path,size_t chunk,H4T_FN_RFC_CHUNK fc,H4T_FN_RFC_START fs=nullptr,H4T_FN_RFC_END fe=nullptr);
size_t                  writeFile(const char* fn,const std::string& data,const char* mode="w");
}

size_t inline       getMaxPayloadSize(){ return (_HAL_maxHeapBlock() - H4T_HEAP_SAFETY) / 2 ; }
#endif




std::string             encodeUTF8(const std::string &);
uint32_t                hex2uint(const uint8_t* str);
std::string 		    join(const H4T_VS& vs,const char* delim="\n");
std::unordered_map<std::string,std::string> json2nvp(const std::string& s);
std::string             lowercase(std::string);
std::string             ltrim(const std::string& s, const char d=' ');
std::string             nvp2json(const std::unordered_map<std::string,std::string>& nvp);

std::string             replaceAll(const std::string& s,const std::string& f,const std::string& r);
std::string             replaceParams(const std::string& s,H4T_FN_LOOKUP f);
std::string             replaceParams(const std::string& s,H4T_NVP_MAP& nvp);
std::string             rtrim(const std::string& s, const char d=' ');
H4T_VS                  split(const std::string& s, const char* delimiter="\n");
std::string		        stringFromInt(int i,const char* fmt="%d");
bool		            stringIsAlpha(const std::string& s);
bool		            stringIsNumeric(const std::string& s);
std::string             trim(const std::string& s, const char d=' ');
std::string             uppercase(std::string);
std::string             urldecode(const std::string &s);
std::string             urlencode(const std::string &s);
//
template<typename T>
std::string flattenMap(const T& m,const std::string& fs=UNIT_SEPARATOR,const std::string& rs=RECORD_SEPARATOR,std::function<std::string(const std::string&)> f=[](const std::string& s){ return s; }){
    std::string flat;
    for(auto const& nvp:m) flat+=f(nvp.first)+fs+f(nvp.second)+rs;
    flat.pop_back();
    return flat;
}