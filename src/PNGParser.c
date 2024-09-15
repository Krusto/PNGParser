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
 * PNGParser Implementation
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "PNGParser.h"
#include "PNGParserDefs.h"
#include "vector.h"
#include <zlib.h>

/***********************************************************************************************************************
Static variables
***********************************************************************************************************************/

PNGParserStateType sPNGParserStateTransitionTable[] = {
        /*PNGParser_State_None                    -->*/ PNGParser_State_Init,
        /*PNGParser_State_Init                    -->*/ PNGParser_State_Checking_Signature,
        /*PNGParser_State_Checking_Signature      -->*/ PNGParser_State_Reading_Chunk,
        /*PNGParser_State_Reading_Chunk           -->*/ PNGParser_State_Reading_Chunk_Done,
        /*PNGParser_State_Reading_Chunk_Done      -->*/ PNGParser_State_Reading_Chunk_Done,
        /*PNGParser_State_Reading_Chunks_Done     -->*/ PNGParser_State_Done,
        /*PNGParser_State_Done                    -->*/ PNGParser_State_Done,
        /*PNGParser_State_Error                   -->*/ PNGParser_State_Error,
};
PNGParserReadingChunkStateType sPNGParserReadingChunkStateTransitionTable[] = {
        /*PNGParser_Reading_Chunk_State_None         -->*/ PNGParser_Reading_Chunk_State_Get_Metadata,
        /*PNGParser_Reading_Chunk_State_Get_Metadata -->*/ PNGParser_Reading_Chunk_State_Check_CRC32,
        /*PNGParser_Reading_Chunk_State_Check_CRC32  -->*/ PNGParser_Reading_Chunk_State_None,
        /*PNGParser_Reading_Chunk_State_Error        -->*/ PNGParser_Reading_Chunk_State_Error,
        /*PNGParser_Reading_Chunk_State_Done         -->*/ PNGParser_Reading_Chunk_State_Done,
};

/***********************************************************************************************************************
Static function Prototypes
***********************************************************************************************************************/

void PNGParserInit( PNGParser* parser );
PNGParserResultType PNGParserUpdate( PNGParser* parser );
void PNGParserUpdateState( PNGParser* parser );
void PNGParserReadingChunkUpdateState( PNGParser* parser );
void PNGParserStateInit( PNGParser* parser );
PNGParserResultType PNGParserStateCheckingSignature( PNGParser* parser );
PNGParserResultType PNGParserStateReadingChunk( PNGParser* parser );
PNGParserChunkType PNGParserGetChunkType( const uint8_t* buffer );
uint32_t PNGParserGetChunkLength( const uint8_t* buffer );
uint32_t PNGParserGetReceivedCRC( const uint8_t* buffer );
void PNGParserGetChunkMetadata( PNGParser* parser );
PNGParserResultType PNGParserReadingChunkCheckCRC32( PNGParser* parser );
PNGParserResultType PNGParserParseIHDRChunk( PNGParser* parser, const uint8_t* buffer );
PNGParserResultType PNGParserParseIDATChunk( PNGParser* parser, uint32_t chunkID );
void PNGParserParseGAMMAChunk( PNGParser* parser, const uint8_t* buffer );
void PNGParserParseSRGBChunk( PNGParser* parser, const uint8_t* buffer );
PNGParserResultType PNGParserProcessPNG( PNGParser* parser );
PNGParserResultType PNGParserDecompressBuffer( PNGParser* parser );
uint16_t PNGParserPaethPredictor( int16_t a, int16_t b, int16_t c );
void PNGParserFilterCurrentByte( PNGParserFilteringType filter, uint8_t* currentByte, uint8_t up, uint8_t left,
                                 uint8_t upleft );
void PNGParserGetNeighbourBytes( const uint8_t* data, uint32_t index, uint32_t offset, uint32_t row, uint32_t rowStride,
                                 uint32_t channels, uint8_t* up, uint8_t* left, uint8_t* upleft );
void PNGParserFilterRow( PNGParser* parser, uint32_t imageDataOffset, uint32_t scanLineOffset, uint32_t rowStride,
                         uint32_t row );
PNGParserResultType PNGParserFilterBuffer( PNGParser* parser );
PNGParserResultType PNGParserCheckCRC( uint8_t* buffer, uint32_t length, uint32_t receivedCRC );
PNGParserResultType PNGParserProcessChunks( PNGParser* parser );

/***********************************************************************************************************************
Implementation
***********************************************************************************************************************/
PNGParserStateType PNGParserGetState( PNGParser* parser ) { return parser->state; }

PNGParserResultType PNGParse( uint8_t* data, uint32_t length, PNGImageDataType* pImage )
{
    PNGParser parser;
    parser.parameters.raw_data = data;
    parser.parameters.length = length;

    PNGParserInit( &parser );

    PNGParserResultType result = PNGParser_Result_OK;
    while ( ( PNGParserGetState( &parser ) != PNGParser_State_Done ) && ( PNGParser_Result_OK == result ) )
    {
        result = PNGParserUpdate( &parser );
    }

    vec_deinit( &parser.chunks );

    pImage->data = parser.imageData;
    pImage->length = parser.imageDataLength;
    pImage->imageSpec = parser.imageSpec;

    return result;
}

void PNGImageFree( uint8_t* imageData, uint32_t length ) { PNGPARSER_FREE( imageData, length ); }

void PNGParserInit( PNGParser* parser )
{
    parser->state = PNGParser_State_None;
    parser->seekOffset = 0;
    parser->chunkCount = 0;
    parser->compressedData = NULL;
    parser->compressedLength = 0;
    parser->decompressionBuffer = NULL;
    vec_init( &parser->chunks );
    parser->imageDataLength = 0;
    parser->imageData = NULL;
    parser->readingChunkState = PNGParser_Reading_Chunk_State_None;
    parser->state = PNGParser_State_None;
    PNGParserUpdateState( parser );
}

PNGParserResultType PNGParserUpdate( PNGParser* parser )
{
    PNGParserResultType result = PNGParser_Result_OK;
    switch ( parser->state )
    {
        case PNGParser_State_Init:
            PNGParserStateInit( parser );
            break;
        case PNGParser_State_Checking_Signature:
            result = PNGParserStateCheckingSignature( parser );
            break;
        case PNGParser_State_Reading_Chunk:
            result = PNGParserStateReadingChunk( parser );
            break;
        case PNGParser_State_Reading_Chunks_Done:
            result = PNGParserProcessChunks( parser );
        default:
            PNGParserUpdateState( parser );
            break;
    }
    return result;
}

void PNGParserUpdateState( PNGParser* parser ) { parser->state = sPNGParserStateTransitionTable[ parser->state ]; }

void PNGParserReadingChunkUpdateState( PNGParser* parser )
{
    parser->readingChunkState = sPNGParserReadingChunkStateTransitionTable[ parser->readingChunkState ];
}

void PNGParserStateInit( PNGParser* parser ) { PNGParserUpdateState( parser ); }

PNGParserResultType PNGParserStateCheckingSignature( PNGParser* parser )
{
    PNGParserResultType result = PNGParser_Result_OK;
    uint32_t length = parser->parameters.length;
    uint8_t* buffer = parser->parameters.raw_data;

    if ( length < PNG_SIGNATURE_LENGTH ) { result = PNGParser_Error_Buffer_Length; }
    else
    {
        const uint8_t signature[] = PNG_SIGNATURE_ARR_DATA;
        for ( uint8_t i = 0; ( ( i < PNG_SIGNATURE_LENGTH ) && ( result != PNGParser_Error_Wrong_Signature ) ); ++i )
        {
            if ( signature[ i ] != buffer[ i ] ) { result = PNGParser_Error_Wrong_Signature; }
        }
    }
    parser->seekOffset += PNG_SIGNATURE_LENGTH;
    if ( PNGParser_Result_OK == result ) { PNGParserUpdateState( parser ); }
    return result;
}

PNGParserResultType PNGParserStateReadingChunk( PNGParser* parser )
{
    PNGParserResultType result = PNGParser_Result_OK;
    switch ( parser->readingChunkState )
    {
        case PNGParser_Reading_Chunk_State_None:
            PNGParserReadingChunkUpdateState( parser );
            break;
        case PNGParser_Reading_Chunk_State_Get_Metadata:
            PNGParserGetChunkMetadata( parser );
            break;
        case PNGParser_Reading_Chunk_State_Check_CRC32:
            result = PNGParserReadingChunkCheckCRC32( parser );
            if ( PNGParser_Result_OK == result ) { PNGParserReadingChunkUpdateState( parser ); }
            break;
        default:
            break;
    }
    return result;
}

PNGParserChunkType PNGParserGetChunkType( const uint8_t* buffer )
{
    uint32_t type = UINT8_TO_UINT32( buffer[ 0 ], buffer[ 1 ], buffer[ 2 ], buffer[ 3 ] );
    uint32_t upperCaseType = TO_UPPERCASE_WORD( type );

    PNGParserChunkType result = PNGParser_Chunk_None;
    switch ( upperCaseType )
    {
        case IHDR_WORD:
        case PLTE_WORD:
        case SRGB_WORD:
        case GAMMA_WORD:
        case IDAT_WORD:
        case IEND_WORD:
            result = upperCaseType;
            break;
        default:
            break;
    }
    return result;
}

uint32_t PNGParserGetChunkLength( const uint8_t* buffer )
{
    return UINT8_TO_UINT32( buffer[ 0 ], buffer[ 1 ], buffer[ 2 ], buffer[ 3 ] );
}

uint32_t PNGParserGetReceivedCRC( const uint8_t* buffer )
{
    return UINT8_TO_UINT32( buffer[ 0 ], buffer[ 1 ], buffer[ 2 ], buffer[ 3 ] );
}

void PNGParserGetChunkMetadata( PNGParser* parser )
{
    uint32_t length = parser->parameters.length;
    uint8_t* buffer = &parser->parameters.raw_data[ parser->seekOffset ];

    PNGChunk chunk;

    if ( length == 0u ) { chunk.chunkData == NULL; }
    else { chunk.chunkData = &buffer[ PNGPARSER_CHUNK_DATA_OFFSET ]; }

    chunk.length = PNGParserGetChunkLength( buffer );
    buffer = &buffer[ 4 ];
    chunk.type = PNGParserGetChunkType( buffer );
    chunk.CRC = PNGParserGetReceivedCRC( &buffer[ chunk.length + 4 ] );
    vec_push( &parser->chunks, chunk );
    parser->chunkCount += 1;

    if ( chunk.type != PNGParser_Chunk_IEND && chunk.type != PNGParser_Chunk_None )
    {
        PNGParserReadingChunkUpdateState( parser );
    }
    else if ( chunk.type == PNGParser_Chunk_IEND )
    {
        parser->readingChunkState = PNGParser_Reading_Chunk_State_None;
        parser->state = PNGParser_State_Reading_Chunks_Done;
    }
    else
    {
        parser->readingChunkState = PNGParser_Reading_Chunk_State_None;
        parser->seekOffset += PNGPARSER_CHUNK_METADATA_LENGTH + chunk.length;
    }
}

PNGParserResultType PNGParserReadingChunkCheckCRC32( PNGParser* parser )
{
    PNGParserResultType result = PNGParser_Result_OK;

    uint8_t* buffer = &parser->parameters.raw_data[ parser->seekOffset ];

    PNGChunk* chunk = &vec_last( &parser->chunks );
    if ( chunk->type != PNGParser_Chunk_IEND && chunk->type != PNGParser_Chunk_None )
    {
        result = PNGParserCheckCRC( &buffer[ PNGPARSER_CHUNK_TYPE_OFFSET ], chunk->length + PNGPARSER_CHUNK_TYPE_LENGTH,
                                    chunk->CRC );
    }
    parser->seekOffset += PNGPARSER_CHUNK_METADATA_LENGTH + chunk->length;

    return result;
}

PNGParserResultType PNGParserParseIHDRChunk( PNGParser* parser, const uint8_t* buffer )
{
    PNGParserResultType result = PNGParser_Result_OK;
    PNGChunk_IHDRType* ihdr = ( PNGChunk_IHDRType* ) buffer;
    parser->imageSpec.width = SWAP_ENDIAN( ihdr->width );
    parser->imageSpec.height = SWAP_ENDIAN( ihdr->height );
    parser->imageSpec.bitDepth = ihdr->bitDepth;
    parser->imageSpec.colorType = ihdr->colorType;
    parser->imageSpec.compressionMethod = ihdr->compressionMethod;
    parser->imageSpec.filterMethod = ihdr->filterMethod;
    parser->imageSpec.interlaceMethod = ihdr->interlaceMethod;

    switch ( parser->imageSpec.colorType )
    {
        case 0:
            parser->imageSpec.channels = 1;
            break;
        case 2:
            parser->imageSpec.channels = 3;
            break;
        case 3:
            result = PNGParser_Error_Not_Supported_Interlaced_Image;
        case 4:
            parser->imageSpec.channels = 2;
            break;
        case 6:
            parser->imageSpec.channels = 4;
            break;
    }
    if ( PNGParser_Result_OK == result )
    {
        parser->imageSpec.pixelStride = parser->imageSpec.channels * parser->imageSpec.bitDepth;

        if ( parser->imageSpec.compressionMethod != PNGPARSER_COMPRESSION_INFLATION_INDEX )
        {
            result = PNGParser_Error_Wrong_Compression;
        }
        else { parser->readingChunkState = PNGParser_Reading_Chunk_State_None; }
    }
    return result;
}

PNGParserResultType PNGParserParseIDATChunk( PNGParser* parser, uint32_t chunkID )
{
    PNGParserResultType result = PNGParser_Result_OK;

    PNGChunk* chunk = &vec_get( &parser->chunks, chunkID );

    uint8_t* checkBuffPtr = NULL;
    if ( NULL == parser->compressedData ) { checkBuffPtr = ( uint8_t* ) PNGPARSER_MALLOC( chunk->length ); }
    else { checkBuffPtr = PNGPARSER_REALLOC( parser->compressedData, parser->compressedLength + chunk->length ); }
    if ( NULL != checkBuffPtr )
    {
        parser->compressedData = checkBuffPtr;
        PNGPARSER_MEMCPY( &parser->compressedData[ parser->compressedLength ], chunk->chunkData, chunk->length );
        parser->compressedLength += chunk->length;
    }
    else { result = PNGParser_Error_Allocating_Memory; }

    return result;
}

void PNGParserParseGAMMAChunk( PNGParser* parser, const uint8_t* buffer )
{
    parser->imageSpec.gamma = SWAP_ENDIAN( UINT8_TO_UINT32( buffer[ 0 ], buffer[ 1 ], buffer[ 2 ], buffer[ 3 ] ) );
    parser->readingChunkState = PNGParser_Reading_Chunk_State_None;
}

void PNGParserParseSRGBChunk( PNGParser* parser, const uint8_t* buffer )
{
    parser->imageSpec.renderingIntent = buffer[ 0 ];
    parser->readingChunkState = PNGParser_Reading_Chunk_State_None;
}

PNGParserResultType PNGParserProcessPNG( PNGParser* parser )
{
    PNGParserResultType result;

    result = PNGParserDecompressBuffer( parser );
    if ( result == PNGParser_Result_OK ) { result = PNGParserFilterBuffer( parser ); }

    return result;
}

PNGParserResultType PNGParserDecompressBuffer( PNGParser* parser )
{

    PNGParserResultType result = PNGParser_Result_OK;
    parser->maxDecompressionLength =
            ( 1 + parser->imageSpec.width ) * parser->imageSpec.height * parser->imageSpec.channels;
    parser->decompressedLength = parser->maxDecompressionLength;
    if ( parser->decompressionBuffer == NULL )
    {
        parser->decompressionBuffer = ( uint8_t* ) PNGPARSER_MALLOC( parser->decompressedLength );
    }
    int32_t decompressResult =
            uncompress( ( Bytef* ) parser->decompressionBuffer, ( uLongf* ) &parser->decompressedLength,
                        ( Bytef* ) parser->compressedData, parser->compressedLength );

    if ( Z_OK != decompressResult ) { result = PNGParser_Error_During_Decompression; }
    return result;
}

uint16_t PNGParserPaethPredictor( int16_t a, int16_t b, int16_t c )
{
    int thresh = c * 3 - ( a + b );
    int lo = a < b ? a : b;
    int hi = a < b ? b : a;
    int t0 = ( hi <= thresh ) ? lo : c;
    int t1 = ( thresh <= lo ) ? hi : t0;

    return ( uint8_t ) t1;
}

void PNGParserFilterCurrentByte( PNGParserFilteringType filter, uint8_t* currentByte, uint8_t up, uint8_t left,
                                 uint8_t upleft )
{
    switch ( filter )
    {
        case PNGParser_Filtering_Sub: {
            *currentByte += left;
        }
        break;
        case PNGParser_Filtering_Up: {
            *currentByte += ( uint8_t ) ( up );
        }
        break;
        case PNGParser_Filtering_Average: {
            *currentByte += ( uint8_t ) FLOOR( ( left + up ) / 2 );
        }
        break;
        case PNGParser_Filtering_Paeth: {
            *currentByte += ( uint8_t ) PNGParserPaethPredictor( left, up, upleft );
        }
        break;
        default:
            break;
    }
}

void PNGParserGetNeighbourBytes( const uint8_t* data, uint32_t index, uint32_t offset, uint32_t row, uint32_t rowStride,
                                 uint32_t channels, uint8_t* up, uint8_t* left, uint8_t* upleft )
{
    offset += index;
    if ( index >= channels )
    {
        *left = data[ offset - channels ];
        if ( row != 0 ) { *upleft = data[ ( offset - rowStride ) - channels ]; }
    }
    if ( row != 0 ) { *up = data[ ( offset - rowStride ) ]; }
}

void PNGParserFilterRow( PNGParser* parser, uint32_t imageDataOffset, uint32_t scanLineOffset, uint32_t rowStride,
                         uint32_t row )
{
    uint8_t filterType = parser->decompressionBuffer[ scanLineOffset ];
    PNGPARSER_MEMCPY( &parser->imageData[ imageDataOffset ], &parser->decompressionBuffer[ scanLineOffset + 1 ],
                      rowStride );
    uint8_t left = 0, up = 0, upleft = 0;

    for ( uint32_t i = 0; i < rowStride; i++ )
    {
        PNGParserGetNeighbourBytes( parser->imageData, i, imageDataOffset, row, rowStride, parser->imageSpec.channels,
                                    &up, &left, &upleft );

        PNGParserFilterCurrentByte( filterType, &parser->imageData[ imageDataOffset + i ], up, left, upleft );
    }
}

PNGParserResultType PNGParserFilterBuffer( PNGParser* parser )
{

    parser->imageDataLength = parser->imageSpec.channels * parser->imageSpec.width * parser->imageSpec.height;
    if ( NULL == parser->imageData ) { parser->imageData = ( uint8_t* ) PNGPARSER_MALLOC( parser->imageDataLength ); }
    PNGPARSER_MEMSET( parser->imageData, 0, parser->imageDataLength );

    uint32_t imageRowStride = parser->imageSpec.width * parser->imageSpec.channels;
    uint32_t scanLineWidth = imageRowStride + 1;
    uint32_t offset = 0, scanLineOffset = 0;

    for ( uint32_t row = 0; row < parser->imageSpec.height; row++ )
    {
        PNGParserFilterRow( parser, offset, scanLineOffset, imageRowStride, row );

        offset += imageRowStride;
        scanLineOffset += scanLineWidth;
    }
    PNGPARSER_FREE( parser->compressedData, parser->compressedLength );
    PNGPARSER_FREE( parser->decompressionBuffer, parser->decompressedLength );
    return PNGParser_Result_OK;
}

PNGParserResultType PNGParserCheckCRC( uint8_t* buffer, uint32_t length, uint32_t receivedCRC )
{
    uint32_t calculatedCRC = CRC32( buffer, length );
    if ( calculatedCRC != receivedCRC ) { return PNGParser_Error_Corrupted_Data; }
    else { return PNGParser_Result_OK; }
}

PNGParserResultType PNGParserProcessChunks( PNGParser* parser )
{
    PNGParserResultType result = PNGParser_Result_OK;

    for ( uint32_t i = 0; i < parser->chunkCount && result == PNGParser_Result_OK; i++ )
    {
        PNGChunk* chunk = &vec_get( &parser->chunks, i );

        switch ( chunk->type )
        {
            case PNGParser_Chunk_IHDR:
                result = PNGParserParseIHDRChunk( parser, chunk->chunkData );
                break;
            case PNGParser_Chunk_IDAT:
                result = PNGParserParseIDATChunk( parser, i );
                break;
            case PNGParser_Chunk_PLTE:
                result = PNGParser_Error_General;
                break;
            case PNGParser_Chunk_GAMMA:
                PNGParserParseGAMMAChunk( parser, chunk->chunkData );
                break;
            case PNGParser_Chunk_SRGB:
                PNGParserParseSRGBChunk( parser, chunk->chunkData );
                break;
            case PNGParser_Chunk_IEND:
                result = PNGParserProcessPNG( parser );
                break;
            default:
                break;
        }
    }
    if ( PNGParser_Result_OK == result ) { PNGParserUpdateState( parser ); }
    return result;
}