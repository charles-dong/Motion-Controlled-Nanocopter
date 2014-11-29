#include "cflie/CTOC.h"


void CTOCConstructor(CTOC* crCTOC,CCrazyRadio *crRadio, int nPort) {
  crCTOC->m_crRadio = crRadio;
  crCTOC->m_nPort = nPort;
  crCTOC->m_nItemCount = 0;
}


bool sendTOCPointerReset(CTOC* crCTOC) {
  //CCRTPPacket *crtpPacket = (CCRTPPacket *)malloc(sizeof(CCRTPPacket)); //CCRTPPacket(0x00, 0);
  CCRTPPacket *crtpPacket = new CCRTPPacket;
  CCRTPPacketInit2(crtpPacket,0x00,0);
  setPort(crtpPacket,crCTOC->m_nPort);
  CCRTPPacket *crtpReceived = sendPacket(crCTOC->m_crRadio,crtpPacket, true);
  
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}

bool requestMetaData(CTOC* crCTOC) {
  bool bReturnvalue = false;
  
  //CCRTPPacket *crtpPacket = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));//new CCRTPPacket(0x01, 0);
  CCRTPPacket *crtpPacket = new CCRTPPacket;
  CCRTPPacketInit2(crtpPacket,0x01,0);
  setPort(crtpPacket,crCTOC->m_nPort);
  CCRTPPacket *crtpReceived = sendAndReceive(crCTOC->m_crRadio,crtpPacket);
  
  if(data(crtpReceived)[1] == 0x01) {
    crCTOC->m_nItemCount = data(crtpReceived)[2];
    bReturnvalue = true;
  }
  
  free(crtpReceived);
  return bReturnvalue;
}

bool requestInitialItem(CTOC* crCTOC) {
  return requestItem2(crCTOC,0, true);
}

//????
bool requestItem1(CTOC* crCTOC,int nID) {
  return requestItem2(crCTOC,nID, false);
}

bool requestItem2(CTOC* crCTOC,int nID, bool bInitial) {
  bool bReturnvalue = false;
  
  char cRequest[2];
  cRequest[0] = 0x0;
  cRequest[1] = nID;
  
  //CCRTPPacket *crtpPacket = new CCRTPPacket(cRequest,
  //            (bInitial ? 1 : 2),
  //            0);

  //CCRTPPacket *crtpPacket = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
  CCRTPPacket *crtpPacket = new CCRTPPacket;
  CCRTPPacketInit3(crtpPacket,cRequest,(bInitial ? 1 : 2),0);

  setPort(crtpPacket,crCTOC->m_nPort);

  CCRTPPacket *crtpReceived = sendAndReceive(crCTOC->m_crRadio,crtpPacket);
  
  bReturnvalue = processItem(crCTOC,crtpReceived);
  
  free(crtpReceived);
  return bReturnvalue;
}

bool requestItems(CTOC* crCTOC) {
  for(int nI = 0; nI < crCTOC->m_nItemCount; nI++) {
   requestItem1(crCTOC,nI);
  }
  //this->requestInitialItem();
  
  return true;
}

bool processItem(CTOC* crCTOC,CCRTPPacket *crtpItem) {
  if(port(crtpItem) == crCTOC->m_nPort) {
    if(channel(crtpItem) == 0) {
      char *cData = data(crtpItem);
      int nLength = dataLength(crtpItem);
      
      if(cData[1] == 0x0) { // Command identification ok?
	int nID = cData[2];
	int nType = cData[3];
	
	string strGroup;
	int nI;
	for(nI = 4; cData[nI] != '\0'; nI++) {
	  strGroup += cData[nI];
	}
	
	nI++;
	string strIdentifier;
	for(; cData[nI] != '\0'; nI++) {
	  strIdentifier += cData[nI];
	}
	
	struct TOCElement teNew;
	teNew.strIdentifier = strIdentifier;
	teNew.strGroup = strGroup;
	teNew.nID = nID;
	teNew.nType = nType;
	teNew.bIsLogging = false;
	teNew.dValue = 0;
	
	crCTOC->m_lstTOCElements.push_back(teNew);
	
	//cout << strGroup << "." << strIdentifier << endl;
  printf("%s.%s\n", strGroup.c_str(),strIdentifier.c_str());
	
	return true;
      }
    }
  }
  
  return false;
}

struct TOCElement elementForName(CTOC* crCTOC,string strName, bool &bFound) {
  for(list<struct TOCElement>::iterator itElement = crCTOC->m_lstTOCElements.begin();
      itElement != crCTOC->m_lstTOCElements.end();
      itElement++) {
    struct TOCElement teCurrent = *itElement;
    
    string strTempFullname = teCurrent.strGroup + "." + teCurrent.strIdentifier;
    if(strName == strTempFullname) {
      bFound = true;
      return teCurrent;
    }
  }
  
  bFound = false;
  //printf("%s\n","false " );
  struct TOCElement teEmpty;
  
  return teEmpty;
}

struct TOCElement elementForID(CTOC* crCTOC,int nID, bool &bFound) {
  for(list<struct TOCElement>::iterator itElement = crCTOC->m_lstTOCElements.begin();
      itElement != crCTOC->m_lstTOCElements.end();
      itElement++) {
    struct TOCElement teCurrent = *itElement;
    
    if(nID == teCurrent.nID) {
      bFound = true;
      return teCurrent;
    }
  }
  
  bFound = false;
  struct TOCElement teEmpty;
  
  return teEmpty;
}

//ok
int idForName(CTOC* cotc,string strName) {
  bool bFound;
  
  struct TOCElement teResult = elementForName(cotc,strName, bFound);
  
  if(bFound) {
    return teResult.nID;
  }
  
  return -1;
}

bool startLogging(CTOC* cotc,string strName, string strBlockName) {
  bool bFound;
  struct LoggingBlock lbCurrent = loggingBlockForName(cotc,strBlockName, bFound);
  
  if(bFound) {
    struct TOCElement teCurrent = elementForName(cotc,strName, bFound);
    if(bFound) {
      printf("ytl Debug Found the logging %s %s\n", strName.c_str(),strBlockName.c_str());
      char cPayload[5] = {0x01, lbCurrent.nID, teCurrent.nType, teCurrent.nID};


      //CCRTPPacket *crtpLogVariable = new CCRTPPacket(cPayload, 4, 1);

      //CCRTPPacket *crtpLogVariable = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
  CCRTPPacket *crtpLogVariable = new CCRTPPacket;
  CCRTPPacketInit3(crtpLogVariable,cPayload,4,1);

      setPort(crtpLogVariable,cotc->m_nPort);
      setChannel(crtpLogVariable,1);
      CCRTPPacket *crtpReceived =sendAndReceive(cotc->m_crRadio,crtpLogVariable, true);
      
      char *cData = data(crtpReceived);
      bool bCreateOK = false;
      if(cData[1] == 0x01 &&
	 cData[2] == lbCurrent.nID &&
	 cData[3] == 0x00) {
	bCreateOK = true;
      } else {
        printf("%c\n", cData[3]);
	//cout << cData[3] << endl;
      }
      
      if(crtpReceived) {
	free(crtpReceived);
      }
      
      if(bCreateOK) {
	addElementToBlock(cotc,lbCurrent.nID, teCurrent.nID);
	
	return true;
      }
    }
  }
  
  return false;
}

bool addElementToBlock(CTOC* cotc,int nBlockID, int nElementID) {
  for(list<struct LoggingBlock>::iterator itBlock = cotc->m_lstLoggingBlocks.begin();
      itBlock != cotc->m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(lbCurrent.nID == nBlockID) {
      (*itBlock).lstElementIDs.push_back(nElementID);
      
      return true;
    }
  }
  
  return false;
}



double doubleValue(CTOC* cotc,string strName) {
  bool bFound;
  
  struct TOCElement teResult = elementForName(cotc,strName, bFound);
  
  if(bFound) {
    return teResult.dValue;
  }
  
  return 0;
}

struct LoggingBlock loggingBlockForName(CTOC* cotc,string strName, bool &bFound) {
  for(list<struct LoggingBlock>::iterator itBlock = cotc->m_lstLoggingBlocks.begin();
      itBlock != cotc->m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(strName == lbCurrent.strName) {
      bFound = true;
      return lbCurrent;
    }
  }
  
  bFound = false;
  struct LoggingBlock lbEmpty;
  
  return lbEmpty;
}

struct LoggingBlock loggingBlockForID(CTOC* cotc,int nID, bool &bFound) {
  for(list<struct LoggingBlock>::iterator itBlock = cotc->m_lstLoggingBlocks.begin();
      itBlock != cotc->m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(nID == lbCurrent.nID) {
      bFound = true;
      return lbCurrent;
    }
  }
  
  bFound = false;
  struct LoggingBlock lbEmpty;
  
  return lbEmpty;
}

bool registerLoggingBlock(CTOC* cotc,string strName, double dFrequency) {
  int nID = 0;
  bool bFound;
  
  if(dFrequency > 0) { // Only do it if a valid frequency > 0 is given
    loggingBlockForName(cotc,strName, bFound);
    if(bFound) {
      unregisterLoggingBlock(cotc,strName);
    }
    
    do {
      loggingBlockForID(cotc,nID, bFound);
	
      if(bFound) {
	nID++;
      }
    } while(bFound);
    
    unregisterLoggingBlockID(cotc,nID);
    
    double d10thOfMS = (1 / dFrequency) * 1000 * 10;
    //    char cPayload[4] = {0x00, (nID >> 16) & 0x00ff, nID & 0x00ff, d10thOfMS};
    char cPayload[4] = {0x00, nID, d10thOfMS};
    //CCRTPPacket *crtpRegisterBlock = new CCRTPPacket(cPayload, 3, 1);
    //CCRTPPacket *crtpRegisterBlock = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
  CCRTPPacket *crtpRegisterBlock = new CCRTPPacket;
  CCRTPPacketInit3(crtpRegisterBlock,cPayload,3,1);


    setPort(crtpRegisterBlock,cotc->m_nPort);
    setChannel(crtpRegisterBlock,1);
    
    CCRTPPacket *crtpReceived = sendAndReceive(cotc->m_crRadio,crtpRegisterBlock, true);
    
    char *cData = data(crtpReceived);
    bool bCreateOK = false;
    if(cData[1] == 0x00 &&
       cData[2] == nID &&
       cData[3] == 0x00) {
      bCreateOK = true;
      printf("Registered logging block `%s'\n",strName.c_str() );
      //cout << "Registered logging block `" << strName << "'" << endl;
    }
    
    if(crtpReceived) {
      free(crtpReceived);
    }
      
    if(bCreateOK) {
      struct LoggingBlock lbNew;
      lbNew.strName = strName;
      lbNew.nID = nID;
      lbNew.dFrequency = dFrequency;
	
      cotc->m_lstLoggingBlocks.push_back(lbNew);
	
      return enableLogging(cotc,strName);
    }
  }
  
  return false;
}

bool enableLogging(CTOC* cotc,string strBlockName) {
  bool bFound;
  
  struct LoggingBlock lbCurrent = loggingBlockForName(cotc,strBlockName, bFound);
  if(bFound) {
    double d10thOfMS = (1 / lbCurrent.dFrequency) * 1000 * 10;
    char cPayload[3] = {0x03, lbCurrent.nID, d10thOfMS};
    
    //CCRTPPacket *crtpEnable = new CCRTPPacket(cPayload, 3, 1);
    //CCRTPPacket *crtpEnable = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
  CCRTPPacket *crtpEnable = new CCRTPPacket;
  CCRTPPacketInit3(crtpEnable,cPayload,3,1);


    setPort(crtpEnable,cotc->m_nPort);
    setChannel(crtpEnable,1);
    
    CCRTPPacket *crtpReceived = sendAndReceive(cotc->m_crRadio,crtpEnable);
    delete crtpReceived;
    
    return true;
  }
  
  return false;
}

bool unregisterLoggingBlock(CTOC* cotc,string strName) {
  bool bFound;
  
  struct LoggingBlock lbCurrent =loggingBlockForName(cotc,strName, bFound);
  if(bFound) {
    return unregisterLoggingBlockID(cotc,lbCurrent.nID);
  }
  
  return false;
}

bool unregisterLoggingBlockID(CTOC* cotc,int nID) {
  char cPayload[2] = {0x02, nID};
  //CCRTPPacket *crtpUnregisterBlock = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
  CCRTPPacket *crtpUnregisterBlock = new CCRTPPacket;
  CCRTPPacketInit3(crtpUnregisterBlock,cPayload,2,1);

  //CCRTPPacket *crtpUnregisterBlock = new CCRTPPacket(cPayload, 2, 1);
  setPort(crtpUnregisterBlock,cotc->m_nPort);
  setChannel(crtpUnregisterBlock,1);
  CCRTPPacket *crtpReceived = sendAndReceive(cotc->m_crRadio,crtpUnregisterBlock, true);
  
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}

bool sendHoldParam(CTOC* ctoc,CCRTPPacket *crtpPacket,int nElementID){
  int hold_id=idForName(ctoc,"flightmode.althold");
  printf("%s %d %d\n", "going to try with nElementID ",nElementID,hold_id);
  if(hold_id!=nElementID)
      return false;
  char *cData = data(crtpPacket);
  char *cLogdata = &cData[5];

  int nByteLength = 1;
  uint8_t uint8Value=1;
  memcpy(&cLogdata[0],&uint8Value, nByteLength);

  CCRTPPacket *crtpReceived = sendPacket(ctoc->m_crRadio,crtpPacket, false);
  if(crtpReceived) {
    printf("%s\n", "set the althold");
    delete crtpReceived;
    return true;
  }
  
  return false;
}

//TY
void processPackets(CTOC* ctoc,list<CCRTPPacket*> lstPackets,bool ifsethold) {
  if(lstPackets.size() > 0) {
    for(list<CCRTPPacket*>::iterator itPacket = lstPackets.begin();
	itPacket != lstPackets.end();
	itPacket++) {
      CCRTPPacket *crtpPacket = *itPacket;
      
      char *cData = data(crtpPacket);
      float fValue;
      memcpy(&fValue, &cData[5], 4);
      //cout << fValue << endl;
      
      char *cLogdata = &cData[5];
      int nOffset = 0;
      int nIndex = 0;
      int nAvailableLogBytes = dataLength(crtpPacket) - 5;
      
      int nBlockID = cData[1];
      bool bFound;
      struct LoggingBlock lbCurrent = loggingBlockForID(ctoc,nBlockID, bFound);
      
      if(bFound) {
	while(nIndex < lbCurrent.lstElementIDs.size()) {
	  int nElementID = elementIDinBlock(ctoc,nBlockID, nIndex);

    
	  
    bool bFound;
	  struct TOCElement teCurrent = elementForID(ctoc,nElementID, bFound);
	  
	  if(bFound) {
	    int nByteLength = 0;
	    
	    // NOTE(winkler): We just copy over the incoming bytes in
	    // their according data structures and afterwards assign
	    // the value to fValue. This way, we let the compiler to
	    // the magic of conversion.
	    float fValue = 0;
	    
	    switch(teCurrent.nType) {
	    case 1: { // UINT8
	      nByteLength = 1;
	      uint8_t uint8Value;
	      memcpy(&uint8Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint8Value;
	    } break;
	      
	    case 2: { // UINT16
	      nByteLength = 2;
	      uint16_t uint16Value;
	      memcpy(&uint16Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint16Value;
	    } break;
	      
	    case 3: { // UINT32
	      nByteLength = 4;
	      uint32_t uint32Value;
	      memcpy(&uint32Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint32Value;
	    } break;
	      
	    case 4: { // INT8
	      nByteLength = 1;
	      int8_t int8Value;
	      memcpy(&int8Value, &cLogdata[nOffset], nByteLength);
	      fValue = int8Value;
	    } break;
	      
	    case 5: { // INT16
	      nByteLength = 2;
	      int16_t int16Value;
	      memcpy(&int16Value, &cLogdata[nOffset], nByteLength);
	      fValue = int16Value;
	    } break;
	      
	    case 6: { // INT32
	      nByteLength = 4;
	      int32_t int32Value;
	      memcpy(&int32Value, &cLogdata[nOffset], nByteLength);
	      fValue = int32Value;
	    } break;
	      
	    case 7: { // FLOAT
	      nByteLength = 4;
	      memcpy(&fValue, &cLogdata[nOffset], nByteLength);
	    } break;
	      
	    case 8: { // FP16
	      // NOTE(winkler): This is untested code (as no FP16
	      // variable gets advertised yet). This has to be tested
	      // and is to be used carefully. I will do that as soon
	      // as I find time for it.
	      nByteLength = 2;
	      char cBuffer1[nByteLength];
	      char cBuffer2[4];
	      memcpy(cBuffer1, &cLogdata[nOffset], nByteLength);
	      cBuffer2[0] = cBuffer1[0] & 0b10000000; // Get the sign bit
	      cBuffer2[1] = 0;
	      cBuffer2[2] = cBuffer1[0] & 0b01111111; // Get the magnitude
	      cBuffer2[3] = cBuffer1[1];
	      memcpy(&fValue, cBuffer2, 4); // Put it into the float variable
	    } break;
	      
	    default: { // Unknown. This hopefully never happens.
	    } break;
	    }
	    
	    setFloatValueForElementID(ctoc,nElementID, fValue);
	    nOffset += nByteLength;
	    nIndex++;
	  } else {
	    printf("Did not find element ID %d in block ID %d while parsing incoming logging data.\n", nElementID,nBlockID); //<< nElementID
		 //<<  << nBlockID
		 //<< " while parsing incoming logging data." << endl;
	    //cerr <<  << endl;
      printf("%s\n","This REALLY shouldn't be happening!" );
	    exit(-1);
	  }

    //add by tyun
    if(ifsethold==true){
        //printf("%s %d\n", "going to try with nElementID ",nElementID);
        sendHoldParam(ctoc,crtpPacket,nElementID);
      }
	}
      }
      
      delete crtpPacket;
    }
  }
}

int elementIDinBlock(CTOC* crCTOC,int nBlockID, int nElementIndex) {
  bool bFound;
  
  struct LoggingBlock lbCurrent = loggingBlockForID(crCTOC,nBlockID, bFound);
  if(bFound) {
    if(nElementIndex < lbCurrent.lstElementIDs.size()) {
      list<int>::iterator itID = lbCurrent.lstElementIDs.begin();
      advance(itID, nElementIndex);
      return *itID;
    }
  }
  
  return -1;
}

//Change to pointer and list
bool setFloatValueForElementID(CTOC* crCTOC,int nElementID, float fValue) {
  int nIndex = 0;
  for(list<struct TOCElement>::iterator itElement = crCTOC->m_lstTOCElements.begin();
      itElement != crCTOC->m_lstTOCElements.end();
      itElement++, nIndex++) {
    struct TOCElement teCurrent = *itElement;
    
    if(teCurrent.nID == nElementID) {
      teCurrent.dValue = fValue; // We store floats as doubles
      (*itElement) = teCurrent;
      // cout << fValue << endl;
      return true;
    }
  }
  
  return false;
}
