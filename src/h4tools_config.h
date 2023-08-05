#pragma once
#define H4T_VERSION "0.0.8"

#define H4T_DEBUG                   0

#if H4T_DEBUG
#ifndef H4AT_PRINTF
#ifdef EMBEDDED_PLATFORM
    #define H4T_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #include <stdio.h>
    #define H4AT_PRINTF(...) printf(__VA_ARGS__)
#endif
#endif
    template<int I, typename... Args>
    void H4T_PRINT(const char* fmt, Args... args) {
        #ifdef ARDUINO_ARCH_ESP32
        // if (H4T_DEBUG >= I) H4T_PRINTF(std::string(std::string("H4T:%d: H=%u M=%u S=%u ")+fmt).c_str(),I,_HAL_freeHeap(),_HAL_maxHeapBlock(),uxTaskGetStackHighWaterMark(NULL),args...);
        if (H4T_DEBUG >= I) H4T_PRINTF(fmt,args...);
        #else
        // if (H4T_DEBUG >= I) H4T_PRINTF(std::string(std::string("H4T:%d: H=%u M=%u ")+fmt).c_str(),I,_HAL_freeHeap(),_HAL_maxHeapBlock(),args...);
        if (H4T_DEBUG >= I) H4T_PRINTF(fmt,args...);
        #endif
    }
    #define H4T_PRINT1(...) H4T_PRINT<1>(__VA_ARGS__)
    #define H4T_PRINT2(...) H4T_PRINT<2>(__VA_ARGS__)
    #define H4T_PRINT3(...) H4T_PRINT<3>(__VA_ARGS__)
    #define H4T_PRINT4(...) H4T_PRINT<4>(__VA_ARGS__)

#else
    #define H4T_PRINTF(...)
    #define H4T_PRINT1(...)
    #define H4T_PRINT2(...)
    #define H4T_PRINT3(...)
    #define H4T_PRINT4(...)

#endif
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define H4T_PWM_DEFAULT         10000
#else
#define H4T_PWM_DEFAULT         48000
#endif

#ifdef ARDUINO_ARCH_ESP8266
    #define H4T_HEAP_SAFETY      3000
    #define H4AT_HEAP_THROTTLE_LO 3000
    #define H4AT_HEAP_THROTTLE_HI 5000
    #define H4T_HEAP_CUTOUT_PC     10
    #define H4T_HEAP_CUTIN_PC      15 // MUST BE HIGHER!
#else
    #define H4T_HEAP_SAFETY      6000
    #define H4AT_HEAP_THROTTLE_LO 20000
    #define H4AT_HEAP_THROTTLE_HI 25000
    #define H4T_HEAP_CUTOUT_PC     20
    #define H4T_HEAP_CUTIN_PC      35 // MUST BE HIGHER!
#endif