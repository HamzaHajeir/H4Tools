#pragma once
#define H4T_VERSION "0.0.15"

#define H4T_DEBUG                   0

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