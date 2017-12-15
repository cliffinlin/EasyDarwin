/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 *
 */
 /*
	 File:       TrackStats.h
 */

#ifndef __TRACK_STATS_H__
#define __TRACK_STATS_H__

//Add by linjingming
struct TrackStats
{
	//Modified by ClientSession
	UInt32				fNumPacketsReceived;                //track only good packets(but include late and duplicates)
	UInt32				fNumBytesReceived;                  //includes RTP header
	UInt32				fNumOutOfOrderPackets;				//excludes duplicates
	UInt32				fNumOutOfBoundPackets;
	UInt32				fNumMalformedPackets;				//include packets with bad SSRC
	UInt32				fNumAcks;							//cumulative; counts ACK packets with masks as 1 ACK

	UInt16				fDestRTCPPort;
	UInt32				fServerSSRC;						//0 for not available
	UInt32				fClientSSRC;

	//Used for the DLSR and LSR field of the RTCP
	SInt64				fLastSenderReportNTPTime;
	SInt64				fLastSenderReportLocalTime;

	//These values are used to calculate the fraction lost and cumulative number of packets lost field in the RTCP RR packet.
	//See RFC 3550 6.4.1 and A.3

	//fHighestSeqNum is the highest valid sequence number received; note that this is 32 bits so that it never overflows.
	//An initial value of kUInt32_Max is used as an invalid marker(such that no valid sequence number has been received yet).
	UInt32				fHighestSeqNum;
	UInt32				fBaseSeqNum;
	UInt32				fExpectedPrior;
	UInt32				fReceivedPrior;

	UInt32				fJitter;
	SInt64				fLastReceivedTimeStamp;
	SInt64				fLastRTPPacketTimeStamp;

//	SVector<UInt32> fPacketsToAck;
	TrackStats() : fNumPacketsReceived(0), fNumBytesReceived(0), fNumOutOfOrderPackets(0), fNumOutOfBoundPackets(0),
			fNumMalformedPackets(0), fNumAcks(0), fDestRTCPPort(0),	fServerSSRC(0), fClientSSRC(0), fLastSenderReportNTPTime(0),
			fLastSenderReportLocalTime(0), fHighestSeqNum(kUInt32_Max), fBaseSeqNum(0), fExpectedPrior(0), fReceivedPrior(0)
	{ }
};
//Add by linjingming

#endif //__TRACK_STATS_H__
