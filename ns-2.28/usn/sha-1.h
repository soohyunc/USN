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

/*
 * PLEASE NOTE THAT THIS IMPLEMENTATION IS BASED ON THE SHA-1 CODE BY
 * 	Dominik Reichl <dominik.reichl@t-online.de>
 * 	Web: http://www.dominik-reichl.de/
 *
 * However, most of the code is re-written by 
 * 	Soo-Hyun Choi <s.choi@cs.ucl.ac.uk>
 * 	Web: http://www.cs.ucl.ac.uk/staff/S.Choi
 */

/* $Id: sha-1.h 1 2009-10-11 22:06:03Z soohyunc $ */

#ifndef ns_sha1_h
#define ns_sha1_h

#include "agent.h"
#include "packet.h"
#include "flags.h"
#include "ip.h"
#include "tcp.h"
#include "random.h"
#include <stdlib.h>
#include <stdio.h>

#include <memory.h>	// for memcpy() and memset() function
#include <string.h>	// for strcat() and strcpy() function

/*
 *     The type definitions below are valid for 32-bit architectures and
 *     may have to be adjusted for 16- or 64-bit architectures.
 *     
 */
typedef unsigned char		uint_8;
typedef unsigned short		uint_16;
typedef unsigned int		uint_32;
typedef short 			int_16;
	  
/*
 *	======== Test Vectors (from FIPS PUB 180-1) ========
 *
 *      SHA1("abc") = 
 *		A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
 *
 *	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
 *		84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
 *
 *      SHA1(A million repetitions of "a") =
 *               34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
 */


typedef union {
	uint_8		c[64];
	uint_32		l[16];
} SHA1_WORKSPACE_BLOCK;

class SHA1 {
public:
	enum {
		REPORT_HEX = 0,
		REPORT_DIGIT = 1
	};

	// Constructor and Destructor
	SHA1();
	~SHA1();

	uint_32		m_state[5];
	uint_32		m_count[2];
	uint_32		__reserved1[1];
	uint_8		m_buffer[64];
	uint_8		m_digest[20];
	uint_32		__reserved2[3];

	void Reset();

	// Update HASH value
	void Update (uint_8 *date, uint_32 len);
	bool HashFile (char *szFileName);

	// Finalize hash and report
	void Final();

	// Report functions: as pre-formatted and raw data
	void ReportHash (char *szReport, unsigned char uReportType = REPORT_HEX);
	void GetHash (uint_8 *puDest);

private:
	// Private SHA-1 transformation
	void Transform (uint_32 *state, uint_8 *buffer);

	// Member variables
	uint_8			m_workspace[64];
	SHA1_WORKSPACE_BLOCK	*m_block;	// SHA-1 pointer to the byte array
};

#endif /* ns_sha1_h */
