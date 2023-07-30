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
#include"H4Tools.h"

#ifdef EMBEDDED_PLATFORM
#if defined(ARDUINO_ARCH_ESP32)
    #include "esp_task_wdt.h"
    uint32_t         h4channel=0;
    std::unordered_map<uint8_t , uint8_t> h4channelmap; // pin, channel

    void        _HAL_attachAnalogPin(uint8_t pin){ // fix, lmit to 16!!!
        h4channelmap[pin]=h4channel;
        _HAL_analogFrequency(pin,H4T_PWM_DEFAULT); // set default f = PWM=0 =OFF
        ledcAttachPin(pin, h4channel);
        h4channel+=2;
    }
    void        _HAL_detachAnalogPin(uint8_t pin){
        if (h4channelmap.count(pin))
            ledcDetachPin(pin);
    }
    void        _HAL_analogFrequency(uint8_t pin,size_t f){ ledcSetup(h4channelmap[pin], f, 10); }
    
    void        _HAL_analogWrite(uint8_t pin,uint32_t f){ ledcWrite(h4channelmap[pin], f); }
    void        _HAL_feedWatchdog(){ esp_task_wdt_reset(); }
    uint32_t    _HAL_freeHeap(uint32_t caps){ return heap_caps_get_free_size(caps); }
    bool        _HAL_isAnalogInput(uint8_t p){
        std::vector<uint8_t> adc={5,8,10,11,12,13,14,15,16,17,18,20,21,22,23,24};
        return std::find(adc.begin(),adc.end(),p)!=adc.end();
    }
    bool        _HAL_isAnalogOutput(uint8_t p){ return h4channelmap.count(p); }
    uint32_t    _HAL_maxHeapBlock(uint32_t caps){ return heap_caps_get_largest_free_block(caps); }
    uint32_t    _HAL_minHeapBlock(uint32_t caps){ return heap_caps_get_minimum_free_size(caps); }
    std::string _HAL_uniqueName(const std::string& prefix){ return std::string(prefix).append(_HAL_macAddress()); }
    std::string _HAL_macAddress() { return stringFromInt((ESP.getEfuseMac() >> 24) & 0xFFFFFF,"%06X"); }
#else
    extern "C" {
        #include "user_interface.h" // what for???
    }
    void        _HAL_attachAnalogPin(uint8_t pin){}
    void        _HAL_analogFrequency(uint8_t pin,size_t f){ analogWriteFreq(f); }
    void        _HAL_analogWrite(uint8_t pin, uint32_t value){ analogWrite(pin,value); }
    void        _HAL_feedWatchdog(){ ESP.wdtFeed(); }
    uint32_t    _HAL_freeHeap(uint32_t caps){ return ESP.getFreeHeap(); }
    bool        _HAL_isAnalogInput(uint8_t p){ return p==A0; }
    bool        _HAL_isAnalogOutput(uint8_t p){         
        std::vector<uint8_t> adc={5,6,16,19};
        return std::find(adc.begin(),adc.end(),p)!=adc.end();
    }
    uint32_t    _HAL_maxHeapBlock(uint32_t caps){ return ESP.getMaxFreeBlockSize(); }
    uint32_t    _HAL_minHeapBlock(uint32_t caps){ return _HAL_maxHeapBlock(caps); }
    std::string _HAL_uniqueName(const std::string& prefix){ return std::string(prefix).append(_HAL_macAddress()); }
    std::string _HAL_macAddress() { return stringFromInt(ESP.getChipId(),"%06X"); }
#endif
//
//
//
#if H4T_DEBUG
void dumpvs(const H4T_VS& vs){ for(auto const& v:vs) Serial.printf("%s\n",v.data()); }
void dumpnvp(const std::map<std::string,std::string>& ms){ for(auto const& r:ms) Serial.printf("%s=%s\n",r.first.data(),r.second.data()); }
#endif
//
void dumphex(const uint8_t* mem, size_t len) {
    if(mem && len){ // no-crash sanity
        auto W=16;
        uint8_t* src;
        memcpy(&src,&mem,sizeof(uint8_t*));
        Serial.printf("Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
        for(uint32_t i = 0; i < len; i++) {
            if(i % W == 0) Serial.printf("\n[0x%08X] 0x%08X %5d:  ", (ptrdiff_t)src, i,i);
            Serial.printf("%02X ", *src);
            src++;
            //
            if(i % W == W-1 || src==mem+len){
                size_t ff=W-((src-mem) % W);
                for(int p=0;p<(ff % W);p++) Serial.print("   ");
                Serial.print("  "); // stretch this for nice alignment of final fragment
                for(uint8_t* j=src-(W-(ff%W));j<src;j++) Serial.printf("%c", isprint(*j) ? *j:'.');
                _HAL_feedWatchdog();
            }
            _HAL_feedWatchdog();
        }
        Serial.println();
    }
}
H4T_HEAP_LIMITS heapLimits(){
    H4T_HEAP_LIMITS hl;
    hl.first=(_HAL_freeHeap() * H4T_HEAP_CUTOUT_PC) / 100;
    hl.second=(_HAL_freeHeap() * H4T_HEAP_CUTIN_PC) / 100;
    return hl;
}

namespace h4t
{

std::string readFile(const char* fn){
	std::string rv="";
        File f=HAL_FS.open(fn, "r");
        if(f && f.size()) {
            int n=f.size();
            uint8_t* buff=(uint8_t *) malloc(n);
            f.readBytes((char*) buff,n);
            rv.assign((const char*) buff,n);
            free(buff);
        }
        f.close();
	return rv;	
}

void readFileChunks(const char* path,size_t chunk,H4T_FN_RFC_CHUNK fc,H4T_FN_RFC_START fs,H4T_FN_RFC_END fe){
    File f=HAL_FS.open(path, "r");
    if(f) {
      size_t lump=0;
      uint8_t* buff=static_cast<uint8_t*>(malloc(chunk));
      size_t bytesRemaining;
      size_t tot;
      tot=bytesRemaining=f.size();
      if(fs) fs(tot);
      while(bytesRemaining-=lump){
         lump=std::min(bytesRemaining,chunk);// < chunk ? bytesRemaining:chunk;
         f.readBytes((char*) buff,lump);
         fc(buff,lump);
      }
      f.close();
      free(buff);
      if(fe) fe();
    } else if(fs) fs(0);
}
size_t writeFile(const char* fn,const std::string& data,const char* mode){
    File b=HAL_FS.open(fn, mode);
    b.print(data.data());
    b.close();
    return data.size(); // fix this!!!!!!!!!!!!
}
}

#endif

uint32_t hex2uint(const uint8_t* str){
    size_t res = 0;
    uint8_t c;
    while (isxdigit(c=*str++)) {
        uint8_t v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8);
        res = (res << 4) | (size_t) v;
    }
    return res;
}

std::string join(const H4T_VS& vs,const char* delim) {
	std::string rv="";
	if(vs.size()){
		std::string sd(delim);
		for(auto const& v:vs) rv+=v+sd;
		for(int i=0;i<sd.size();i++) rv.pop_back();		
	}
	return rv;
}

//
std::string encodeUTF8(const std::string& s){
    std::string value(s);
    size_t u=value.find("\\u");
    while(u!=std::string::npos){
        uint32_t cp=hex2uint((const uint8_t*) &value[u+2]);
//        Serial.printf("value %s u=%d CODE POINT!!! %d 0x%06x\n",value.data(),u,cp,cp);
        uint8_t b0=cp&0x3f;
        uint8_t b1=(cp&0xfc0) >> 6;
        uint8_t b2=(cp&0xf000) >> 12;
//        Serial.printf("bytes %02x %02x %02x\n",b2,b1,b0);
        std::vector<char> reps;
        if(cp>0x7FF){
            reps.push_back(b2 | 0xE0);
            reps.push_back(b1 | 0x80);
            reps.push_back(b0 | 0x80);
        }
        else {
            if(cp>0x7f){
                reps.push_back(b1 | 0xC0);
                reps.push_back(b0 | 0x80);
            } else reps.push_back(b0);
        }
//        std::string spesh(reps.begin(),reps.end());
        std::string lhf=value.substr(0,u)+std::string(reps.begin(),reps.end())+value.substr(u+6,std::string::npos);
        value=lhf;
        u=value.find("\\u",u+6);
    }
    return value; 
}

std::unordered_map<std::string,std::string> json2nvp(const std::string& s){
    std::unordered_map<std::string,std::string> J;
    if(s.size() > 7){
        std::string json=ltrim(rtrim(ltrim(rtrim(s,']'),'['),'}'),'{');
        size_t i=json.find("\":");
        if(i){
            do{
                size_t h=1+json.rfind("\"",i-2);
                size_t j=json.find(",\"",i);
                J[json.substr(h,i-h)]=encodeUTF8(replaceAll(trim(json.substr(i+2,j-(i+2)),'"'),"\\/","/"));
                i=json.find("\":",i+2);
            } while(i!=std::string::npos);
            return J;
        } //else Serial.printf("can't parse json - no components\n");
    } //else Serial.printf("can't parse json - too short \n");
    return {};
}

std::string lowercase(std::string s){
   std::string ucase(s);
   transform(ucase.begin(), ucase.end(),ucase.begin(), [](unsigned char c){ return std::tolower(c); } );
   return ucase;
}

std::string ltrim(const std::string& s, const char d){
	std::string rv(s);
	while(rv.size() && (rv.front()==d)) rv=std::string(++rv.begin(),rv.end());
	return rv;	
}

std::string nvp2json(const std::unordered_map<std::string,std::string>& nvp){
  std::string j="{";
  for(auto const& m:nvp) j+="\""+m.first+"\":\""+m.second+"\",";
  j.pop_back();
  return j.append("}");
}
std::string replaceAll(const std::string& s,const std::string& f,const std::string& r){
    std::string tmp=s;
    size_t pos = tmp.find(f);
    while( pos != std::string::npos){
        tmp.replace(pos, f.size(), r);
        pos =tmp.find(f, pos + r.size());
    }
    return tmp;
}
// rationalise these two!!!!!!!!!!!!!
std::string replaceParams(const std::string& s,H4T_FN_LOOKUP f){
    int i=0;
	int j=0;
	std::string rv;
    rv.reserve((s.size()*115)/100);
	while(i < s.size()){
        if(s[i]=='%'){
            if(j){
                std::string v=s.substr(j,i-j);
                rv.append(f(v));
                j=0;
            }
            else j=i+1;
        } else if(!j) rv.push_back(s[i]);
        i++;
	}
    rv.shrink_to_fit();
	return rv.c_str();
}

std::string replaceParams(const std::string& s,H4T_NVP_MAP& nvp){
    int i=0;
	int j=0;
	std::string rv;
    rv.reserve((s.size()*115)/100);
	while(i < s.size()){
        if(s[i]=='%'){
            if(j){
                std::string v=s.substr(j,i-j);
                rv.append( nvp.count(v) ? nvp[v]:"%"+v+"%" );
                j=0;
            }
            else j=i+1;
        } else if(!j) rv.push_back(s[i]);
        i++;
	}
    rv.shrink_to_fit();
	return rv.c_str();
}

std::string rtrim(const std::string& s, const char d){
	std::string rv(s);
	while(rv.size() && (rv.back()==d)) rv.pop_back();
	return rv;	
}

H4T_VS split(const std::string& s, const char* delimiter){
	std::vector<std::string> vt;
	std::string delim(delimiter);
	auto len=delim.size();
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos){
		vt.push_back(s.substr(start, end - start));
		start = end + len;
		end = s.find(delim, start);
	}
	std::string tec=s.substr(start, end);
	if(tec.size()) vt.push_back(tec);		
	return vt;
}

std::string stringFromInt(int i,const char* fmt){
	char buf[16];
	sprintf(buf,fmt,i);
	return std::string(buf);
}

bool stringIsAlpha(const std::string& s){ 
    return !(std::find_if(s.begin(), s.end(),[](char c) { return !std::isalpha(c); }) != s.end());
}

bool stringIsNumeric(const std::string& s){ 
    std::string abs=(s[0]=='-' || s[0]=='+') ? std::string(++s.begin(),s.end()):s; // allow leading minus
    return !s.empty() && all_of(abs.begin(), abs.end(), ::isdigit);
}

std::string trim(const std::string& s, const char d){ return(ltrim(rtrim(s,d),d)); }

std::string uppercase(std::string s){
   std::string ucase(s);
   transform(ucase.begin(), ucase.end(),ucase.begin(), [](unsigned char c){ return std::toupper(c); } );
   return ucase;
}

std::string urlencode(const std::string &s){
    static const char lookup[]= "0123456789abcdef";
    std::string e;
    for(int i=0, ix=s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if ( (48 <= c && c <= 57) ||//0-9
             (65 <= c && c <= 90) ||//abc...xyz
             (97 <= c && c <= 122) || //ABC...XYZ
             (c=='-' || c=='_' || c=='.' || c=='~') 
        ) e.push_back(c);
        else
        {
            e.push_back('%');
            e.push_back(lookup[ (c&0xF0)>>4 ]);
            e.push_back(lookup[ (c&0x0F) ]);
        }
    }
    return e;
}

std::string urldecode(const std::string &s) { /// optimise this!!!
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<s.length(); i++) {
        if (int(s[i])==37) {
            sscanf(s.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else ret+=s[i]=='+' ? ' ':s[i];
    }
    return (ret);
}

