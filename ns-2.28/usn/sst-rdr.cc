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

/* $Id: sst-rdr.cc 1 2009-10-11 22:06:03Z soohyunc $ */

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "basetrace.h"
#include "sst-rdr.h"

int hdr_sst_rdr::offset_;          // header offset

/*
 * OTcl linkage for packet headers
 */
static class SstRdrHeaderClass : public PacketHeaderClass {
public:
        SstRdrHeaderClass() : PacketHeaderClass ("PacketHeader/SstRdr", sizeof(hdr_sst_rdr)){
                bind_offset(&hdr_sst_rdr::offset_);
        }
} class_sst_rdr_hdr;

static class SstRdrClass : public TclClass {
public:
        SstRdrClass() : TclClass("Agent/SstRdr") {}
        TclObject* create(int, const char*const*) {
                return (new SstRdrAgent());
        }
} class_sst_rdr;

/*
 * SstRdrAgent Command
 */
int SstRdrAgent::command(int argc, const char* const* argv) {

        //printf(" XXX command in SstRdr called - %s\n", argv[0]);
        //printf(" XXX command in SstRdr called - %s\n", argv[1]);
        Tcl& tcl = Tcl::instance();
        if (argc==2) {
                if (strcmp(argv[1],"reset")==0) {
                        init_query();
                        return TCL_OK;
                }
                if (strcmp(argv[1],"stop")==0) {
                        //stop();
                        return TCL_OK;
                }
        }
        return (Agent::command(argc, argv));
}

/*
 * SstRdrAgent
 */
SstRdrAgent::SstRdrAgent() : Agent(PT_SstRdr) {
	printf(" *** SstRdrAgent() called ***\n");

	srand(time(NULL));
	isRID_			= false;
	isRND_			= false;
	isTagRND_		= false;
	isRdrRND_		= false;
	isTID_			= false;
	rid_			= 0;
	pkt_size_		= 512;
	npacket_		= 0;
	seqno_			= 0;

	byteCnt_		= 16;
}

/*
 * SstRdr Query Initiation
 */
void SstRdrAgent::init_query() {
	printf(" [Rdr] init_query	called !\n");

	if(isRND_ != true) {
		r_rand_	= (BYTE) rand()%10000;
		isRND_	= true;
	}
	printf("	[.] rnd no. (reader)		%d\n", r_rand_);

	if(isRID_ != true) {
		rid_	= (BYTE) rand()%10000;
		isRID_	= true;
	}
	printf("	[.] Reader ID			%d\n", rid_);

	output(seqno_);
}

/*
 * SstRdr Output Method
 */
void SstRdrAgent::output(int seqno) {
	printf(" [Rdr] output		called !\n");

	seqno_ = seqno;

	/*
	 * alloc a packet
	 */
	Packet* pkt = allocpkt();

	/*
	 * declare headers
	 */
	hdr_sst_rdr 	*rh	= hdr_sst_rdr::access(pkt);
	hdr_cmn		*cmnh	= hdr_cmn::access(pkt);
	cmnh->size_ = pkt_size_;			// packet size

	/*
	 * assign values
	 */
	rh->seqno_	= seqno_;
	rh->rid_	= rid_;
	rh->rnd_	= r_rand_;
	//printf("	[.] seqno_	%d\n", seqno_);

	/*
	 * Random number by Reader
	 */
	if (isRdrRND_) {
	        printf("	====================	Random number by Reader	(Reader)\n");
		Ek(r_rand_);
		memcpy(&rh->e_rR_, ct_, byteCnt_);
	/*
	        printf("        [.] returned rR		");
	        for (int i = 0; i < byteCnt_; i++) {
	                printf(" %d ", rh->e_rR_[i]);
	        }
	        printf("\n");
	*/
	}				

	/*
	 * Random number by Tag
	 */
	if(isTagRND_) {
	        printf("	====================	Random number by Tag (Reader)\n");
		Ek(t_rand_);
		memcpy(&rh->e_tR_, ct_, byteCnt_);
	/*
	        printf("        [.] returned tR		");
	        for (int i = 0; i < byteCnt_; i++) {
	                printf(" %d ", rh->e_tR_[i]);
	        }
		printf("\n");
	*/
	}	

	/*
	 * Tag ID
	 */
	if(isTID_) {
	        printf("	====================	Tag ID (Reader)\n");
		Ek(tid_);
		memcpy(&rh->e_tid_, ct_, byteCnt_);
	/*
		printf("        [.] returned TID	");
	        for (int i = 0; i < byteCnt_; i++) {
	                printf(" %d ", rh->e_tid_[i]);
	        }
	        printf("\n");
	*/
	}
	/*
	 * send a packet
	 */
	send(pkt,0);
	seqno_++;
	npacket_++;
}

/*
 * SstRdr Reception Path
 */
void SstRdrAgent::recv(Packet* pkt, Handler*) {
	printf(" [Rdr] recv		called !\n");
	hdr_sst_tag	*th	= hdr_sst_tag::access(pkt);

	/*
	 * Extract Reader ID 
	 */
	if (blockDecrypt(&th->ci_, &th->ki_, th->e_rid_, byteCnt_*8, dt_) != byteCnt_*8)
		printf("	[.] AES DECRYPT FAILED !!!\n");
	
	if (dt_[0] != rid_)
		printf("	READER ID IS NOT CONSISTENT !!!\n");
	else
		printf("	[.] Reader ID Verified\n");

	/*
	 * Extract Tag ID
	 */
	if (blockDecrypt(&th->ci_, &th->ki_, th->mid_, byteCnt_*8, dt_) != byteCnt_*8)
		printf("	[.] AES DECRYPT FAILED !!!\n");
	else
		isTID_		= true;

	tid_	= dt_[0];
	printf("	[.] Tag ID			%d\n", tid_);

	/*
	 * Extract random number by Reader
	 */
	if (blockDecrypt(&th->ci_, &th->ki_, th->e_rR_, byteCnt_*8, dt_) != byteCnt_*8)
		printf("	[.] AES DECRYPT FAILED !!!\n");
	else
		isRdrRND_	= true;

	r_rand_	= dt_[0];
	printf("	[.] rnd no. (reader)		%d\n", r_rand_);

	/*
	 * Extract random number by Tag
	 */
	if (blockDecrypt(&th->ci_, &th->ki_, th->e_tR_, byteCnt_*8, dt_) != byteCnt_*8)
		printf("	[.] AES DECRYPT FAILED !!!\n");
	else
		isTagRND_	= true;

	t_rand_	= dt_[0];
	printf("	[.] rnd no. (tag)		%d\n", t_rand_);

	output(seqno_);
	Packet::free(pkt);
}

/*
 * AES Encryption
 */
void SstRdrAgent::Ek(int number) {
	//printf(" [Rdr] Ek	called !\n");
	
	/*
         * Make key
         */
        if (makeKey(&ki_, DIR_ENCRYPT, KEYLEN, NULL) != TRUE)
                printf("        [.] AES KEY MAKING FAILED !!!\n");

        /*
         * Cipher Init
         */
        if (cipherInit(&ci_, MODE_ECB, NULL) != TRUE)
                printf("        [.] AES CIPHER INIT FAILED !!!\n");

        /*
         * Select key bits
         */
        for (int i = 0; i < KEYLEN/32; i++)
                ki_.key32[i] = 0x10003 * rand();

        /*
         * Run the key schedule
         */
        //reKey(&th->ki_);

        /*
         * Generate test data
         */

        printf("	[.] CT ");
        for (int i = 0; i < byteCnt_; i++) {
                pt_[i] = (BYTE) rand();
                printf(" %d ", pt_[i]);
        }
        printf("\n");


        pt_[0]  = number;

        /*
         * Encrypt the bytes
         */
        if (blockEncrypt(&ci_, &ki_, pt_, byteCnt_*8, ct_) != byteCnt_*8)
                printf("        [.] AES ENCRYPT FAILED !!!\n");

/*
        printf("        [.] cipher text         ");
        for (int i = 0; i < byteCnt_; i++) {
                printf(" %d ", ct_[i]);
        }
        printf("\n");
*/
}
