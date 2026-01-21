/*
MIT License

Copyright (c) 2026 H4Group

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Contact Email: TBD
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
