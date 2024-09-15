/**
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef RXI_VEC_H
#define RXI_VEC_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <stdlib.h>
#include <string.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define VEC_VERSION "0.2.1"

#define arr_unpack_( data, length, capacity ) ( char** ) data, length, capacity, sizeof( *data )
#define vec_unpack_( v ) ( char** ) &( v )->data, &( v )->length, &( v )->capacity, sizeof( *( v )->data )

#define vec_t( T )                                                                                                     \
    struct {                                                                                                           \
        T* data;                                                                                                       \
        int length, capacity;                                                                                          \
    }


#define vec_init( v ) ( memset( ( v ), 0, sizeof( *( v ) ) ) )

#define arr_deinit( arr, size ) ( memset( arr, 0, size ), free( arr ) )
#define vec_deinit( v ) ( vec_init( v ), free( ( v )->data ) )

#define arr_push( data, length, capacity, val )                                                                        \
    ( vec_expand_( arr_unpack_( data, length, capacity ) ) ? -1 : ( data[ length++ ] = ( val ), 0 ), 0 )
#define vec_push( v, val ) ( vec_expand_( vec_unpack_( v ) ) ? -1 : ( ( v )->data[ ( v )->length++ ] = ( val ), 0 ), 0 )

#define vec_pop( v ) ( v )->data[ --( v )->length ]

#define vec_get( v, n ) ( v )->data[ n ]

#define vec_splice( v, start, count ) ( vec_splice_( vec_unpack_( v ), start, count ), ( v )->length -= ( count ) )

#define vec_swapsplice( v, start, count )                                                                              \
    ( vec_swapsplice_( vec_unpack_( v ), start, count ), ( v )->length -= ( count ) )

#define vec_insert( v, idx, val )                                                                                      \
    ( vec_insert_( vec_unpack_( v ), idx ) ? -1 : ( ( v )->data[ idx ] = ( val ), 0 ), ( v )->length++, 0 )

#define vec_sort( v, fn ) qsort( ( v )->data, ( v )->length, sizeof( *( v )->data ), fn )

#define vec_swap( v, idx1, idx2 ) vec_swap_( vec_unpack_( v ), idx1, idx2 )

#define vec_truncate( v, len ) ( ( v )->length = ( len ) < ( v )->length ? ( len ) : ( v )->length )

#define vec_clear( v ) ( ( v )->length = 0 )

#define vec_first( v ) ( v )->data[ 0 ]

#define vec_last( v ) ( v )->data[ ( v )->length - 1 ]

#define arr_reserve( data, size, capacity, n ) vec_reserve_( arr_unpack_( data, size, capacity ), n )
#define vec_reserve( v, n ) vec_reserve_( vec_unpack_( v ), n )

#define vec_compact( v ) vec_compact_( vec_unpack_( v ) )

#define vec_pusharr( v, arr, count )                                                                                   \
    do {                                                                                                               \
        int i__, n__ = ( count );                                                                                      \
        if ( vec_reserve_po2_( vec_unpack_( v ), ( v )->length + n__ ) != 0 ) break;                                   \
        for ( i__ = 0; i__ < n__; i__++ ) { ( v )->data[ ( v )->length++ ] = ( arr )[ i__ ]; }                         \
    } while ( 0 )

#define vec_extend( v, v2 ) vec_pusharr( ( v ), ( v2 )->data, ( v2 )->length )

#define vec_find( v, val, idx )                                                                                        \
    do {                                                                                                               \
        for ( ( idx ) = 0; ( idx ) < ( v )->length; ( idx )++ )                                                        \
        {                                                                                                              \
            if ( ( v )->data[ ( idx ) ] == ( val ) ) break;                                                            \
        }                                                                                                              \
        if ( ( idx ) == ( v )->length ) ( idx ) = -1;                                                                  \
    } while ( 0 )

#define vec_remove( v, val )                                                                                           \
    do {                                                                                                               \
        int idx__;                                                                                                     \
        vec_find( v, val, idx__ );                                                                                     \
        if ( idx__ != -1 ) vec_splice( v, idx__, 1 );                                                                  \
    } while ( 0 )

#define vec_reverse( v )                                                                                               \
    do {                                                                                                               \
        int i__ = ( v )->length / 2;                                                                                   \
        while ( i__-- ) { vec_swap( ( v ), i__, ( v )->length - ( i__ + 1 ) ); }                                       \
    } while ( 0 )

#define vec_foreach( v, var, iter )                                                                                    \
    if ( ( v )->length > 0 )                                                                                           \
        for ( ( iter ) = 0; ( iter ) < ( v )->length && ( ( ( var ) = ( v )->data[ ( iter ) ] ), 1 ); ++( iter ) )

#define vec_foreach_rev( v, var, iter )                                                                                \
    if ( ( v )->length > 0 )                                                                                           \
        for ( ( iter ) = ( v )->length - 1; ( iter ) >= 0 && ( ( ( var ) = ( v )->data[ ( iter ) ] ), 1 ); --( iter ) )

#define vec_foreach_ptr( v, var, iter )                                                                                \
    if ( ( v )->length > 0 )                                                                                           \
        for ( ( iter ) = 0; ( iter ) < ( v )->length && ( ( ( var ) = &( v )->data[ ( iter ) ] ), 1 ); ++( iter ) )

#define vec_foreach_ptr_rev( v, var, iter )                                                                            \
    if ( ( v )->length > 0 )                                                                                           \
        for ( ( iter ) = ( v )->length - 1; ( iter ) >= 0 && ( ( ( var ) = &( v )->data[ ( iter ) ] ), 1 ); --( iter ) )

/***********************************************************************************************************************
Static function Prototypes
***********************************************************************************************************************/
int vec_expand_( char** data, int* length, int* capacity, int memsz );
int vec_reserve_( char** data, int* length, int* capacity, int memsz, int n );
int vec_reserve_po2_( char** data, int* length, int* capacity, int memsz, int n );
int vec_compact_( char** data, int* length, int* capacity, int memsz );
int vec_insert_( char** data, int* length, int* capacity, int memsz, int idx );
void vec_splice_( char** data, int* length, int* capacity, int memsz, int start, int count );
void vec_swapsplice_( char** data, int* length, int* capacity, int memsz, int start, int count );
void vec_swap_( char** data, int* length, int* capacity, int memsz, int idx1, int idx2 );

#endif// RXI_VEC_H