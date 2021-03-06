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

/* $Id: sst-sink.cc 1 2009-10-11 22:06:03Z soohyunc $ */

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <iostream>
#include <assert.h>
#include <time.h>

#include "basetrace.h"
#include "sst-sink.h"

static class SstSinkClass : public TclClass {
public:
        SstSinkClass() : TclClass("Agent/SstSink") {}
        TclObject* create(int, const char*const*) {
                return (new SstSinkAgent());
        }
} class_sst_sink;

/*
 * SstSinkAgent declaration
 */
int SstSinkAgent::command(int argc, const char*const* argv)
{
	printf(" XXX command in sst_sink called - %s\n", argv[0]);
	printf(" XXX command in sst_sink called - %s\n", argv[1]);
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

SstSinkAgent::SstSinkAgent() : Agent(PT_SSTack), seqno_(0) {
	printf(" * SstSinkAgent() called *\n");
	
	isRID		= false;		// RID is not available yet
	isRND_R		= false;		// random number by reader yet
	flag_		= 0;			// TID verified?
	rID_		= 0;			// init reader ID
	pkt_size_	= 512;			// packet size
	npacket_	= 0;			// number of packet sent
}

/*
 * SstSink Query Initiation
 */
void SstSinkAgent::init_query() {
	assert(seqno > 0);

	if (!isRID) { /* if sst-sink doesn't have RID yet... */
		srand(time(NULL));
		rID_ = rand()%10000;

		isRID = true;
	}
	output(seqno_);
}

/*
 * SstSink Main Reception Path
 */
void SstSinkAgent::recv(Packet* secupkt, Handler*) {

	hdr_sst *ssth = hdr_sst::access(secupkt);

	//seqno_ = ssth->seqno_;

	output(seqno_);
	Packet::free(secupkt);
}

/*
 * SstSink Output Method
 */
void SstSinkAgent::output(int seqno) {

	/* allocate a packet */
	Packet* secupkt = allocpkt();

	/* declare headers */
        hdr_sst_ack *sstah = hdr_sst_ack::access(secupkt);
        hdr_cmn *cmnh = hdr_cmn::access(secupkt);
        cmnh->size_ = pkt_size_;                // initial query packet size

	/* if it is the very first packet (i.e., query packet) */
	if (seqno_ == 0) 
		sstah->PT_QUERY = true;
	else
		sstah->PT_QUERY = false;
	
	/* assign values */
        sstah->readerID_ = rID_;                // reader ID
        sstah->seqno() = seqno;
	if (!isRND_R) {
		srand(time(NULL));
		rR_ = rand()%10000;
		sstah->rnd_ = rR_;
		
		isRND_R = true;
	}

	/* send a packet */
	send(secupkt, 0);
	seqno_++;
	npacket_++;
}
