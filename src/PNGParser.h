#ifndef PNGPARSER_HEADER
#define PNGPARSER_HEADER
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
 * PNGParser Header File
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "PNGParserDefs.h"

/**
 * @brief Parses the image buffer and puts the result int pImage.
 * @param data 
 * @param length 
 * @param pImage 
 * @return Parse result code
 */
extern PNGParserResultType PNGParse( uint8_t* data, uint32_t length, PNGImage* pImage );
/**
 * @brief Frees the pImage memory that is allocated.
 * @param imageData 
 * @param length 
 */
extern void PNGImageFree( uint8_t* imageData, uint32_t length );

/**
 * @brief External library function for calculating CRC.
 * @param buffer 
 * @param length 
 * @return CRC32 of the buffer
 */
extern uint32_t CRC32( uint8_t* buffer, uint32_t length );

#endif// PNGPARSER_HEADER