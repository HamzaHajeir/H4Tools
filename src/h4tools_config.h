#define H4T_VERSION "0.0.2"

#define H4T_DEBUG                   0

#define H4T_PWM_DEFAULT         48000

#ifdef ARDUINO_ARCH_ESP8266
    #define H4T_HEAP_SAFETY      3000
    #define H4T_HEAP_CUTOUT_PC     10
    #define H4T_HEAP_CUTIN_PC      15 // MUST BE HIGHER!
#else
    #define H4T_HEAP_SAFETY      6000
    #define H4T_HEAP_CUTOUT_PC     20
    #define H4T_HEAP_CUTIN_PC      35 // MUST BE HIGHER!
#endif