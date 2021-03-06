/* -*-  Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright(c) 1991-1997 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Computer Systems
 *      Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/* $Id: sha-1.cc 1 2009-10-11 22:06:03Z soohyunc $ */

#include "sha-1.h"
#include <math.h>
#include <sys/types.h>

#define SHA1_MAX_FILE_BUFFER	8000

/*
 * Circular left shift operation, S_n(x)
 * 
 * 	[.] S_n(x) = (x << n) OR (x >> 32-n)
 * 	    ,where n is an integer with 0 <= n < 2^32
 *
 * 	[.] x << n: discard the left-most n bits and pad the result with n zeros
 * 	[.] x >> n: discard the right-most n bits and pad the result with n zeros
 */
static inline uint_32 LEFT_ROT32 (uint_32 _val32, uint_32 _bits) {
	    return (((_val32) << (_bits)) | ((_val32) >> (32 - (_bits))));
}

#define SHABLK0(i) (m_block->l[i])
#define SHABLK(i) (m_block->l[i&15] = LEFT_ROT32(m_block->l[(i+13)&15] ^ m_block->l[(i+8)&15] ^ m_block->l[(i+2)&15] ^ m_block->l[i&15],1))

/* 
 * SHA1 Rounds
 */
#define _R0(v,w,x,y,z,i) { z += ((w&(x^y))^y) + SHABLK0(i) + 0x5A827999 + LEFT_ROT32(v,5); w = LEFT_ROT32(w,30); }
#define _R1(v,w,x,y,z,i) { z += ((w&(x^y))^y) + SHABLK(i) + 0x5A827999 + LEFT_ROT32(v,5); w = LEFT_ROT32(w,30); }
#define _R2(v,w,x,y,z,i) { z += (w^x^y)+SHABLK(i) + 0x6ED9EBA1 + LEFT_ROT32(v,5); w = LEFT_ROT32(w,30); }
#define _R3(v,w,x,y,z,i) { z += (((w|x)&y)|(w&x)) + SHABLK(i) + 0x8F1BBCDC + LEFT_ROT32(v,5); w = LEFT_ROT32(w,30); }
#define _R4(v,w,x,y,z,i) { z += (w^x^y) + SHABLK(i) + 0xCA62C1D6 + LEFT_ROT32(v,5); w = LEFT_ROT32(w,30); }

/*
 * SHA1 Constructor
 */
SHA1::SHA1() {
        m_block = (SHA1_WORKSPACE_BLOCK *) m_workspace;
        Reset();
}

/*
 * SHA1 Destructor
 */
SHA1::~SHA1() {
	Reset();
}

/*
 * SHA1 Reset
 *
 * 	[.] this function resets the class. if you hash only one single data stream,
 * 	    you don't have to call this func manually.
 */
void SHA1::Reset() {
	// SHA1 initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xEFCDAB89;
	m_state[2] = 0x98BADCFE;
	m_state[3] = 0x10325476;
	m_state[4] = 0xC3D2E1F0;

	m_count[0] = 0;
	m_count[1] = 0;
}

/*
 * SHA1 Transform
 * 
 *	[.] Main SHA-1 Operation
 *	[.] Hash a single 512-bit block
 *
 *	[.] This is the core SHA1 algorithm
 */
void SHA1::Transform(uint_32 *state, uint_8 *buffer) {

        // Copy state[] to working vars
        uint_32 a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

        memcpy(m_block, buffer, 64);

        // 4 rounds of 20 operations each. Loop unrolled.
        _R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
        _R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
        _R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
        _R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
        _R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
        _R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
        _R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
        _R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
        _R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
        _R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
        _R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
        _R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
        _R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
        _R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
        _R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
        _R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
        _R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
        _R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
        _R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
        _R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

        // Add the working vars back into state
        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;

        // Wipe variables
#ifdef SHA1_WIPE_VARIABLES
        a = b = c = d = e = 0;
#endif
}

/*
 * Hash value update for binary data and strings
 */
void SHA1::Update(uint_8 *data, uint_32 len) {
        uint_32 i, j;

        j = (m_count[0] >> 3) & 63;

        if((m_count[0] += len << 3) < (len << 3)) m_count[1]++;

        m_count[1] += (len >> 29);

        if((j + len) > 63) {
                i = 64 - j;
                memcpy(&m_buffer[j], data, i);
                Transform(m_state, m_buffer);

                for( ; i + 63 < len; i += 64) Transform(m_state, &data[i]);

                j = 0;
        }
        else i = 0;

        memcpy(&m_buffer[j], &data[i], len - i);
}

/*
 * Hash value in file contents
 */
#ifdef SHA1_UTILITY_FUNCTIONS
bool SHA1::HashFile(char *szFileName) {

        unsigned long ulFileSize, ulRest, ulBlocks;
        unsigned long i;
        uint_8 uData[SHA1_MAX_FILE_BUFFER];
        FILE *fIn;

        if(szFileName == NULL) return false;

        fIn = fopen(szFileName, "rb");
        if(fIn == NULL) return false;

        fseek(fIn, 0, SEEK_END);
        ulFileSize = (unsigned long)ftell(fIn);
        fseek(fIn, 0, SEEK_SET);

        if(ulFileSize != 0) {
                ulBlocks = ulFileSize / SHA1_MAX_FILE_BUFFER;
                ulRest = ulFileSize % SHA1_MAX_FILE_BUFFER;
        } else {
                ulBlocks = 0;
                ulRest = 0;
        } 

        for(i = 0; i < ulBlocks; i++){
                fread(uData, 1, SHA1_MAX_FILE_BUFFER, fIn);
                Update((uint_8 *)uData, SHA1_MAX_FILE_BUFFER);
        }

        if(ulRest != 0) { 
                fread(uData, 1, ulRest, fIn);
                Update((uint_8 *)uData, ulRest);
        }

        fclose(fIn); fIn = NULL;
        return true; 
}
#endif

/*
 * Compute the final SHA-1 message digest
 *
 * 	[.] This func needs to be called before GetHash(.) and ReportHash(.)
 */
void SHA1::Final() {

        uint_32 i;
        uint_8 finalcount[8];

        for(i = 0; i < 8; i++)
                finalcount[i] = (uint_8)((m_count[((i >= 4) ? 0 : 1)]
                        >> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

        Update((uint_8 *)"\200", 1);

        while ((m_count[0] & 504) != 448)
                Update((uint_8 *)"\0", 1);

        Update(finalcount, 8); // Cause a SHA1Transform()

        for(i = 0; i < 20; i++)
        {
                m_digest[i] = (uint_8)((m_state[i >> 2] >> ((3 - (i & 3)) * 8) ) & 255);
        }

        // Wipe variables for security reasons
#ifdef SHA1_WIPE_VARIABLES
        i = 0;
        memset(m_buffer, 0, 64);
        memset(m_state, 0, 20);
        memset(m_count, 0, 8);
        memset(finalcount, 0, 8);
        Transform(m_state, m_buffer);
#endif
}

/*
 * After Final() calling, we can get the message digest (pre-formatted) using this func
 * 
 * 	[.] result is stored in szReport as string value
 */ 
#ifdef SHA1_UTILITY_FUNCTIONS
void SHA1::ReportHash(char *szReport, unsigned char uReportType) {
        unsigned char i;
        char szTemp[16];

        if(szReport == NULL) return;

        if(uReportType == REPORT_HEX) {
                sprintf(szTemp, "%02X", m_digest[0]);
                strcat(szReport, szTemp);

                for(i = 1; i < 20; i++) {
                        sprintf(szTemp, " %02X", m_digest[i]);
                        strcat(szReport, szTemp);
                }
        } else if(uReportType == REPORT_DIGIT) {
                sprintf(szTemp, "%u", m_digest[0]);
                strcat(szReport, szTemp);

                for(i = 1; i < 20; i++) {
                        sprintf(szTemp, " %u", m_digest[i]);
                        strcat(szReport, szTemp);
                }
        } else 
		strcpy(szReport, "Error: Unknown report type!");
}
#endif

/*
 * After Final() calling, we can get the message digest (raw format) using this func
 *
 * 	[.] SHA-1 produces a 160-bit/20-byte hash
 */
void SHA1::GetHash(uint_8 *puDest) {
        memcpy(puDest, m_digest, 20);
}
