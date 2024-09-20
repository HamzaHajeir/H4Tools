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
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "mbx.h"
#include "H4Tools.h"

H4T_MEM_POOL          mbx::pool;
mbx& mbx::operator=(const mbx& other) {
    data = other.data;
    managed = other.managed;
    len = other.len;
    flags = other.flags;
    return *this;
}
mbx& mbx::operator=(const mbx&& other) {
    data = std::move(other.data);
    managed = std::move(other.managed);
    len = std::move(other.len);
    flags = std::move(other.flags);
    return *this;
}
mbx::mbx(uint8_t* p,size_t s,bool copy,uint8_t f):managed(copy),len(s),flags(f){
    // Serial.printf("mbx::mbx(%p,%u,%d,%u) -> %p\n",p,s,copy,f,this);
    if(managed){
        data=getMemory(len);
        if(data) {
            memcpy(data,p,len);
            H4T_PRINT1("MBX %p len=%d COPIED FROM %p POOL=%d\n",(void*) data,len,p,pool.size());
        }
        else H4T_PRINT1("MBX %p len=%d MALLOC FAIL\n",(void*) data,len);
    } 
    else {
        H4T_PRINT1("MBX %p len=%d UNMANAGED POOL=%d\n",p,len,pool.size());
        data=p;
    }
}

uint8_t *mbx::get()
{
    // Serial.printf("mbx::get() this %p data %p\n", this, data);
	return data;
}
//
// public
//
void mbx::clear(uint8_t* p){
    // Serial.printf("mbx::clear(%p) exists %d\n", p, pool.count(p));
    if(pool.count(p)) {
        H4T_PRINT1("MBX DEL BLOCK %p\n",p);
        free(p);
        pool.erase(p);
        H4T_PRINT1("MBX DEL %p POOL NOW %d\n",p,pool.size());
    } else H4T_PRINT1("INSANITY? %p NOT IN POOL!\n",p);
}

void mbx::clear(){ if(managed) clear(data); }

uint8_t* mbx::getMemory(size_t size){
    uint8_t* mm=static_cast<uint8_t*>(malloc(size));
    if(mm){
        pool.insert(mm);
        H4T_PRINT1("MBX GM %p len=%d POOL=%d\n",mm,size,pool.size());
    } else H4T_PRINT1("********** MBX FAIL STATUS: FH=%u MXBLK=%u ASKED:%u\n",_HAL_freeHeap(),_HAL_maxHeapBlock(),size);
    return mm;
}

void mbx::            dump(size_t slice){
#if H4T_DEBUG >= 2
    H4T_PRINT2("Memory POOL DUMP s=%d\n", pool.size());
    for (auto &p:pool) Serial.printf("%p\t",p);
    Serial.println();
    for(auto &p:pool) dumphex(p,slice);
#endif
}
uint8_t *mbx::realloc(uint8_t *p, size_t size)
{
    if (pool.count(p)){
        auto ptr = static_cast<uint8_t *>(malloc(size));
        H4T_PRINT1("MBX REALLOC %p %d --> %p\n", p, size, ptr);
        if (ptr){
            clear(p);
            pool.insert(ptr);
            return ptr;
        }
    }
    return nullptr;
}
