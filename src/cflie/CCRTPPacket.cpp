// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "cflie/CCRTPPacket.h"
#include <stdlib.h>


void CCRTPPacketInit1(CCRTPPacket* crPacket,int nPort) {
  basicSetup(crPacket);
  setPort(crPacket,nPort);
}

void CCRTPPacketInit3(CCRTPPacket* crPacket,char *cData, int nDataLength, int nPort) {
  basicSetup(crPacket);
  setPort(crPacket,nPort);
  
  setData(crPacket,cData, nDataLength);
}

void CCRTPPacketInit2(CCRTPPacket* crPacket,char cData, int nPort) {
  basicSetup(crPacket);
  setPort(crPacket,nPort);
  
  setData(crPacket,&cData, 1);
}

void CCRTPPacketDesctructor(CCRTPPacket* crPacket) {
  clearData(crPacket);
}

void basicSetup(CCRTPPacket* crPacket) {
  crPacket->m_cData = NULL;
  crPacket->m_nDataLength = 0;
  crPacket->m_nPort = 0;
  crPacket->m_nChannel = 0;
  crPacket->m_bIsPingPacket = false;
}

//Clear Data before set
void setData(CCRTPPacket* crPacket,char *cData, int nDataLength) {
  clearData(crPacket);
  
  //m_cData = new char[nDataLength]();
  crPacket->m_cData=(char*)malloc(nDataLength*sizeof(char));
  memcpy(crPacket->m_cData, cData, nDataLength);
  crPacket->m_nDataLength = nDataLength;
}

char *data(CCRTPPacket* crPacket) {
  return crPacket->m_cData;
}

int dataLength(CCRTPPacket* crPacket) {
  return crPacket->m_nDataLength;
}

void clearData(CCRTPPacket* crPacket) {
  if(crPacket->m_cData != NULL) {
    free (crPacket->m_cData);
    crPacket->m_cData = NULL;
    crPacket->m_nDataLength = 0;
  }
}

char *sendableData(CCRTPPacket* crPacket) {
  //char *cSendable = new char[sendableDataLength(crPacket)]();
  char *cSendable = (char*)malloc(sendableDataLength(crPacket)*sizeof(char));

  if(crPacket->m_bIsPingPacket) {
    cSendable[0] = 0xff;
  } else {
    // Header byte
    cSendable[0] = (crPacket->m_nPort << 4) | 0b00001100 | (crPacket->m_nChannel & 0x03);
    
    // Payload
    memcpy(&cSendable[1], crPacket->m_cData, crPacket->m_nDataLength);
    
    // Finishing byte
    //cSendable[m_nDataLength + 1] = 0x27;
  }
  
  return cSendable;
}

int sendableDataLength(CCRTPPacket* crPacket) {
  if(crPacket->m_bIsPingPacket) {
    return 1;
  } else {
    return crPacket->m_nDataLength + 1;//2;
  }
}

void setPort(CCRTPPacket* crPacket,int nPort) {
  crPacket->m_nPort = nPort;
}

int port(CCRTPPacket* crPacket) {
  return crPacket->m_nPort;
}

void setChannel(CCRTPPacket* crPacket,int nChannel) {
  crPacket->m_nChannel = nChannel;
}

int channel(CCRTPPacket* crPacket) {
  return crPacket->m_nChannel;
}

void setIsPingPacket(CCRTPPacket* crPacket,bool bIsPingPacket) {
  crPacket->m_bIsPingPacket = bIsPingPacket;
}

bool isPingPacket(CCRTPPacket* crPacket) {
  return crPacket->m_bIsPingPacket;
}
