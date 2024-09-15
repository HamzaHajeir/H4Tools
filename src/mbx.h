#pragma once
#include <unordered_set>
#include "h4tools_config.h"
using H4T_MEM_POOL        =std::unordered_set<uint8_t*>;

class mbx {
                uint8_t*        data=nullptr;
    public:
        static  H4T_MEM_POOL  pool;
                bool            managed;
                size_t          len=0;
                uint8_t         flags=0;
                uint8_t*        get();
        mbx(){}
        mbx(uint8_t* p,size_t s,bool copy=true,uint8_t f=0);
        mbx(const mbx& other) : data(other.data), managed(other.managed), len(other.len), flags(other.flags) {}
        mbx(const mbx&& other) : data(std::move(other.data)), managed(std::move(other.managed)), len(std::move(other.len)), flags(std::move(other.flags)) {}
        mbx& operator=(const mbx&);
        mbx& operator=(const mbx&&);
        // 
         ~mbx(){} // absolutely do not never nohow free the data pointer here! It must stay alive till it is ACKed

                void            clear();
        static  void            clear(uint8_t*);
        static  uint8_t*        getMemory(size_t size);
        static  void            dump(size_t slice=32);
        static  uint8_t*        realloc(uint8_t* p, size_t size);
};