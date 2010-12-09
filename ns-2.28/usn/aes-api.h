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

/**
 * This code is mainly written by the following authors.
 *      @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 *      @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 *      @author Paulo Barreto <paulo.barreto@terra.com.br>
 *    
 * URL: <http://www.iaik.tu-graz.ac.at/research/krypto/AES/old/%7Erijmen/rijndael/>
 * 
 * This code has been modified slightly in order this code to work under ns-2.28
 * by Soo-Hyun Choi <s.choi@cs.ucl.ac.uk>
 *
 * URL: http://www.cs.ucl.ac.uk/staff/S.Choi
 */

/*
 * $Id: aes-api.h 1 2009-10-11 22:06:03Z soohyunc $
 */ 

#ifndef _ns_aes_api_h
#define _ns_aes_api_h

#include <stdio.h>
#include "aes.h"

/*  Generic Defines  */
#define     DIR_ENCRYPT           0 /*  Are we encrpyting?  */
#define     DIR_DECRYPT           1 /*  Are we decrpyting?  */
#define     MODE_ECB              1 /*  Are we ciphering in ECB mode?   */
#define     MODE_CBC              2 /*  Are we ciphering in CBC mode?   */
#define     MODE_CFB1             3 /*  Are we ciphering in 1-bit CFB mode? */
#define     TRUE                  1
#define     FALSE                 0
#define     BITSPERBLOCK        128 /* Default number of bits in a cipher block */

/*  Error Codes  */
#define     BAD_KEY_DIR          -1 /*  Key direction is invalid, e.g., unknown value */
#define     BAD_KEY_MAT          -2 /*  Key material not of correct length */
#define     BAD_KEY_INSTANCE     -3 /*  Key passed is not valid */
#define     BAD_CIPHER_MODE      -4 /*  Params struct passed to cipherInit invalid */
#define     BAD_CIPHER_STATE     -5 /*  Cipher in wrong state (e.g., not initialized) */
#define     BAD_BLOCK_LENGTH     -6
#define     BAD_CIPHER_INSTANCE  -7
#define     BAD_DATA             -8 /*  Data contents are invalid, e.g., invalid padding */
#define     BAD_OTHER            -9 /*  Unknown error */

/*  Algorithm-specific Defines  */
#define     MAX_KEY_SIZE         64 /* # of ASCII char's needed to represent a key */
#define     MAX_IV_SIZE          16 /* # bytes needed to represent an IV  */

/*  Typedefs  */
typedef unsigned char   BYTE;

/*  The structure for key information */
typedef struct {
    BYTE  direction;                /* Key used for encrypting or decrypting? */
    int   keyLen;                   /* Length of the key  */
    char  keyMaterial[MAX_KEY_SIZE+1];  /* Raw key data in ASCII, e.g., user input or KAT values */
	int   Nr;                       /* key-length-dependent number of rounds */
	uint_32   rk[4*(MAXNR + 1)];        /* key schedule */
	uint_32   ek[4*(MAXNR + 1)];        /* CFB1 key schedule (encryption only) */
} keyInstance;

/*  The structure for cipher information */
typedef struct {                    /* changed order of the components */
    BYTE  mode;                     /* MODE_ECB, MODE_CBC, or MODE_CFB1 */
    BYTE  IV[MAX_IV_SIZE];          /* A possible Initialization Vector for ciphering */
} cipherInstance;

/*  Function prototypes  */
int makeKey(keyInstance *key, BYTE direction, int keyLen, char *keyMaterial);

int cipherInit(cipherInstance *cipher, BYTE mode, char *IV);

int blockEncrypt(cipherInstance *cipher, keyInstance *key,
        BYTE *input, int inputLen, BYTE *outBuffer);

int padEncrypt(cipherInstance *cipher, keyInstance *key,
		BYTE *input, int inputOctets, BYTE *outBuffer);

int blockDecrypt(cipherInstance *cipher, keyInstance *key,
        BYTE *input, int inputLen, BYTE *outBuffer);

int padDecrypt(cipherInstance *cipher, keyInstance *key,
		BYTE *input, int inputOctets, BYTE *outBuffer);

#ifdef INTERMEDIATE_VALUE_KAT
int cipherUpdateRounds(cipherInstance *cipher, keyInstance *key,
        BYTE *input, int inputLen, BYTE *outBuffer, int Rounds);
#endif /* INTERMEDIATE_VALUE_KAT */

#endif /* _ns_aes_api_h */
