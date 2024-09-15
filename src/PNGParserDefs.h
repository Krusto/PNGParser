#ifndef PNGPARSER_DEFS
#define PNGPARSER_DEFS
/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2024 Krusto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * PNGParser Type Definitions
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "STDTypes.h"

#include <vector.h>
#include <zlib.h>

#ifndef PNGPARSER_MALLOC
#include <stdlib.h>
#endif

#ifndef NON_STD_MATH
#include <math.h>
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define PNG_SIGNATURE_LENGTH 8u
#define PNG_SIGNATURE_ARR_DATA                                                                                         \
    {                                                                                                                  \
        137, 80, 78, 71, 13, 10, 26, 10                                                                                \
    }
#define UINT8_TO_UINT32( x, y, z, w ) ( ( ( x ) << 24 ) | ( ( y ) << 16 ) | ( ( z ) << 8 ) | ( w ) )
#define TO_UPPERCASE_WORD( x ) ( ( x ) & 0xDFDFDFDF )
#define TO_LOWERCASE_WORD( x ) ( ( x ) | 0x10101010 )

#define GET_BYTE0( x ) ( ( x ) & 0xFF )
#define GET_BYTE1( x ) ( ( ( x ) & 0xFF00 ) >> 8 )
#define GET_BYTE2( x ) ( ( ( x ) & 0xFF0000 ) >> 16 )
#define GET_BYTE3( x ) ( ( ( x ) & 0xFF000000 ) >> 24 )

#define SWAP_ENDIAN( x )                                                                                               \
    ( ( GET_BYTE0( ( x ) & 0xFF ) << 24 ) | ( GET_BYTE1( x ) << 16 ) | ( GET_BYTE2( x ) << 8 ) | ( GET_BYTE3( x ) ) )

#define IHDR_WORD 0x49484452
#define PLTE_WORD 0x504C5445
#define SRGB_WORD 0x53524742
#define GAMMA_WORD 0x47414D41
#define IDAT_WORD 0x49444154
#define IEND_WORD 0x49454E44

#define PNGPARSER_CHUNK_LENGTH_LENGTH 4u
#define PNGPARSER_CHUNK_TYPE_OFFSET 4u
#define PNGPARSER_CHUNK_TYPE_LENGTH 4u
#define PNGPARSER_CHUNK_DATA_OFFSET ( PNGPARSER_CHUNK_TYPE_OFFSET + PNGPARSER_CHUNK_TYPE_LENGTH )
#define PNGPARSER_CHUNK_CRC_LENGTH 4u
#define PNGPARSER_CHUNK_METADATA_LENGTH                                                                                \
    ( PNGPARSER_CHUNK_LENGTH_LENGTH + PNGPARSER_CHUNK_TYPE_LENGTH + PNGPARSER_CHUNK_CRC_LENGTH )
#define PNGPARSER_COMPRESSION_INFLATION_INDEX 0u

#ifndef PNGPARSER_MALLOC
#define PNGPARSER_MEMCPY( dest, p, size ) memcpy( dest, p, size )
#define PNGPARSER_MALLOC( size ) malloc( size )
#define PNGPARSER_REALLOC( p, new_size ) realloc( p, new_size )
#define PNGPARSER_FREE( p, size ) free( p )
#define PNGPARSER_MEMSET( p, value, size ) memset( p, value, size )
#endif

#ifndef NON_STD_MATH
#define FLOOR( n ) ( floor( n ) )
#endif

/***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/

typedef enum
{
    PNGParser_Chunk_None = 0,
    PNGParser_Chunk_IHDR = IHDR_WORD,
    PNGParser_Chunk_PLTE = PLTE_WORD,
    PNGParser_Chunk_SRGB = SRGB_WORD,
    PNGParser_Chunk_GAMMA = GAMMA_WORD,
    PNGParser_Chunk_IDAT = IDAT_WORD,
    PNGParser_Chunk_IEND = IEND_WORD
} PNGParserChunkType;

typedef enum
{
    PNGParser_Filtering_None = 0,
    PNGParser_Filtering_Sub,
    PNGParser_Filtering_Up,
    PNGParser_Filtering_Average,
    PNGParser_Filtering_Paeth,
} PNGParserFilteringType;

typedef enum
{
    PNGParser_Result_OK = 0,
    PNGParser_Error_Buffer_Length,
    PNGParser_Error_Wrong_Signature,
    PNGParser_Error_Corrupted_Data,
    PNGParser_Error_Wrong_Compression,
    PNGParser_Error_During_Decompression,
    PNGParser_Error_Allocating_Memory,
    PNGParser_Error_Not_Supported_Interlaced_Image,
    PNGParser_Error_General
} PNGParserResultType;

typedef enum
{
    PNGParser_State_None = 0,
    PNGParser_State_Init,
    PNGParser_State_Checking_Signature,
    PNGParser_State_Reading_Chunk,
    PNGParser_State_Reading_Chunk_Done,
    PNGParser_State_Reading_Chunks_Done,
    PNGParser_State_Done,
    PNGParser_State_Error
} PNGParserStateType;

typedef enum
{
    PNGParser_Reading_Chunk_State_None = 0,
    PNGParser_Reading_Chunk_State_Get_Metadata,
    PNGParser_Reading_Chunk_State_Check_CRC32,
    PNGParser_Reading_Chunk_State_Error,
    PNGParser_Reading_Chunk_State_Done,
} PNGParserReadingChunkStateType;

typedef struct {
    uint32_t length;
    uint32_t CRC;
    uint32_t flags;
    PNGParserChunkType type;
    uint8_t* chunkData;
} PNGChunk;

typedef vec_t( PNGChunk ) vec_PNGChunk_t;

typedef struct {
    int32_t width;
    int32_t height;
    uint8_t bitDepth;
    uint8_t colorType;
    uint8_t compressionMethod;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
} PNGChunk_IHDRType;

typedef struct {
    uint32_t length;
    uint8_t* raw_data;
} PNGParserParameterType;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t gamma;
    uint8_t bitDepth;
    uint8_t colorType;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
    uint8_t pixelStride;
    uint8_t compressionMethod;
    uint8_t renderingIntent;
} PNGMetadataDataType;

typedef struct PNGParserInternalDataType {
    PNGParserStateType state;
    PNGParserReadingChunkStateType readingChunkState;
    PNGParserParameterType parameters;
    PNGParserResultType result;
    PNGMetadataDataType imageSpec;
    uint32_t chunkCount;
    uint32_t imageDataLength;
    uint32_t compressedLength;
    uint32_t decompressedLength;
    uint32_t maxDecompressionLength;
    uint32_t seekOffset;
    vec_PNGChunk_t chunks;

    uint8_t* imageData;
    uint8_t* decompressionBuffer;
    uint8_t* compressedData;
} PNGParserInternalDataType;

typedef struct PNGParserDataType {
    PNGMetadataDataType imageSpec;
    uint32_t length;
    uint8_t* data;
} PNGImageDataType;

typedef PNGParserInternalDataType PNGParser;
typedef PNGImageDataType PNGImage;

#endif//PNGPARSER_DEFS