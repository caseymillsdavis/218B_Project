#ifndef TYPES_H
#define TYPES_H

typedef unsigned char boolean;

#define False ((boolean) 0)
#define True  ((boolean) !False)

/* Standard ANSI  99 C types */
#ifndef int8_t
typedef signed char int8_t;
#endif
#ifndef int16_t
typedef signed int   int16_t;
#endif
#ifndef int32_t
typedef signed long int    int32_t;
#endif

#ifndef uint8_t
typedef unsigned char       uint8_t;
#endif
#ifndef uint16_t
typedef unsigned int  uint16_t;
#endif
#ifndef uint32_t
typedef unsigned long int   uint32_t;
#endif

#endif /* TYPES_H */