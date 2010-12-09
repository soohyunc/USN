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

/* $Id: sst-rdr.h 10 2009-11-23 01:22:19Z soohyunc $ */

#ifndef ns_sst_rdr_h
#define ns_sst_rdr_h

#include "sst-tag.h"
#include "aes.h"

// SST Reader packet header structure
struct hdr_sst_rdr {
	int	rnd_;			// random number
	int	rid_;			// reader ID
	int	seqno_;			// sequence number

	BYTE	e_tid_[MAX_BLK_CNT*(BLOCK_SIZE/8)];     // encrypted Tag ID
	BYTE	e_rR_[MAX_BLK_CNT*(BLOCK_SIZE/8)];      // random number by Reader
	BYTE	e_tR_[MAX_BLK_CNT*(BLOCK_SIZE/8)];      // random number by Tag


	// packet header access functions
	static int offset_;             // offset for this header
	inline static int& offset() { return offset_; }
	inline static hdr_sst_rdr* access(const Packet* p) {
		return (hdr_sst_rdr*) p->access(offset_);
	}
};

class SstRdrAgent : public Agent {
public:
	// constructor - a new Sst Reader Agent
	SstRdrAgent();

	// process a function call from OTcl
	int command(int argc, const char* const* agrv);

	// receive a packet
	virtual void recv(Packet*, Handler*);

	// send a packet
	virtual void output(int seqno);

protected:
        // current simulator time
	inline double now() { return Scheduler::instance().clock(); }

	// Reader Methods
	void init_query();			// initialise the query (Rdr to Tag)
	void Ek(int);				// AES encrypted message
	
	keyInstance     ki_;			// AES Key
	cipherInstance  ci_;			// AES message block

	// variables
	bool	isRID_;			// has RID generated?
	bool	isRND_;			// has a reader got r_rand_?
	bool	isTagRND_;	// has the rnd no from the tag been received?
	bool	isRdrRND_;	// has the rnd no from the reader been received?
	bool	isTID_;			// has TID been decrypted successfully?
	int		rid_;			// reader id
	int		tid_;			// tag id
	int		t_rand_;		// random number by tag
	int		r_rand_;		// random number by reader
	int		seqno_;			// packet sequence number
	int		npacket_;		// number of packet sent
	int		pkt_size_;

	int 		byteCnt_;

	BYTE    pt_[MAX_BLK_CNT*(BLOCK_SIZE/8)];        // plain text
	BYTE    ct_[MAX_BLK_CNT*(BLOCK_SIZE/8)];        // cipher text
	BYTE    dt_[MAX_BLK_CNT*(BLOCK_SIZE/8)];        // decrypted text

};

#endif /* ns_sst_rdr_h */
