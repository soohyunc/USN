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

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <iostream>
#include <assert.h>
#include "basetrace.h"
#include "sst-tag.h"

int hdr_sst_tag::offset_;      // header offset of an ACK

/*
 * OTcl linkage for packet headers
 */
static class SstTagHeaderClass : public PacketHeaderClass {
public:
        SstTagHeaderClass() : PacketHeaderClass ("PacketHeader/SstTag", sizeof(hdr_sst_tag)) {
                bind_offset(&hdr_sst_tag::offset_);
             }
} class_sst_tag_hdr;

static class SstClass : public TclClass {
public:
        SstClass() : TclClass("Agent/SstTag") {}
        TclObject* create(int, const char*const*) {
                return (new SstTagAgent());
        }
} class_sst_tag;

/*
 * SstTagAgent Command
 */
int SstTagAgent::command(int argc, const char*const* argv) {

        //printf(" XXX command in SstTag called - %s\n", argv[0]);
        //printf(" XXX command in SstTag called - %s\n", argv[1]);
        Tcl& tcl = Tcl::instance();
        if (argc==3) {
                if (strcmp(argv[1],"send")==0) {
                        //output(seqno_);
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
 * SstAgent declaration
 */
SstTagAgent::SstTagAgent() : Agent(PT_SstTag) {
	printf(" *** SstTagAgent called ***\n");

	srand(time(NULL));
	seqno_ 			= 0;			// initial packet seqno
	ackno_			= 0;			// initial ackno
	pkt_size_		= 512;			// total packet size
	isTID_			= false;		// has Tag ID been created?
	isRND_			= false;		// has t_rand_ been created?


	/*
	 * Select the number of bytes to encrypt (multiple of block)
	 */
	byteCnt_		= 16;
	//byteCnt_ 		= (BLOCK_SIZE/8) * (1 + (rand() % MAX_BLK_CNT));
}

/*
 * SST Tag Ack Method
 */
void SstTagAgent::tagAck(int seqno) {
	printf(" [Tag] tagAck		called !\n");

	/*
	 * alloc a packet
	 */
	Packet* pkt	= allocpkt();

	/*
	 * declare headers
	 */
	hdr_sst_tag	*th	= hdr_sst_tag::access(pkt);
	hdr_cmn		*cmnh	= hdr_cmn::access(pkt);
	cmnh->size_	= pkt_size_;

	/*
	 * assign values
	 */
	th->ackno_	= seqno;
	//printf("	[.] th->ackno_	%d\n", th->ackno_);

	/*
	 * meta ID
	 */
	printf("	--------------------	meta ID	(Tag)\n");
	if(isTID_ != true) { 	/* Tag ID */
		tagID();
		isTID_ = true;
	}
	//printf("	[.] pt_[0]	%d\n", pt_[0]);
	printf("	[.] Tag ID 		%d\n", tid_);

	Ek(pkt, tid_);
	memcpy(&th->mid_, ct_, byteCnt_);
/*
	printf("	[.] copied meta ID	");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", th->mid_[i]);
	}
	printf("\n");
*/
	
	/*
	 * Random number by Reader
	 */
	printf("	--------------------	Random number by Reader	(Tag)\n");
	printf("	[.] rnd no. (reader) 	%d\n", r_rand_);
	Ek(pkt, r_rand_);
	memcpy(&th->e_rR_, ct_, byteCnt_);
/*
	printf("	[.] copied rR		");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", th->e_rR_[i]);
	}
	printf("\n");
*/

	/*
	 * Random number by Tag
	 */
	printf("	--------------------	Random number by Tag (Tag)\n");
	if (isRND_ != true) {	/* Random number by Tag */
		tagRnd();
		isRND_	= true;
	}
	printf("	[.] rnd no. (tag) 	%d\n", t_rand_);
	Ek(pkt, t_rand_);
	memcpy(&th->e_tR_, ct_, byteCnt_);
/*
	printf("	[.] copied tR		");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", th->e_tR_[i]);
	}
	printf("\n");
*/
	
	/*
	 * Reader ID
	 */
	printf("	--------------------	Reader ID (Tag)\n");
	printf("	[.] Reader ID 		%d\n", rid_);
	Ek(pkt, rid_);
	memcpy(&th->e_rid_, ct_, byteCnt_);
/*
	printf("	[.] copied RID		");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", th->e_rid_[i]);
	}
	printf("\n");
*/
	
	if (seqno >= ackno_) {
		send(pkt,0);
		ackno_++;
		npacket_++;
	}
}

/*
 * SST Reception Path
 */
void SstTagAgent::recv(Packet* pkt, Handler*) {
	printf(" [Tag] recv		called !\n");

	hdr_sst_rdr	*rh	= hdr_sst_rdr::access(pkt);

	rid_		= rh->rid_;
	r_rand_		= rh->rnd_;
	seqno_		= rh->seqno_;
	//printf("	[.] seqno_	%d\n", seqno_);

	tagAck(seqno_);
	Packet::free(pkt);
}

/*
 * AES Encryption
 */
void SstTagAgent::Ek(Packet* pkt, int number) {
	//printf(" [Tag] Ek	called !\n");
	hdr_sst_tag	*th	= hdr_sst_tag::access(pkt);
	
	/*
	 * Make key
	 */
	if (makeKey(&th->ki_, DIR_ENCRYPT, KEYLEN, NULL) != TRUE)
		printf("	[.] AES KEY MAKING FAILED !!!\n");

	/*
	 * Cipher Init
	 */
	if (cipherInit(&th->ci_, MODE_ECB, NULL) != TRUE)
		printf("	[.] AES CIPHER INIT FAILED !!!\n");

	/*
	 * Select key bits
	 */
	for (int i = 0; i < KEYLEN/32; i++)
		th->ki_.key32[i] = 0x10003 * rand();

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

	pt_[0]	= number;
	
	/* 
	 * Encrypt the bytes
	 */
	if (blockEncrypt(&th->ci_, &th->ki_, pt_, byteCnt_*8, ct_) != byteCnt_*8)
		printf("	[.] AES ENCRYPT FAILED !!!\n");

/*
	printf("	[.] cipher text		");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", ct_[i]);
	}
	printf("\n");
*/

	/*
	 * Decrypt the bytes
	 */
/*
	if (blockDecrypt(&th->ci_, &th->ki_, ct_, byteCnt_*8, dt_) != byteCnt_*8)
		printf("	[.] AES DECRYPT FAILED !!!\n");

	printf("	[.] decipher text	");
	for (int i = 0; i < byteCnt_; i++) {
		printf(" %d ", dt_[i]);
	}
	printf("\n");
*/
}

/*
 * Tag ID
 */
void SstTagAgent::tagID() {
	tid_	= (BYTE) rand()%10000;		// tag id
}

/*
 * Random number by Tag
 */
void SstTagAgent::tagRnd() {
	t_rand_	= (BYTE) rand()%10000;		// random number by tag
	//printf("	[.] t_rand_	%d\n", t_rand_);
}
