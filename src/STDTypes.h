#ifndef STDTYPES_HEADER
#define STDTYPES_HEADER
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
 * Platform specific standard type definitions so you can use the libary 
 * other devices
 */


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

#ifdef UINT16_MAX
#undef UINT16_MAX
#endif
#define UINT16_MAX ( ( uint16_t ) -1 )

#endif//STDTYPES_HEADER
