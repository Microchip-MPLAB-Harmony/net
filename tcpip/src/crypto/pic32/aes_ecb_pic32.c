/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/


/****************************************************************************
// advanced encryption standard
// author: karl malbrain, malbrain@yahoo.com
//
// adapted from Christophe Devine's tables
// and George Anescu's c++ code.
 ********************************************************************/
//#include <config_crypto.h>
#include "aes_ecb_pic32.h"
//#include <GenericTypeDefs.h>

#define AES_KEY (4 * Nk)
#define ROUND_KEY_COUNT ((Nr + 1) * 4)

uint8_t Sbox[256] = {		// forward s-box
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

uint8_t InvSbox[256] = {	// inverse s-box
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
	0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
	0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
	0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
	0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
	0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
	0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
	0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
	0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
	0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
	0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
	0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
	0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
	0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
	0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
	0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
	0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

/* forward tables */

#define FT \
    V(C6,63,63,A5), V(F8,7C,7C,84), V(EE,77,77,99), V(F6,7B,7B,8D), \
    V(FF,F2,F2,0D), V(D6,6B,6B,BD), V(DE,6F,6F,B1), V(91,C5,C5,54), \
    V(60,30,30,50), V(02,01,01,03), V(CE,67,67,A9), V(56,2B,2B,7D), \
    V(E7,FE,FE,19), V(B5,D7,D7,62), V(4D,AB,AB,E6), V(EC,76,76,9A), \
    V(8F,CA,CA,45), V(1F,82,82,9D), V(89,C9,C9,40), V(FA,7D,7D,87), \
    V(EF,FA,FA,15), V(B2,59,59,EB), V(8E,47,47,C9), V(FB,F0,F0,0B), \
    V(41,AD,AD,EC), V(B3,D4,D4,67), V(5F,A2,A2,FD), V(45,AF,AF,EA), \
    V(23,9C,9C,BF), V(53,A4,A4,F7), V(E4,72,72,96), V(9B,C0,C0,5B), \
    V(75,B7,B7,C2), V(E1,FD,FD,1C), V(3D,93,93,AE), V(4C,26,26,6A), \
    V(6C,36,36,5A), V(7E,3F,3F,41), V(F5,F7,F7,02), V(83,CC,CC,4F), \
    V(68,34,34,5C), V(51,A5,A5,F4), V(D1,E5,E5,34), V(F9,F1,F1,08), \
    V(E2,71,71,93), V(AB,D8,D8,73), V(62,31,31,53), V(2A,15,15,3F), \
    V(08,04,04,0C), V(95,C7,C7,52), V(46,23,23,65), V(9D,C3,C3,5E), \
    V(30,18,18,28), V(37,96,96,A1), V(0A,05,05,0F), V(2F,9A,9A,B5), \
    V(0E,07,07,09), V(24,12,12,36), V(1B,80,80,9B), V(DF,E2,E2,3D), \
    V(CD,EB,EB,26), V(4E,27,27,69), V(7F,B2,B2,CD), V(EA,75,75,9F), \
    V(12,09,09,1B), V(1D,83,83,9E), V(58,2C,2C,74), V(34,1A,1A,2E), \
    V(36,1B,1B,2D), V(DC,6E,6E,B2), V(B4,5A,5A,EE), V(5B,A0,A0,FB), \
    V(A4,52,52,F6), V(76,3B,3B,4D), V(B7,D6,D6,61), V(7D,B3,B3,CE), \
    V(52,29,29,7B), V(DD,E3,E3,3E), V(5E,2F,2F,71), V(13,84,84,97), \
    V(A6,53,53,F5), V(B9,D1,D1,68), V(00,00,00,00), V(C1,ED,ED,2C), \
    V(40,20,20,60), V(E3,FC,FC,1F), V(79,B1,B1,C8), V(B6,5B,5B,ED), \
    V(D4,6A,6A,BE), V(8D,CB,CB,46), V(67,BE,BE,D9), V(72,39,39,4B), \
    V(94,4A,4A,DE), V(98,4C,4C,D4), V(B0,58,58,E8), V(85,CF,CF,4A), \
    V(BB,D0,D0,6B), V(C5,EF,EF,2A), V(4F,AA,AA,E5), V(ED,FB,FB,16), \
    V(86,43,43,C5), V(9A,4D,4D,D7), V(66,33,33,55), V(11,85,85,94), \
    V(8A,45,45,CF), V(E9,F9,F9,10), V(04,02,02,06), V(FE,7F,7F,81), \
    V(A0,50,50,F0), V(78,3C,3C,44), V(25,9F,9F,BA), V(4B,A8,A8,E3), \
    V(A2,51,51,F3), V(5D,A3,A3,FE), V(80,40,40,C0), V(05,8F,8F,8A), \
    V(3F,92,92,AD), V(21,9D,9D,BC), V(70,38,38,48), V(F1,F5,F5,04), \
    V(63,BC,BC,DF), V(77,B6,B6,C1), V(AF,DA,DA,75), V(42,21,21,63), \
    V(20,10,10,30), V(E5,FF,FF,1A), V(FD,F3,F3,0E), V(BF,D2,D2,6D), \
    V(81,CD,CD,4C), V(18,0C,0C,14), V(26,13,13,35), V(C3,EC,EC,2F), \
    V(BE,5F,5F,E1), V(35,97,97,A2), V(88,44,44,CC), V(2E,17,17,39), \
    V(93,C4,C4,57), V(55,A7,A7,F2), V(FC,7E,7E,82), V(7A,3D,3D,47), \
    V(C8,64,64,AC), V(BA,5D,5D,E7), V(32,19,19,2B), V(E6,73,73,95), \
    V(C0,60,60,A0), V(19,81,81,98), V(9E,4F,4F,D1), V(A3,DC,DC,7F), \
    V(44,22,22,66), V(54,2A,2A,7E), V(3B,90,90,AB), V(0B,88,88,83), \
    V(8C,46,46,CA), V(C7,EE,EE,29), V(6B,B8,B8,D3), V(28,14,14,3C), \
    V(A7,DE,DE,79), V(BC,5E,5E,E2), V(16,0B,0B,1D), V(AD,DB,DB,76), \
    V(DB,E0,E0,3B), V(64,32,32,56), V(74,3A,3A,4E), V(14,0A,0A,1E), \
    V(92,49,49,DB), V(0C,06,06,0A), V(48,24,24,6C), V(B8,5C,5C,E4), \
    V(9F,C2,C2,5D), V(BD,D3,D3,6E), V(43,AC,AC,EF), V(C4,62,62,A6), \
    V(39,91,91,A8), V(31,95,95,A4), V(D3,E4,E4,37), V(F2,79,79,8B), \
    V(D5,E7,E7,32), V(8B,C8,C8,43), V(6E,37,37,59), V(DA,6D,6D,B7), \
    V(01,8D,8D,8C), V(B1,D5,D5,64), V(9C,4E,4E,D2), V(49,A9,A9,E0), \
    V(D8,6C,6C,B4), V(AC,56,56,FA), V(F3,F4,F4,07), V(CF,EA,EA,25), \
    V(CA,65,65,AF), V(F4,7A,7A,8E), V(47,AE,AE,E9), V(10,08,08,18), \
    V(6F,BA,BA,D5), V(F0,78,78,88), V(4A,25,25,6F), V(5C,2E,2E,72), \
    V(38,1C,1C,24), V(57,A6,A6,F1), V(73,B4,B4,C7), V(97,C6,C6,51), \
    V(CB,E8,E8,23), V(A1,DD,DD,7C), V(E8,74,74,9C), V(3E,1F,1F,21), \
    V(96,4B,4B,DD), V(61,BD,BD,DC), V(0D,8B,8B,86), V(0F,8A,8A,85), \
    V(E0,70,70,90), V(7C,3E,3E,42), V(71,B5,B5,C4), V(CC,66,66,AA), \
    V(90,48,48,D8), V(06,03,03,05), V(F7,F6,F6,01), V(1C,0E,0E,12), \
    V(C2,61,61,A3), V(6A,35,35,5F), V(AE,57,57,F9), V(69,B9,B9,D0), \
    V(17,86,86,91), V(99,C1,C1,58), V(3A,1D,1D,27), V(27,9E,9E,B9), \
    V(D9,E1,E1,38), V(EB,F8,F8,13), V(2B,98,98,B3), V(22,11,11,33), \
    V(D2,69,69,BB), V(A9,D9,D9,70), V(07,8E,8E,89), V(33,94,94,A7), \
    V(2D,9B,9B,B6), V(3C,1E,1E,22), V(15,87,87,92), V(C9,E9,E9,20), \
    V(87,CE,CE,49), V(AA,55,55,FF), V(50,28,28,78), V(A5,DF,DF,7A), \
    V(03,8C,8C,8F), V(59,A1,A1,F8), V(09,89,89,80), V(1A,0D,0D,17), \
    V(65,BF,BF,DA), V(D7,E6,E6,31), V(84,42,42,C6), V(D0,68,68,B8), \
    V(82,41,41,C3), V(29,99,99,B0), V(5A,2D,2D,77), V(1E,0F,0F,11), \
    V(7B,B0,B0,CB), V(A8,54,54,FC), V(6D,BB,BB,D6), V(2C,16,16,3A)

#define V(a,b,c,d) 0x##a##b##c##d
static const uint32_t FT0[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
static const uint32_t FT1[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
static const uint32_t FT2[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
static const uint32_t FT3[256] = { FT };
#undef V

#undef FT

/* reverse tables */

#define RT \
    V(51,F4,A7,50), V(7E,41,65,53), V(1A,17,A4,C3), V(3A,27,5E,96), \
    V(3B,AB,6B,CB), V(1F,9D,45,F1), V(AC,FA,58,AB), V(4B,E3,03,93), \
    V(20,30,FA,55), V(AD,76,6D,F6), V(88,CC,76,91), V(F5,02,4C,25), \
    V(4F,E5,D7,FC), V(C5,2A,CB,D7), V(26,35,44,80), V(B5,62,A3,8F), \
    V(DE,B1,5A,49), V(25,BA,1B,67), V(45,EA,0E,98), V(5D,FE,C0,E1), \
    V(C3,2F,75,02), V(81,4C,F0,12), V(8D,46,97,A3), V(6B,D3,F9,C6), \
    V(03,8F,5F,E7), V(15,92,9C,95), V(BF,6D,7A,EB), V(95,52,59,DA), \
    V(D4,BE,83,2D), V(58,74,21,D3), V(49,E0,69,29), V(8E,C9,C8,44), \
    V(75,C2,89,6A), V(F4,8E,79,78), V(99,58,3E,6B), V(27,B9,71,DD), \
    V(BE,E1,4F,B6), V(F0,88,AD,17), V(C9,20,AC,66), V(7D,CE,3A,B4), \
    V(63,DF,4A,18), V(E5,1A,31,82), V(97,51,33,60), V(62,53,7F,45), \
    V(B1,64,77,E0), V(BB,6B,AE,84), V(FE,81,A0,1C), V(F9,08,2B,94), \
    V(70,48,68,58), V(8F,45,FD,19), V(94,DE,6C,87), V(52,7B,F8,B7), \
    V(AB,73,D3,23), V(72,4B,02,E2), V(E3,1F,8F,57), V(66,55,AB,2A), \
    V(B2,EB,28,07), V(2F,B5,C2,03), V(86,C5,7B,9A), V(D3,37,08,A5), \
    V(30,28,87,F2), V(23,BF,A5,B2), V(02,03,6A,BA), V(ED,16,82,5C), \
    V(8A,CF,1C,2B), V(A7,79,B4,92), V(F3,07,F2,F0), V(4E,69,E2,A1), \
    V(65,DA,F4,CD), V(06,05,BE,D5), V(D1,34,62,1F), V(C4,A6,FE,8A), \
    V(34,2E,53,9D), V(A2,F3,55,A0), V(05,8A,E1,32), V(A4,F6,EB,75), \
    V(0B,83,EC,39), V(40,60,EF,AA), V(5E,71,9F,06), V(BD,6E,10,51), \
    V(3E,21,8A,F9), V(96,DD,06,3D), V(DD,3E,05,AE), V(4D,E6,BD,46), \
    V(91,54,8D,B5), V(71,C4,5D,05), V(04,06,D4,6F), V(60,50,15,FF), \
    V(19,98,FB,24), V(D6,BD,E9,97), V(89,40,43,CC), V(67,D9,9E,77), \
    V(B0,E8,42,BD), V(07,89,8B,88), V(E7,19,5B,38), V(79,C8,EE,DB), \
    V(A1,7C,0A,47), V(7C,42,0F,E9), V(F8,84,1E,C9), V(00,00,00,00), \
    V(09,80,86,83), V(32,2B,ED,48), V(1E,11,70,AC), V(6C,5A,72,4E), \
    V(FD,0E,FF,FB), V(0F,85,38,56), V(3D,AE,D5,1E), V(36,2D,39,27), \
    V(0A,0F,D9,64), V(68,5C,A6,21), V(9B,5B,54,D1), V(24,36,2E,3A), \
    V(0C,0A,67,B1), V(93,57,E7,0F), V(B4,EE,96,D2), V(1B,9B,91,9E), \
    V(80,C0,C5,4F), V(61,DC,20,A2), V(5A,77,4B,69), V(1C,12,1A,16), \
    V(E2,93,BA,0A), V(C0,A0,2A,E5), V(3C,22,E0,43), V(12,1B,17,1D), \
    V(0E,09,0D,0B), V(F2,8B,C7,AD), V(2D,B6,A8,B9), V(14,1E,A9,C8), \
    V(57,F1,19,85), V(AF,75,07,4C), V(EE,99,DD,BB), V(A3,7F,60,FD), \
    V(F7,01,26,9F), V(5C,72,F5,BC), V(44,66,3B,C5), V(5B,FB,7E,34), \
    V(8B,43,29,76), V(CB,23,C6,DC), V(B6,ED,FC,68), V(B8,E4,F1,63), \
    V(D7,31,DC,CA), V(42,63,85,10), V(13,97,22,40), V(84,C6,11,20), \
    V(85,4A,24,7D), V(D2,BB,3D,F8), V(AE,F9,32,11), V(C7,29,A1,6D), \
    V(1D,9E,2F,4B), V(DC,B2,30,F3), V(0D,86,52,EC), V(77,C1,E3,D0), \
    V(2B,B3,16,6C), V(A9,70,B9,99), V(11,94,48,FA), V(47,E9,64,22), \
    V(A8,FC,8C,C4), V(A0,F0,3F,1A), V(56,7D,2C,D8), V(22,33,90,EF), \
    V(87,49,4E,C7), V(D9,38,D1,C1), V(8C,CA,A2,FE), V(98,D4,0B,36), \
    V(A6,F5,81,CF), V(A5,7A,DE,28), V(DA,B7,8E,26), V(3F,AD,BF,A4), \
    V(2C,3A,9D,E4), V(50,78,92,0D), V(6A,5F,CC,9B), V(54,7E,46,62), \
    V(F6,8D,13,C2), V(90,D8,B8,E8), V(2E,39,F7,5E), V(82,C3,AF,F5), \
    V(9F,5D,80,BE), V(69,D0,93,7C), V(6F,D5,2D,A9), V(CF,25,12,B3), \
    V(C8,AC,99,3B), V(10,18,7D,A7), V(E8,9C,63,6E), V(DB,3B,BB,7B), \
    V(CD,26,78,09), V(6E,59,18,F4), V(EC,9A,B7,01), V(83,4F,9A,A8), \
    V(E6,95,6E,65), V(AA,FF,E6,7E), V(21,BC,CF,08), V(EF,15,E8,E6), \
    V(BA,E7,9B,D9), V(4A,6F,36,CE), V(EA,9F,09,D4), V(29,B0,7C,D6), \
    V(31,A4,B2,AF), V(2A,3F,23,31), V(C6,A5,94,30), V(35,A2,66,C0), \
    V(74,4E,BC,37), V(FC,82,CA,A6), V(E0,90,D0,B0), V(33,A7,D8,15), \
    V(F1,04,98,4A), V(41,EC,DA,F7), V(7F,CD,50,0E), V(17,91,F6,2F), \
    V(76,4D,D6,8D), V(43,EF,B0,4D), V(CC,AA,4D,54), V(E4,96,04,DF), \
    V(9E,D1,B5,E3), V(4C,6A,88,1B), V(C1,2C,1F,B8), V(46,65,51,7F), \
    V(9D,5E,EA,04), V(01,8C,35,5D), V(FA,87,74,73), V(FB,0B,41,2E), \
    V(B3,67,1D,5A), V(92,DB,D2,52), V(E9,10,56,33), V(6D,D6,47,13), \
    V(9A,D7,61,8C), V(37,A1,0C,7A), V(59,F8,14,8E), V(EB,13,3C,89), \
    V(CE,A9,27,EE), V(B7,61,C9,35), V(E1,1C,E5,ED), V(7A,47,B1,3C), \
    V(9C,D2,DF,59), V(55,F2,73,3F), V(18,14,CE,79), V(73,C7,37,BF), \
    V(53,F7,CD,EA), V(5F,FD,AA,5B), V(DF,3D,6F,14), V(78,44,DB,86), \
    V(CA,AF,F3,81), V(B9,68,C4,3E), V(38,24,34,2C), V(C2,A3,40,5F), \
    V(16,1D,C3,72), V(BC,E2,25,0C), V(28,3C,49,8B), V(FF,0D,95,41), \
    V(39,A8,01,71), V(08,0C,B3,DE), V(D8,B4,E4,9C), V(64,56,C1,90), \
    V(7B,CB,84,61), V(D5,32,B6,70), V(48,6C,5C,74), V(D0,B8,57,42)

#define V(a,b,c,d) 0x##a##b##c##d
static const uint32_t RT0[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
static const uint32_t RT1[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
static const uint32_t RT2[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
static const uint32_t RT3[256] = { RT };
#undef V

#undef RT

#define UT \
	V(00,00,00,00), V(0e,09,0d,0b), V(1c,12,1a,16), V(12,1b,17,1d), \
	V(38,24,34,2c), V(36,2d,39,27), V(24,36,2e,3a), V(2a,3f,23,31), \
	V(70,48,68,58), V(7e,41,65,53), V(6c,5a,72,4e), V(62,53,7f,45), \
	V(48,6c,5c,74), V(46,65,51,7f), V(54,7e,46,62), V(5a,77,4b,69), \
	V(e0,90,d0,b0), V(ee,99,dd,bb), V(fc,82,ca,a6), V(f2,8b,c7,ad), \
	V(d8,b4,e4,9c), V(d6,bd,e9,97), V(c4,a6,fe,8a), V(ca,af,f3,81), \
	V(90,d8,b8,e8), V(9e,d1,b5,e3), V(8c,ca,a2,fe), V(82,c3,af,f5), \
	V(a8,fc,8c,c4), V(a6,f5,81,cf), V(b4,ee,96,d2), V(ba,e7,9b,d9), \
	V(db,3b,bb,7b), V(d5,32,b6,70), V(c7,29,a1,6d), V(c9,20,ac,66), \
	V(e3,1f,8f,57), V(ed,16,82,5c), V(ff,0d,95,41), V(f1,04,98,4a), \
	V(ab,73,d3,23), V(a5,7a,de,28), V(b7,61,c9,35), V(b9,68,c4,3e), \
	V(93,57,e7,0f), V(9d,5e,ea,04), V(8f,45,fd,19), V(81,4c,f0,12), \
	V(3b,ab,6b,cb), V(35,a2,66,c0), V(27,b9,71,dd), V(29,b0,7c,d6), \
	V(03,8f,5f,e7), V(0d,86,52,ec), V(1f,9d,45,f1), V(11,94,48,fa), \
	V(4b,e3,03,93), V(45,ea,0e,98), V(57,f1,19,85), V(59,f8,14,8e), \
	V(73,c7,37,bf), V(7d,ce,3a,b4), V(6f,d5,2d,a9), V(61,dc,20,a2), \
	V(ad,76,6d,f6), V(a3,7f,60,fd), V(b1,64,77,e0), V(bf,6d,7a,eb), \
	V(95,52,59,da), V(9b,5b,54,d1), V(89,40,43,cc), V(87,49,4e,c7), \
	V(dd,3e,05,ae), V(d3,37,08,a5), V(c1,2c,1f,b8), V(cf,25,12,b3), \
	V(e5,1a,31,82), V(eb,13,3c,89), V(f9,08,2b,94), V(f7,01,26,9f), \
	V(4d,e6,bd,46), V(43,ef,b0,4d), V(51,f4,a7,50), V(5f,fd,aa,5b), \
	V(75,c2,89,6a), V(7b,cb,84,61), V(69,d0,93,7c), V(67,d9,9e,77), \
	V(3d,ae,d5,1e), V(33,a7,d8,15), V(21,bc,cf,08), V(2f,b5,c2,03), \
	V(05,8a,e1,32), V(0b,83,ec,39), V(19,98,fb,24), V(17,91,f6,2f), \
	V(76,4d,d6,8d), V(78,44,db,86), V(6a,5f,cc,9b), V(64,56,c1,90), \
	V(4e,69,e2,a1), V(40,60,ef,aa), V(52,7b,f8,b7), V(5c,72,f5,bc), \
	V(06,05,be,d5), V(08,0c,b3,de), V(1a,17,a4,c3), V(14,1e,a9,c8), \
	V(3e,21,8a,f9), V(30,28,87,f2), V(22,33,90,ef), V(2c,3a,9d,e4), \
	V(96,dd,06,3d), V(98,d4,0b,36), V(8a,cf,1c,2b), V(84,c6,11,20), \
	V(ae,f9,32,11), V(a0,f0,3f,1a), V(b2,eb,28,07), V(bc,e2,25,0c), \
	V(e6,95,6e,65), V(e8,9c,63,6e), V(fa,87,74,73), V(f4,8e,79,78), \
	V(de,b1,5a,49), V(d0,b8,57,42), V(c2,a3,40,5f), V(cc,aa,4d,54), \
	V(41,ec,da,f7), V(4f,e5,d7,fc), V(5d,fe,c0,e1), V(53,f7,cd,ea), \
	V(79,c8,ee,db), V(77,c1,e3,d0), V(65,da,f4,cd), V(6b,d3,f9,c6), \
	V(31,a4,b2,af), V(3f,ad,bf,a4), V(2d,b6,a8,b9), V(23,bf,a5,b2), \
	V(09,80,86,83), V(07,89,8b,88), V(15,92,9c,95), V(1b,9b,91,9e), \
	V(a1,7c,0a,47), V(af,75,07,4c), V(bd,6e,10,51), V(b3,67,1d,5a), \
	V(99,58,3e,6b), V(97,51,33,60), V(85,4a,24,7d), V(8b,43,29,76), \
	V(d1,34,62,1f), V(df,3d,6f,14), V(cd,26,78,09), V(c3,2f,75,02), \
	V(e9,10,56,33), V(e7,19,5b,38), V(f5,02,4c,25), V(fb,0b,41,2e), \
	V(9a,d7,61,8c), V(94,de,6c,87), V(86,c5,7b,9a), V(88,cc,76,91), \
	V(a2,f3,55,a0), V(ac,fa,58,ab), V(be,e1,4f,b6), V(b0,e8,42,bd), \
	V(ea,9f,09,d4), V(e4,96,04,df), V(f6,8d,13,c2), V(f8,84,1e,c9), \
	V(d2,bb,3d,f8), V(dc,b2,30,f3), V(ce,a9,27,ee), V(c0,a0,2a,e5), \
	V(7a,47,b1,3c), V(74,4e,bc,37), V(66,55,ab,2a), V(68,5c,a6,21), \
	V(42,63,85,10), V(4c,6a,88,1b), V(5e,71,9f,06), V(50,78,92,0d), \
	V(0a,0f,d9,64), V(04,06,d4,6f), V(16,1d,c3,72), V(18,14,ce,79), \
	V(32,2b,ed,48), V(3c,22,e0,43), V(2e,39,f7,5e), V(20,30,fa,55), \
	V(ec,9a,b7,01), V(e2,93,ba,0a), V(f0,88,ad,17), V(fe,81,a0,1c), \
	V(d4,be,83,2d), V(da,b7,8e,26), V(c8,ac,99,3b), V(c6,a5,94,30), \
	V(9c,d2,df,59), V(92,db,d2,52), V(80,c0,c5,4f), V(8e,c9,c8,44), \
	V(a4,f6,eb,75), V(aa,ff,e6,7e), V(b8,e4,f1,63), V(b6,ed,fc,68), \
	V(0c,0a,67,b1), V(02,03,6a,ba), V(10,18,7d,a7), V(1e,11,70,ac), \
	V(34,2e,53,9d), V(3a,27,5e,96), V(28,3c,49,8b), V(26,35,44,80), \
	V(7c,42,0f,e9), V(72,4b,02,e2), V(60,50,15,ff), V(6e,59,18,f4), \
	V(44,66,3b,c5), V(4a,6f,36,ce), V(58,74,21,d3), V(56,7d,2c,d8), \
	V(37,a1,0c,7a), V(39,a8,01,71), V(2b,b3,16,6c), V(25,ba,1b,67), \
	V(0f,85,38,56), V(01,8c,35,5d), V(13,97,22,40), V(1d,9e,2f,4b), \
	V(47,e9,64,22), V(49,e0,69,29), V(5b,fb,7e,34), V(55,f2,73,3f), \
	V(7f,cd,50,0e), V(71,c4,5d,05), V(63,df,4a,18), V(6d,d6,47,13), \
	V(d7,31,dc,ca), V(d9,38,d1,c1), V(cb,23,c6,dc), V(c5,2a,cb,d7), \
	V(ef,15,e8,e6), V(e1,1c,e5,ed), V(f3,07,f2,f0), V(fd,0e,ff,fb), \
	V(a7,79,b4,92), V(a9,70,b9,99), V(bb,6b,ae,84), V(b5,62,a3,8f), \
	V(9f,5d,80,be), V(91,54,8d,b5), V(83,4f,9a,a8), V(8d,46,97,a3)

#define V(a,b,c,d) 0x##a##b##c##d
static const uint32_t U0[256] = { UT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
static const uint32_t U1[256] = { UT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
static const uint32_t U2[256] = { UT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
static const uint32_t U3[256] = { UT };
#undef V
#undef UT

static const uint8_t Rcon[30] =
{
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xc0, 
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 
	0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 
	0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91
};

#ifdef _AES_KEY_STATIC

//Expand a user-supplied key material into a session key.
// key        - The 128/192/256-bit user-key to use.

void AESCreateSessionKeyStaticKey(UINT8 *key, void *session)
{
    UINT32 tk[Nk], tt; 
    int idx = 0, t = 0;
    int i, j, r;
    AES_SESSION_KEY *session_key;

    session_key = (AES_SESSION_KEY *)session;

	for( i = 0; i <= Nr; i++ )
		for(j=0; j<Nb; j++)
			session_key->key[0][i][j] = 0;

	for( i = 0; i <= Nr; i++)
		for(j=0; j<Nb; j++)
			session_key->key[1][i][j] = 0;

	//Copy user material bytes into temporary ints
	for( i = 0; i < Nk; i++ )
	{
		tk[i] = *key++ << 24;
		tk[i] |= *key++ << 16;
		tk[i] |= *key++ << 8;
		tk[i] |= *key++;
	}

	//Copy values into round key arrays
	for( j = 0; j < Nk && t < ROUND_KEY_COUNT; j++, t++ )
	{
		session_key->key[0][t/Nb][t%Nb] = tk[j];
		session_key->key[1][Nr - (t/Nb)][t%Nb] = tk[j];
	}

	while( t < ROUND_KEY_COUNT )
	{
		//Extrapolate using phi (the round key evolution function)
		tt = tk[Nk-1];
		tk[0] ^= Sbox[(UINT8)(tt >> 16)] << 24 ^
			Sbox[(UINT8)(tt >>  8)] << 16 ^
			Sbox[(UINT8) tt] <<  8 ^
			Sbox[(UINT8)(tt >> 24)] ^
			Rcon[idx++] << 24;

        if ( Nk != 8 )
		  for( i=1, j=0; i < Nk; )
			tk[i++] ^= tk[j++];
        else {
          for( i = 1, j = 0; i < Nk / 2; )
		    tk[i++] ^= tk[j++];
          tt = tk[Nk / 2 - 1];
          tk[Nk / 2] ^= Sbox[(UINT8)  tt        ]       ^
                        Sbox[(UINT8)( tt >>  8 )] <<  8 ^
                        Sbox[(UINT8)( tt >> 16 )] << 16 ^
                        Sbox[(UINT8)( tt >> 24 )] << 24;
          for( j = Nk / 2, i = j + 1; i < Nk; )
		    tk[i++] ^= tk[j++];
	    }

		//Copy values into round key arrays
		for( j = 0; j < Nk && t < ROUND_KEY_COUNT; j++, t++ )
		{
			session_key->key[0][t/Nb][t%Nb] = tk[j];
			session_key->key[1][Nr - (t/Nb)][t%Nb] = tk[j];
		}
	}

	//Inverse MixColumn where needed

	for( r = 1; r < Nr; r++ )
 		for( j = 0; j < Nb; j++ )
		{
			tt = session_key->key[1][r][j];
			session_key->key[1][r][j] = U0[(UINT8)(tt >> 24)] ^
				U1[(UINT8)(tt >> 16)] ^
				U2[(UINT8)(tt >>  8)] ^
				U3[(UINT8)tt];
		}
}

//Convenience method to encrypt exactly one block of plaintext, assuming
//Rijndael's default block size (128-bit).
// in         - The plaintext
// result     - The ciphertext generated from a plaintext using the key
void AESECBEncryptBlockStaticKey(UINT8 *data_block, void *session, UINT8 *encrypt_block)
{
    UINT32 t0, t1, t2, t3, tt; 
    UINT32 a0, a1, a2, a3, r;
    AES_SESSION_KEY *session_key;

    session_key = (AES_SESSION_KEY *)session;

	t0 = *data_block++ << 24;
	t0 |= *data_block++ << 16;
	t0 |= *data_block++ << 8;
	t0 |= *data_block++;
	t0 ^= session_key->key[0][0][0];
	
	
	t1 = *data_block++ << 24;
	t1 |= *data_block++ << 16;
	t1 |= *data_block++ << 8;
	t1 |= *data_block++;
	t1 ^= session_key->key[0][0][1];
	
	t2 = *data_block++ << 24;
	t2 |= *data_block++ << 16;
	t2 |= *data_block++ << 8;
	t2 |= *data_block++;
	t2 ^= session_key->key[0][0][2];
	
	t3 = *data_block++ << 24;
	t3 |= *data_block++ << 16;
	t3 |= *data_block++ << 8;
	t3 |= *data_block++;
	t3 ^= session_key->key[0][0][3];

	//Apply Round Transforms
	for( r = 1; r < Nr; r++ )
	{
		a0 = (FT0[(UINT8)(t0 >> 24)] ^
			FT1[(UINT8)(t1 >> 16)] ^
			FT2[(UINT8)(t2 >>  8)] ^
			FT3[(UINT8)t3]);
		a1 = (FT0[(UINT8)(t1 >> 24)] ^
			FT1[(UINT8)(t2 >> 16)] ^
			FT2[(UINT8)(t3 >>  8)] ^
			FT3[(UINT8)t0]);
		a2 = (FT0[(UINT8)(t2 >> 24)] ^
			FT1[(UINT8)(t3 >> 16)] ^
			FT2[(UINT8)(t0 >>  8)] ^
			FT3[(UINT8)t1]);
		a3 = (FT0[(UINT8)(t3 >> 24)] ^
			FT1[(UINT8)(t0 >> 16)] ^
			FT2[(UINT8)(t1 >>  8)] ^
			FT3[(UINT8)t2]);
		t0 = a0 ^ session_key->key[0][r][0];
		t1 = a1 ^ session_key->key[0][r][1];
		t2 = a2 ^ session_key->key[0][r][2];
		t3 = a3 ^ session_key->key[0][r][3];
	}

	//Last Round is special

	tt = session_key->key[0][Nr][0];
	encrypt_block[0] = Sbox[(UINT8)(t0 >> 24)] ^ (UINT8)(tt >> 24);
	encrypt_block[1] = Sbox[(UINT8)(t1 >> 16)] ^ (UINT8)(tt >> 16);
	encrypt_block[2] = Sbox[(UINT8)(t2 >>  8)] ^ (UINT8)(tt >>  8);
	encrypt_block[3] = Sbox[(UINT8)t3] ^ (UINT8)tt;

	tt = session_key->key[0][Nr][1];
	encrypt_block[4] = Sbox[(UINT8)(t1 >> 24)] ^ (UINT8)(tt >> 24);
	encrypt_block[5] = Sbox[(UINT8)(t2 >> 16)] ^ (UINT8)(tt >> 16);
	encrypt_block[6] = Sbox[(UINT8)(t3 >>  8)] ^ (UINT8)(tt >>  8);
	encrypt_block[7] = Sbox[(UINT8)t0] ^ (UINT8)tt;

	tt = session_key->key[0][Nr][2];
	encrypt_block[8] = Sbox[(UINT8)(t2 >> 24)] ^ (UINT8)(tt >> 24);
	encrypt_block[9] = Sbox[(UINT8)(t3 >> 16)] ^ (UINT8)(tt >> 16);
	encrypt_block[10] = Sbox[(UINT8)(t0 >>  8)] ^ (UINT8)(tt >>  8);
	encrypt_block[11] = Sbox[(UINT8)t1] ^ (UINT8)tt;

	tt = session_key->key[0][Nr][3];
	encrypt_block[12] = Sbox[(UINT8)(t3 >> 24)] ^ (UINT8)(tt >> 24);
	encrypt_block[13] = Sbox[(UINT8)(t0 >> 16)] ^ (UINT8)(tt >> 16);
	encrypt_block[14] = Sbox[(UINT8)(t1 >>  8)] ^ (UINT8)(tt >>  8);
	encrypt_block[15] = Sbox[(UINT8)t2] ^ (UINT8)tt;
}

//Convenience method to decrypt exactly one block of plaintext, assuming
//Rijndael's default block size (128-bit).
// in         - The ciphertext.
// result     - The plaintext generated from a ciphertext using the session key.
void AESECBDecryptBlockStaticKey(UINT8 *encrypt_block, void *session, UINT8 *decrypt_block)
{
    UINT32 t0, t1, t2, t3, tt; 
    UINT32 a0, a1, a2, a3, r;
    AES_SESSION_KEY *session_key;

    session_key = (AES_SESSION_KEY *)session;

	t0 = *encrypt_block++ << 24;
	t0 |= *encrypt_block++ << 16;
	t0 |= *encrypt_block++ << 8;
	t0 |= *encrypt_block++;
	t0 ^= session_key->key[1][0][0];
	
	t1 = *encrypt_block++ << 24;
	t1 |= *encrypt_block++ << 16;
	t1 |= *encrypt_block++ << 8;
	t1 |= *encrypt_block++;
	t1 ^= session_key->key[1][0][1];
	
	t2 = *encrypt_block++ << 24;
	t2 |= *encrypt_block++ << 16;
	t2 |= *encrypt_block++ << 8;
	t2 |= *encrypt_block++;
	t2 ^= session_key->key[1][0][2];
	
	t3 = *encrypt_block++ << 24;
	t3 |= *encrypt_block++ << 16;
	t3 |= *encrypt_block++ << 8;
	t3 |= *encrypt_block++;
	t3 ^= session_key->key[1][0][3];
	
	for( r = 1; r < Nr; r++ ) // apply round transforms
	{
		a0 = (RT0[(UINT8)(t0 >> 24)] ^
			RT1[(UINT8)(t3 >> 16)] ^
			RT2[(UINT8)(t2 >>  8)] ^
			RT3[(UINT8) t1       ] );
		a1 = (RT0[(UINT8)(t1 >> 24)] ^
			RT1[(UINT8)(t0 >> 16)] ^
			RT2[(UINT8)(t3 >>  8)] ^
			RT3[(UINT8) t2       ] );
		a2 = (RT0[(UINT8)(t2 >> 24)] ^
			RT1[(UINT8)(t1 >> 16)] ^
			RT2[(UINT8)(t0 >>  8)] ^
			RT3[(UINT8) t3       ] );
		a3 = (RT0[(UINT8)(t3 >> 24)] ^
			RT1[(UINT8)(t2 >> 16)] ^
			RT2[(UINT8)(t1 >>  8)] ^
			RT3[(UINT8) t0       ] );
		t0 = a0 ^ session_key->key[1][r][0];
		t1 = a1 ^ session_key->key[1][r][1];
		t2 = a2 ^ session_key->key[1][r][2];
		t3 = a3 ^ session_key->key[1][r][3];
	}

	//Last Round is special
	tt = session_key->key[1][Nr][0];
	decrypt_block[ 0] = InvSbox[(UINT8)(t0 >> 24)] ^ (UINT8)(tt >> 24);
	decrypt_block[ 1] = InvSbox[(UINT8)(t3 >> 16)] ^ (UINT8)(tt >> 16);
	decrypt_block[ 2] = InvSbox[(UINT8)(t2 >>  8)] ^ (UINT8)(tt >>  8);
	decrypt_block[ 3] = InvSbox[(UINT8) t1] ^ (UINT8)tt;

	tt = session_key->key[1][Nr][1];
	decrypt_block[ 4] = InvSbox[(UINT8)(t1 >> 24)] ^ (UINT8)(tt >> 24);
	decrypt_block[ 5] = InvSbox[(UINT8)(t0 >> 16)] ^ (UINT8)(tt >> 16);
	decrypt_block[ 6] = InvSbox[(UINT8)(t3 >>  8)] ^ (UINT8)(tt >>  8);
	decrypt_block[ 7] = InvSbox[(UINT8) t2] ^ (UINT8)tt;

	tt = session_key->key[1][Nr][2];
	decrypt_block[ 8] = InvSbox[(UINT8)(t2 >> 24)] ^ (UINT8)(tt >> 24);
	decrypt_block[ 9] = InvSbox[(UINT8)(t1 >> 16)] ^ (UINT8)(tt >> 16);
	decrypt_block[10] = InvSbox[(UINT8)(t0 >>  8)] ^ (UINT8)(tt >>  8);
	decrypt_block[11] = InvSbox[(UINT8) t3] ^ (UINT8)tt;

	tt = session_key->key[1][Nr][3];
	decrypt_block[12] = InvSbox[(UINT8)(t3 >> 24)] ^ (UINT8)(tt >> 24);
	decrypt_block[13] = InvSbox[(UINT8)(t2 >> 16)] ^ (UINT8)(tt >> 16);
	decrypt_block[14] = InvSbox[(UINT8)(t1 >>  8)] ^ (UINT8)(tt >>  8);
	decrypt_block[15] = InvSbox[(UINT8) t0] ^ (UINT8)tt;
}
#else

#define Nb 4			                                // number of columns in the state & expanded key

//Expand a user-supplied key material into a session key.
// key        - The 128/192/256-bit user-key to use.

void AESCreateRoundKeys(uint8_t *session, void *key, AES_KEY_TYPE key_type)
{
    unsigned long *p = (unsigned long*)session;

    //Save the key type in the first 32bits of the session pointer
    *p++ = (unsigned long) key_type;

    //Add 4 to the session pointer to skip past the key_type field to the actual
    //  session data.
    AESCreateSessionKey(key, session+4, key_type);
}

void AESCreateSessionKey(uint8_t *key, void *session, AES_KEY_TYPE key_type)
{
    uint32_t tk[8], tt; 
    int idx = 0, t = 0;
    int i, j, r;
    int Nk, Nr;
    AES_SESSION_KEY *session_key;

    session_key = (AES_SESSION_KEY *)session;

	switch(key_type)
    {
    case AES_KEY_TYPE_128_BIT:
        Nr = 10;
        Nk = 4;
        break;
    case AES_KEY_TYPE_192_BIT:
        Nr = 12;
        Nk = 6;
        break;
    case AES_KEY_TYPE_256_BIT:
        Nr = 14;
        Nk = 8;
        break;
    default:
        return;
    }

    for( i = 0; i <= Nr; i++ )
		for(j=0; j<Nb; j++)
			session_key->key[0][i][j] = 0;

	for( i = 0; i <= Nr; i++)
		for(j=0; j<Nb; j++)
			session_key->key[1][i][j] = 0;

	//Copy user material bytes into temporary ints
	for( i = 0; i < Nk; i++ )
	{
		tk[i] = *key++ << 24;
		tk[i] |= *key++ << 16;
		tk[i] |= *key++ << 8;
		tk[i] |= *key++;
	}

	//Copy values into round key arrays
	for( j = 0; j < Nk && t < ROUND_KEY_COUNT; j++, t++ )
	{
		session_key->key[0][t/Nb][t%Nb] = tk[j];
		session_key->key[1][Nr - (t/Nb)][t%Nb] = tk[j];
	}

	while( t < ROUND_KEY_COUNT )
	{
		//Extrapolate using phi (the round key evolution function)
		tt = tk[Nk-1];
		tk[0] ^= Sbox[(uint8_t)(tt >> 16)] << 24 ^
			Sbox[(uint8_t)(tt >>  8)] << 16 ^
			Sbox[(uint8_t) tt] <<  8 ^
			Sbox[(uint8_t)(tt >> 24)] ^
			Rcon[idx++] << 24;

        if ( Nk != 8 )
		  for( i=1, j=0; i < Nk; )
			tk[i++] ^= tk[j++];
        else {
          for( i = 1, j = 0; i < Nk / 2; )
		    tk[i++] ^= tk[j++];
          tt = tk[Nk / 2 - 1];
          tk[Nk / 2] ^= Sbox[(uint8_t)  tt        ]       ^
                        Sbox[(uint8_t)( tt >>  8 )] <<  8 ^
                        Sbox[(uint8_t)( tt >> 16 )] << 16 ^
                        Sbox[(uint8_t)( tt >> 24 )] << 24;
          for( j = Nk / 2, i = j + 1; i < Nk; )
		    tk[i++] ^= tk[j++];
	    }

		//Copy values into round key arrays
		for( j = 0; j < Nk && t < ROUND_KEY_COUNT; j++, t++ )
		{
			session_key->key[0][t/Nb][t%Nb] = tk[j];
			session_key->key[1][Nr - (t/Nb)][t%Nb] = tk[j];
		}
	}

	//Inverse MixColumn where needed

	for( r = 1; r < Nr; r++ )
 		for( j = 0; j < Nb; j++ )
		{
			tt = session_key->key[1][r][j];
			session_key->key[1][r][j] = U0[(uint8_t)(tt >> 24)] ^
				U1[(uint8_t)(tt >> 16)] ^
				U2[(uint8_t)(tt >>  8)] ^
				U3[(uint8_t)tt];
		}
}

//Convenience method to encrypt exactly one block of plaintext, assuming
//Rijndael's default block size (128-bit).
// in         - The plaintext
// result     - The ciphertext generated from a plaintext using the key
void AESECBEncryptBlock(uint8_t *plain_text, void *session, uint8_t *cipher_text, AES_KEY_TYPE key_type)
{
    uint32_t t0, t1, t2, t3, tt; 
    uint32_t a0, a1, a2, a3, r;
    AES_SESSION_KEY *session_key;
    int Nr;

    session_key = (AES_SESSION_KEY *)session;

	switch(key_type)
    {
    case AES_KEY_TYPE_128_BIT:
        Nr = 10;
        break;
    case AES_KEY_TYPE_192_BIT:
        Nr = 12;
        break;
    case AES_KEY_TYPE_256_BIT:
        Nr = 14;
        break;
    default:
        return;
    }

	t0 = *plain_text++ << 24;
	t0 |= *plain_text++ << 16;
	t0 |= *plain_text++ << 8;
	t0 |= *plain_text++;
	t0 ^= session_key->key[0][0][0];
	
	
	t1 = *plain_text++ << 24;
	t1 |= *plain_text++ << 16;
	t1 |= *plain_text++ << 8;
	t1 |= *plain_text++;
	t1 ^= session_key->key[0][0][1];
	
	t2 = *plain_text++ << 24;
	t2 |= *plain_text++ << 16;
	t2 |= *plain_text++ << 8;
	t2 |= *plain_text++;
	t2 ^= session_key->key[0][0][2];
	
	t3 = *plain_text++ << 24;
	t3 |= *plain_text++ << 16;
	t3 |= *plain_text++ << 8;
	t3 |= *plain_text++;
	t3 ^= session_key->key[0][0][3];

	//Apply Round Transforms
	for( r = 1; r < Nr; r++ )
	{
		a0 = (FT0[(uint8_t)(t0 >> 24)] ^
			FT1[(uint8_t)(t1 >> 16)] ^
			FT2[(uint8_t)(t2 >>  8)] ^
			FT3[(uint8_t)t3]);
		a1 = (FT0[(uint8_t)(t1 >> 24)] ^
			FT1[(uint8_t)(t2 >> 16)] ^
			FT2[(uint8_t)(t3 >>  8)] ^
			FT3[(uint8_t)t0]);
		a2 = (FT0[(uint8_t)(t2 >> 24)] ^
			FT1[(uint8_t)(t3 >> 16)] ^
			FT2[(uint8_t)(t0 >>  8)] ^
			FT3[(uint8_t)t1]);
		a3 = (FT0[(uint8_t)(t3 >> 24)] ^
			FT1[(uint8_t)(t0 >> 16)] ^
			FT2[(uint8_t)(t1 >>  8)] ^
			FT3[(uint8_t)t2]);
		t0 = a0 ^ session_key->key[0][r][0];
		t1 = a1 ^ session_key->key[0][r][1];
		t2 = a2 ^ session_key->key[0][r][2];
		t3 = a3 ^ session_key->key[0][r][3];
	}

	//Last Round is special

	tt = session_key->key[0][Nr][0];
	cipher_text[0] = Sbox[(uint8_t)(t0 >> 24)] ^ (uint8_t)(tt >> 24);
	cipher_text[1] = Sbox[(uint8_t)(t1 >> 16)] ^ (uint8_t)(tt >> 16);
	cipher_text[2] = Sbox[(uint8_t)(t2 >>  8)] ^ (uint8_t)(tt >>  8);
	cipher_text[3] = Sbox[(uint8_t)t3] ^ (uint8_t)tt;

	tt = session_key->key[0][Nr][1];
	cipher_text[4] = Sbox[(uint8_t)(t1 >> 24)] ^ (uint8_t)(tt >> 24);
	cipher_text[5] = Sbox[(uint8_t)(t2 >> 16)] ^ (uint8_t)(tt >> 16);
	cipher_text[6] = Sbox[(uint8_t)(t3 >>  8)] ^ (uint8_t)(tt >>  8);
	cipher_text[7] = Sbox[(uint8_t)t0] ^ (uint8_t)tt;

	tt = session_key->key[0][Nr][2];
	cipher_text[8] = Sbox[(uint8_t)(t2 >> 24)] ^ (uint8_t)(tt >> 24);
	cipher_text[9] = Sbox[(uint8_t)(t3 >> 16)] ^ (uint8_t)(tt >> 16);
	cipher_text[10] = Sbox[(uint8_t)(t0 >>  8)] ^ (uint8_t)(tt >>  8);
	cipher_text[11] = Sbox[(uint8_t)t1] ^ (uint8_t)tt;

	tt = session_key->key[0][Nr][3];
	cipher_text[12] = Sbox[(uint8_t)(t3 >> 24)] ^ (uint8_t)(tt >> 24);
	cipher_text[13] = Sbox[(uint8_t)(t0 >> 16)] ^ (uint8_t)(tt >> 16);
	cipher_text[14] = Sbox[(uint8_t)(t1 >>  8)] ^ (uint8_t)(tt >>  8);
	cipher_text[15] = Sbox[(uint8_t)t2] ^ (uint8_t)tt;
}

//Convenience method to decrypt exactly one block of plaintext, assuming
//Rijndael's default block size (128-bit).
// in         - The ciphertext.
// result     - The plaintext generated from a ciphertext using the session key.
void AESECBDecryptBlock(uint8_t *cipher_text, void *session, uint8_t *plain_text, AES_KEY_TYPE key_type)
{
    uint32_t t0, t1, t2, t3, tt; 
    uint32_t a0, a1, a2, a3, r;
    AES_SESSION_KEY *session_key;
    int Nr;

    session_key = (AES_SESSION_KEY *)session;

	switch(key_type)
    {
    case AES_KEY_TYPE_128_BIT:
        Nr = 10;
        break;
    case AES_KEY_TYPE_192_BIT:
        Nr = 12;
        break;
    case AES_KEY_TYPE_256_BIT:
        Nr = 14;
        break;
    default:
        return;
    }

    t0 = *cipher_text++ << 24;
	t0 |= *cipher_text++ << 16;
	t0 |= *cipher_text++ << 8;
	t0 |= *cipher_text++;
	t0 ^= session_key->key[1][0][0];
	
	t1 = *cipher_text++ << 24;
	t1 |= *cipher_text++ << 16;
	t1 |= *cipher_text++ << 8;
	t1 |= *cipher_text++;
	t1 ^= session_key->key[1][0][1];
	
	t2 = *cipher_text++ << 24;
	t2 |= *cipher_text++ << 16;
	t2 |= *cipher_text++ << 8;
	t2 |= *cipher_text++;
	t2 ^= session_key->key[1][0][2];
	
	t3 = *cipher_text++ << 24;
	t3 |= *cipher_text++ << 16;
	t3 |= *cipher_text++ << 8;
	t3 |= *cipher_text++;
	t3 ^= session_key->key[1][0][3];
	
	for( r = 1; r < Nr; r++ ) // apply round transforms
	{
		a0 = (RT0[(uint8_t)(t0 >> 24)] ^
			RT1[(uint8_t)(t3 >> 16)] ^
			RT2[(uint8_t)(t2 >>  8)] ^
			RT3[(uint8_t) t1       ] );
		a1 = (RT0[(uint8_t)(t1 >> 24)] ^
			RT1[(uint8_t)(t0 >> 16)] ^
			RT2[(uint8_t)(t3 >>  8)] ^
			RT3[(uint8_t) t2       ] );
		a2 = (RT0[(uint8_t)(t2 >> 24)] ^
			RT1[(uint8_t)(t1 >> 16)] ^
			RT2[(uint8_t)(t0 >>  8)] ^
			RT3[(uint8_t) t3       ] );
		a3 = (RT0[(uint8_t)(t3 >> 24)] ^
			RT1[(uint8_t)(t2 >> 16)] ^
			RT2[(uint8_t)(t1 >>  8)] ^
			RT3[(uint8_t) t0       ] );
		t0 = a0 ^ session_key->key[1][r][0];
		t1 = a1 ^ session_key->key[1][r][1];
		t2 = a2 ^ session_key->key[1][r][2];
		t3 = a3 ^ session_key->key[1][r][3];
	}

	//Last Round is special
	tt = session_key->key[1][Nr][0];
	plain_text[ 0] = InvSbox[(uint8_t)(t0 >> 24)] ^ (uint8_t)(tt >> 24);
	plain_text[ 1] = InvSbox[(uint8_t)(t3 >> 16)] ^ (uint8_t)(tt >> 16);
	plain_text[ 2] = InvSbox[(uint8_t)(t2 >>  8)] ^ (uint8_t)(tt >>  8);
	plain_text[ 3] = InvSbox[(uint8_t) t1] ^ (uint8_t)tt;

	tt = session_key->key[1][Nr][1];
	plain_text[ 4] = InvSbox[(uint8_t)(t1 >> 24)] ^ (uint8_t)(tt >> 24);
	plain_text[ 5] = InvSbox[(uint8_t)(t0 >> 16)] ^ (uint8_t)(tt >> 16);
	plain_text[ 6] = InvSbox[(uint8_t)(t3 >>  8)] ^ (uint8_t)(tt >>  8);
	plain_text[ 7] = InvSbox[(uint8_t) t2] ^ (uint8_t)tt;

	tt = session_key->key[1][Nr][2];
	plain_text[ 8] = InvSbox[(uint8_t)(t2 >> 24)] ^ (uint8_t)(tt >> 24);
	plain_text[ 9] = InvSbox[(uint8_t)(t1 >> 16)] ^ (uint8_t)(tt >> 16);
	plain_text[10] = InvSbox[(uint8_t)(t0 >>  8)] ^ (uint8_t)(tt >>  8);
	plain_text[11] = InvSbox[(uint8_t) t3] ^ (uint8_t)tt;

	tt = session_key->key[1][Nr][3];
	plain_text[12] = InvSbox[(uint8_t)(t3 >> 24)] ^ (uint8_t)(tt >> 24);
	plain_text[13] = InvSbox[(uint8_t)(t2 >> 16)] ^ (uint8_t)(tt >> 16);
	plain_text[14] = InvSbox[(uint8_t)(t1 >>  8)] ^ (uint8_t)(tt >>  8);
	plain_text[15] = InvSbox[(uint8_t) t0] ^ (uint8_t)tt;
}

#endif
 
