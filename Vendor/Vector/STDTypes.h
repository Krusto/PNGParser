#ifndef STDTYPES_HEADER
#define STDTYPES_HEADER
/***********************************************************************************************************************
Platform specific std type definitions
***********************************************************************************************************************/

#define BOOL char
#define TRUE 1u == 1u
#define FALSE 1u == 0u

#ifdef uint8_t
#undef uint8_t
#endif
#define uint8_t unsigned char
#ifdef int8_t
#undef int8_t
#endif
#define int8_t char
#ifdef uint32_t
#undef uint32_t
#endif
#define uint32_t unsigned int

#ifdef int32_t
#undef int32_t
#endif
#define int32_t int

#ifdef uint16_t
#undef uint16_t
#endif
#define uint16_t unsigned short

#ifdef int16_t
#undef int16_t
#endif
#define int16_t short

#define UINT16_MAX ( uint16_t ) - 1

#endif//STDTYPES_HEADER
