#ifndef VECTOR_H
#define VECTOR_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "STDTypes.h"
#include "vec.h"

/***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/

typedef vec_t( void* ) vec_ptr_t;
typedef vec_t( char* ) vec_str_t;
typedef vec_t( uint32_t ) vec_uint32_t;
typedef vec_t( int32_t ) vec_int32_t;
typedef vec_t( uint16_t ) vec_uint16_t;
typedef vec_t( int16_t ) vec_int16_t;
typedef vec_t( uint8_t ) vec_uint8_t;
typedef vec_t( int8_t ) vec_int8_t;
typedef vec_t( float ) vec_float_t;
typedef vec_t( double ) vec_double_t;

#endif