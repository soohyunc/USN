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

/* $Id: sst.cc 1 2009-10-11 22:06:03Z soohyunc $ */

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <iostream>
#include <assert.h>
#include "basetrace.h"
#include "sst.h"

int hdr_sst::offset_;          // header offset
int hdr_sst_ack::offset_;      // header offset of an ACK

/*
 * OTcl linkage for packet headers
 */
static class SstHeaderClass : public PacketHeaderClass {
public:
	SstHeaderClass() : PacketHeaderClass ("PacketHeader/SST", sizeof(hdr_sst)){
		bind_offset(&hdr_sst::offset_);
	}
} class_ssthdr;

static class Sst_ACKHeaderClass : public PacketHeaderClass {
public:
        Sst_ACKHeaderClass() : PacketHeaderClass ("PacketHeader/SSTack", sizeof(hdr_sst_ack)) {
                bind_offset(&hdr_sst_ack::offset_);
             }
} class_sst_ackhdr;

static class SstClass : public TclClass {
public:
        SstClass() : TclClass("Agent/SST") {}
        TclObject* create(int, const char*const*) {
                return (new SstAgent());
        }
} class_sst;

/*
 * SstAgent declaration
 */
int SstAgent::command(int argc, const char*const* argv)
{
	//printf(" XXX command in SST called - %s\n", argv[0]);
	//printf(" XXX command in SST called - %s\n", argv[1]);
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

SstAgent::SstAgent() : Agent(PT_SST), seqno_(0) {
	printf(" * SstAgent() called *\n");

	tID_		= 0;		// initial Tag ID
	pkt_size_ 	= 512;		// packet size
	ackno_		= 0;		// latest ack received
	keySize_	= 128;		// AES key size
}

/*
 * SST Main Reception Path
 */
void SstAgent::recv(Packet* secupkt, Handler*) {
	printf(" recv called !\n");

	hdr_sst_ack *sstah = hdr_sst_ack::access(secupkt);

	rID_ = sstah->readerID_;	// take RID
	rR_ = sstah->rnd_;		// take rR_
	seqno_ = sstah->seqno_;		// take seqno_ 

	randT();			// random number by Tag
	TID();				// tag ID

	metaID();			// generate metaID
	Ek(rR_, rT_, rID_);		// AES encryption

	Packet::free(secupkt);
	output(seqno_);
}

/*
 * SST Output Method
 */
void SstAgent::output(int seqno) {

	Packet* secupkt = allocpkt();

	hdr_sst *ssth = hdr_sst::access(secupkt);
	hdr_cmn *cmnh = hdr_cmn::access(secupkt);
	cmnh->size_ = pkt_size_;	// packet size
	ssth->ackno_ = seqno;

	if (seqno == ackno_) {
		send(secupkt,0);
		ackno_++;
		npacket_++;
	}	
}

/*
 * Meta ID creation
 */
void SstAgent::metaID() {
	Packet* secupkt = allocpkt();
	hdr_sst *ssth = hdr_sst::access(secupkt);
	ssth->sstKeyElm.keyLen = KEYLENGTH;
}

/*
 * AES Encryption
 */
void SstAgent::Ek(u_int16_t randR, u_int16_t randT, u_int16_t readerID) {

}

void SstAgent::randT() {
	srand(time(NULL));
	rT_ = rand()%10000;
}

void SstAgent::TID() {
	srand(time(NULL));
	tID_ = rand()%10000;
}
