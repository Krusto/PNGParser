#include "CRC32.h"
#include "PNGParser.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned char* read_binary_file( const char* filename, size_t* filesize );

int main( void )
{
    int w, h, comp;

    const uint8_t* img;
    int writeResult;


    size_t bufferSize;
    uint8_t* inputBuffer = read_binary_file( "example_images/4.png", &bufferSize );

    PNGImage image;
    PNGParserResultType result = PNGParse( inputBuffer, bufferSize, &image );
    if ( result != PNGParser_Result_OK )
    {
        printf( "Error While Parsing" );
        return -1;
    }
    printf( "Image Size:\n" );
    printf( "      Width:%d\n", image.imageSpec.width );
    printf( "      Height:%d\n", image.imageSpec.height );
    printf( "      Channels:%d\n", image.imageSpec.channels );
    printf( "      Bit Depth:%d\n", image.imageSpec.bitDepth );
    PNGImageFree( image.data, image.length );

    return 0;
}

unsigned char* read_binary_file( const char* filename, size_t* filesize )
{
    // start processing
    FILE* fileIn = fopen( filename, "rb" );// open input file (binary)
    if ( fileIn == NULL )
    {
        puts( "Error opening input file\n" );
        exit( -1 );
    }


    // obtain file size.
    fseek( fileIn, 0, SEEK_END );
    *filesize = ftell( fileIn );
    rewind( fileIn );
    printf( "Filesize: %d bytes.\n", *filesize );

    // allocate memory to contain the whole file.
    uint8_t* buffer = ( unsigned char* ) malloc( *filesize );
    if ( buffer == NULL )
    {
        puts( "malloc for input file buffer failed (not enough memory?)" );
        exit( -1 );
    }

    // copy the file into the buffer.
    fread( buffer, 1, *filesize, fileIn );
    fclose( fileIn );
    return buffer;
}