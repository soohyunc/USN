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

#ifndef ns_sst_tag_h
#define ns_sst_tag_h

#include "agent.h"
#include "packet.h"
#include "flags.h"
#include "ip.h"
#include "tcp.h"
#include "timer-handler.h"
#include "random.h"
#include "sst-rdr.h"
#include "aes.h"

#define	KEYLEN		128
#define MAX_BLK_CNT	4
#define BLOCK_SIZE	128			// number of bits per block

// SST Tag packet header structure
struct hdr_sst_tag {
	keyInstance	ki_;		// AES key
	cipherInstance	ci_;	// AES cipher instance
	int		ackno_;		// acknowledge number
	BYTE	mid_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// meta ID
	BYTE	e_rid_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// encrypted Reader ID
	BYTE	e_rR_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// random number by Reader
	BYTE	e_tR_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// random number by Tag

	// packet header access functions
	static int offset_;			// offset for this header
	inline static int& offset() { return offset_; }
	inline static hdr_sst_tag* access(const Packet* p) {
		return (hdr_sst_tag*) p->access(offset_);
	}
};

class SstTagAgent : public Agent {
public:
	// constructor - a new SST Tag agent
	SstTagAgent();

	// process a function call from OTcl
	int command(int argc, const char* const* argv);

	// receive a packet
	virtual void recv(Packet*, Handler*);

protected:
	// current simulator time
	inline double now() { return Scheduler::instance().clock(); }

	// Tag Methods
	void tagAck(int seqno);		// Tag Ack
	void metaID(Packet*);		// meta ID
	void Ek(Packet*, int);		// AES encrypted message
	void tagID();				// Tag ID
	void tagRnd();				// Random number by Tag

	// variables
	bool	isTID_;			// has Tag ID been created?
	bool	isRND_;			// has t_rand_ been created?
	int		seqno_;			// packet sequence number
	int		ackno_;			// ack number
	int		rid_;			// reader id
	int		tid_;			// tag id
	int		r_rand_;		// random number by reader
	int		t_rand_;		// random number by tag
	int		pkt_size_;		// total packet size
	int		npacket_;		// number of packet sent
	int		byteCnt_;		// AES byte count

	BYTE	pt_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// plain text
	BYTE	ct_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// cipher text
	BYTE	dt_[MAX_BLK_CNT*(BLOCK_SIZE/8)];	// decrypted text
};
#endif /* ns_sst_tag_h */
