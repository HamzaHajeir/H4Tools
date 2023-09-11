#pragma once
#include <unordered_set>
#include "mbx.h"
#include "h4tools_config.h"
using H4T_MEM_POOL        =std::unordered_set<uint8_t*>;

class mbx {
    public:
        static  H4T_MEM_POOL  pool;
                bool            managed;
                size_t          len=0;
                uint8_t*        data=nullptr;
                uint8_t         flags=0;
        mbx(){}
        mbx(uint8_t* p,size_t s,bool copy=true,uint8_t f=0);
        // 
         ~mbx(){} // absolutely do not never nohow free the data pointer here! It must stay alive till it is ACKed

                void            clear();
        static  void            clear(uint8_t*);
        static  uint8_t*        getMemory(size_t size);
        static  void            dump(size_t slice=32);
        static  uint8_t*        realloc(uint8_t* p, size_t size);
};