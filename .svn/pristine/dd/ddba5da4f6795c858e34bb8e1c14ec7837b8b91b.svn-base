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

#include "cflie/CCrazyRadio.h"

void CCrazyRadioConstructor(CCrazyRadio* crRadio,string strRadioIdentifier) {
  crRadio->m_strRadioIdentifier = strRadioIdentifier;
  crRadio->m_enumPower = P_M18DBM;
  
  crRadio->m_ctxContext = NULL;
  crRadio->m_hndlDevice = NULL;
  
  crRadio->m_bAckReceived = false;
  
  /*int nReturn = */libusb_init(&(crRadio->m_ctxContext));
  
  // Do error checking here.
}

void CCrazyRadioDestructor(CCrazyRadio* crRadio) {
  closeDevice(crRadio);
  
  // TODO(winkler): Free all remaining packets in m_lstLoggingPackets.
  
  if(crRadio->m_ctxContext) {
    libusb_exit(crRadio->m_ctxContext);
  }
}

void closeDevice(CCrazyRadio* crRadio) {
  if(crRadio->m_hndlDevice) {
    libusb_close(crRadio->m_hndlDevice);
    libusb_unref_device(crRadio->m_devDevice);
    
    crRadio->m_hndlDevice = NULL;
    crRadio->m_devDevice = NULL;
  }
}

//Find the device via the ID
list<libusb_device*> listDevices(CCrazyRadio* crRadio,int nVendorID, int nProductID) {
  list<libusb_device*> lstDevices;
  ssize_t szCount;
  libusb_device **ptDevices;
  
  szCount = libusb_get_device_list(crRadio->m_ctxContext, &ptDevices);
  for(unsigned int unI = 0; unI < szCount; unI++) {
    libusb_device *devCurrent = ptDevices[unI];
    libusb_device_descriptor ddDescriptor;
    
    libusb_get_device_descriptor(devCurrent, &ddDescriptor);
    
    if(ddDescriptor.idVendor == nVendorID && ddDescriptor.idProduct == nProductID) {
      libusb_ref_device(devCurrent);
      lstDevices.push_back(devCurrent);
    }
  }
  
  if(szCount > 0) {
    libusb_free_device_list(ptDevices, 1);
  }
  
  return lstDevices;
}

bool openUSBDongle(CCrazyRadio* crRadio) {
  closeDevice(crRadio);
  list<libusb_device*> lstDevices = listDevices(crRadio,0x1915, 0x7777);
  
  if(lstDevices.size() > 0) {
    // For now, just take the first device. Give it a second to
    // initialize the system permissions.
    sleep(1.0);
    
    libusb_device *devFirst = lstDevices.front();
    int nError = libusb_open(devFirst, &(crRadio->m_hndlDevice));
    
    if(nError == 0) {
      // Opening device OK. Don't free the first device just yet.
      lstDevices.pop_front();
      crRadio->m_devDevice = devFirst;
    }
    
    for(list<libusb_device*>::iterator itDevice = lstDevices.begin();
	itDevice != lstDevices.end();
	itDevice++) {
      libusb_device *devCurrent = *itDevice;
      
      libusb_unref_device(devCurrent);
    }
    
    return !nError;
  }
  
  return false;
}

bool startRadio(CCrazyRadio* crRadio) {
  if(openUSBDongle(crRadio)) {
    int nDongleNBR;
    int nRadioChannel;
    int nDataRate;
    char cDataRateType;
    
    if(sscanf((crRadio->m_strRadioIdentifier).c_str(), "radio://%d/%d/%d%c",
	      &nDongleNBR, &nRadioChannel, &nDataRate,
	      &cDataRateType) != EOF) {
      //cout << "Opening radio " << nDongleNBR << "/" << nRadioChannel << "/" << nDataRate << cDataRateType << endl;
      printf("Opening radio %d/%d/%d/%c\n", nDongleNBR,nRadioChannel,nDataRate,cDataRateType);
      stringstream sts;
      sts << nDataRate;
      sts << cDataRateType;
      string strDataRate = sts.str();
      
      // Read device version
      libusb_device_descriptor ddDescriptor;
      libusb_get_device_descriptor(crRadio->m_devDevice, &ddDescriptor);
      sts.clear();
      sts.str(std::string());
      sts << (ddDescriptor.bcdDevice >> 8);
      sts << ".";
      sts << (ddDescriptor.bcdDevice & 0x0ff);
      sscanf(sts.str().c_str(), "%f", &(crRadio->m_fDeviceVersion));
      
      printf("Got device version %f\n", crRadio->m_fDeviceVersion);
      //cout << "Got device version " << m_fDeviceVersion << endl;
      if(crRadio->m_fDeviceVersion < 0.3) {
	return false;
      }
      
      // Set active configuration to 1
      libusb_set_configuration(crRadio->m_hndlDevice, 1);
      
      // Claim interface
      if(claimInterface(crRadio,0)) {
	// Set power-up settings for dongle (>= v0.4)
	setDataRate(crRadio,"2M");
	setChannel(crRadio,2);
	
	if(crRadio->m_fDeviceVersion >= 0.4) {
	  setContCarrier(crRadio,false);
	  char cAddress[5];
	  cAddress[0] = 0xe7;
	  cAddress[1] = 0xe7;
	  cAddress[2] = 0xe7;
	  cAddress[3] = 0xe7;
	  cAddress[4] = 0xe7;
	  setAddress(crRadio,cAddress);
	  setPower(crRadio,P_0DBM);
	  setARC(crRadio,3);
	  setARDBytes(crRadio,32);
	}
	
	// Initialize device
	if(crRadio->m_fDeviceVersion >= 0.4) {
	  setARC(crRadio,10);
	}
	
	setChannel(crRadio,nRadioChannel);
	setDataRate(crRadio,strDataRate);
	
	return true;
      }
    }
  }
  
  return false;
}

CCRTPPacket *writeData(CCrazyRadio* crRadio,void *vdData, int nLength) {
  CCRTPPacket *crtpPacket = NULL;
  
  int nActuallyWritten;
  int nReturn = libusb_bulk_transfer(crRadio->m_hndlDevice, (0x01 | LIBUSB_ENDPOINT_OUT), (unsigned char*)vdData, nLength, &nActuallyWritten, 1000);
  
  if(nReturn == 0 && nActuallyWritten == nLength) {
    crtpPacket = readACK(crRadio);
  }
  
  return crtpPacket;
}

bool readData(CCrazyRadio* crRadio,void *vdData, int &nMaxLength) {
  int nActuallyRead;
  int nReturn = libusb_bulk_transfer(crRadio->m_hndlDevice, (0x81 | LIBUSB_ENDPOINT_IN), (unsigned char*)vdData, nMaxLength, &nActuallyRead, 50);
  
  if(nReturn == 0) {
    nMaxLength = nActuallyRead;
    
    return true;
  } else {
    switch(nReturn) {
    case LIBUSB_ERROR_TIMEOUT:
      printf("%s\n", "USB timeout"); 
      break;
      
    default:
      break;
    }
  }
  
  return false;
}

bool writeControl(CCrazyRadio* crRadio,void *vdData, int nLength, uint8_t u8Request, uint16_t u16Value, uint16_t u16Index) {
  int nTimeout = 1000;
  /*int nReturn = */libusb_control_transfer(crRadio->m_hndlDevice, LIBUSB_REQUEST_TYPE_VENDOR, u8Request, u16Value, u16Index, (unsigned char*)vdData, nLength, nTimeout);
  
  // if(nReturn == 0) {
  //   return true;
  // }
  
  // Hack.
  return true;
}

void setARC(CCrazyRadio* crRadio,int nARC) {
  crRadio->m_nARC = nARC;
  writeControl(crRadio,NULL, 0, 0x06, nARC, 0);
}

void setChannel(CCrazyRadio* crRadio,int nChannel) {
  crRadio->m_nChannel = nChannel;
  writeControl(crRadio,NULL, 0, 0x01, nChannel, 0);
}

void setDataRate(CCrazyRadio* crRadio,string strDataRate) {
  crRadio->m_strDataRate = strDataRate;
  int nDataRate = -1;
  if(crRadio->m_strDataRate == "250K") {
    nDataRate = 0;
  } else if(crRadio->m_strDataRate == "1M") {
    nDataRate = 1;
  } else if(crRadio->m_strDataRate == "2M") {
    nDataRate = 2;
  }
  writeControl(crRadio,NULL, 0, 0x03, nDataRate, 0);
}

void setARDTime(CCrazyRadio* crRadio,int nARDTime) { // in uSec
  crRadio->m_nARDTime = nARDTime;
  int nT = int((nARDTime / 250) - 1);
  if(nT < 0) {
    nT = 0;
  } else if(nT > 0xf) {
    nT = 0xf;
  }
  writeControl(crRadio,NULL, 0, 0x05, nT, 0);
}

void setARDBytes(CCrazyRadio* crRadio,int nARDBytes) {
  crRadio->m_nARDBytes = nARDBytes;
  
  writeControl(crRadio,NULL, 0, 0x05, 0x80 | nARDBytes, 0);
}

//Get power
enum Power power(CCrazyRadio* crRadio) {
  return crRadio->m_enumPower;
}

//Set Power
void setPower(CCrazyRadio* crRadio,enum Power enumPower) {
  crRadio->m_enumPower = enumPower;
  writeControl(crRadio,NULL, 0, 0x04, enumPower, 0);
}

void setAddress(CCrazyRadio* crRadio,char *cAddress) {
  crRadio->m_cAddress = cAddress;
  writeControl(crRadio,cAddress, 5, 0x02, 0, 0);
}

void setContCarrier(CCrazyRadio* crRadio,bool bContCarrier) {
  crRadio->m_bContCarrier = bContCarrier;
  writeControl(crRadio,NULL, 0, 0x20, (bContCarrier ? 1 : 0), 0);
}

bool claimInterface(CCrazyRadio* crRadio,int nInterface) {
  return libusb_claim_interface(crRadio->m_hndlDevice, nInterface) == 0;
}

CCRTPPacket *sendPacket(CCrazyRadio* crRadio,CCRTPPacket *crtpSend, bool bDeleteAfterwards) {
  CCRTPPacket *crtpPacket = NULL;
  
  char *cSendable = sendableData(crtpSend);
  crtpPacket = writeData(crRadio,cSendable, sendableDataLength(crtpSend));
    
  delete[] cSendable;
    
  if(crtpPacket) {
    char *cData = data(crtpPacket);
    int nLength = dataLength(crtpPacket);
    
    if(nLength > 0) {
      short sPort = (cData[0] & 0xf0) >> 4;
      setPort(crtpPacket,sPort);
      short sChannel = cData[0] & 0b00000011;
      setChannel(crtpPacket,sChannel);
      
      switch(sPort) {
      case 0: { // Console
	char cText[nLength];
	memcpy(cText, &cData[1], nLength - 1);
	cText[nLength - 1] = '\0';
	  
  printf("Console text: %s\n", cText);
	//cout << "" << cText << endl;
      } break;
	
      case 5: { // Logging
	if(channel(crtpPacket) == 2) {
	  //CCRTPPacket *crtpLog = new CCRTPPacket(cData, nLength, crtpPacket->channel());
	  //CCRTPPacket *crtpLog = (CCRTPPacket *)malloc(sizeof(CCRTPPacket));
    CCRTPPacket *crtpLog = new CCRTPPacket;
  CCRTPPacketInit3(crtpLog,cData,nLength,channel(crtpPacket));

    setChannel(crtpLog,channel(crtpPacket));
	  setPort(crtpLog,port(crtpPacket));
	  
	  crRadio->m_lstLoggingPackets.push_back(crtpLog);
	}
      } break;
      }
    }
  }
  
  if(bDeleteAfterwards) {
    delete crtpSend;
  }
  
  return crtpPacket;
}

CCRTPPacket *readACK(CCrazyRadio* crRadio) {
  CCRTPPacket *crtpPacket = NULL;
  
  int nBufferSize = 64;
  char cBuffer[nBufferSize];
  int nBytesRead = nBufferSize;
  
  if(readData(crRadio,cBuffer, nBytesRead)) {
    if(nBytesRead > 0) {
      // Analyse status byte
      crRadio->m_bAckReceived = true;//cBuffer[0] & 0x1;
      //bool bPowerDetector = cBuffer[0] & 0x2;
      //int nRetransmissions = cBuffer[0] & 0xf0;
      
      // TODO(winkler): Do internal stuff with the data received here
      // (store current link quality, etc.). For now, ignore it.
      
      //crtpPacket = new CCRTPPacket(0);
      //crtpPacket = (CCRTPPacket *)malloc(sizeof(CCRTPPacket)); //CCRTPPacket(0x00, 0);
    crtpPacket = new CCRTPPacket;
    CCRTPPacketInit1(crtpPacket,0);
      
      if(nBytesRead > 1) {
      	setData(crtpPacket,&cBuffer[1], nBytesRead);
      }
    } else {
      crRadio->m_bAckReceived = false;
    }
  }
  
  return crtpPacket;
}

bool ackReceived(CCrazyRadio* crRadio) {
  return crRadio->m_bAckReceived;
}

bool usbOK(CCrazyRadio* crRadio) {
  libusb_device_descriptor ddDescriptor;
  return (libusb_get_device_descriptor(crRadio->m_devDevice,
				       &ddDescriptor) == 0);
}

CCRTPPacket *waitForPacket(CCrazyRadio* crRadio) {
  bool bGoon = true;
  CCRTPPacket *crtpReceived = NULL;
  //CCRTPPacket *crtpDummy = new CCRTPPacket(0);
  //CCRTPPacket *crtpDummy = (CCRTPPacket *)malloc(sizeof(CCRTPPacket)); //CCRTPPacket(0x00, 0);
  CCRTPPacket *crtpDummy = new CCRTPPacket;
  CCRTPPacketInit1(crtpDummy,0);

  setIsPingPacket(crtpDummy,true);
  
  while(bGoon) {
    crtpReceived = sendPacket(crRadio,crtpDummy);
    bGoon = (crtpReceived == NULL);
  }
  
  delete crtpDummy;
  return crtpReceived;
}

CCRTPPacket *sendAndReceive(CCrazyRadio* crRadio,CCRTPPacket *crtpSend, bool bDeleteAfterwards) {
  return sendAndReceiveInner(crRadio,crtpSend, port(crtpSend), channel(crtpSend), bDeleteAfterwards);
}

CCRTPPacket *sendAndReceiveInner(CCrazyRadio* crRadio,CCRTPPacket *crtpSend, int nPort, int nChannel, bool bDeleteAfterwards, int nRetries, int nMicrosecondsWait) {
  bool bGoon = true;
  int nResendCounter = 0;
  CCRTPPacket *crtpReturnvalue = NULL;
  CCRTPPacket *crtpReceived = NULL;
  
  while(bGoon) {
    if(nResendCounter == 0) {
      crtpReceived = sendPacket(crRadio,crtpSend);
      nResendCounter = nRetries;
    } else {
      nResendCounter--;
    }
    
    if(crtpReceived) {
      if(port(crtpReceived) == nPort &&
	 channel(crtpReceived) == nChannel) {
	crtpReturnvalue = crtpReceived;
	bGoon = false;
      }
    }
    
    if(bGoon) {
      if(crtpReceived) {
	delete crtpReceived;
      }
      
      usleep(nMicrosecondsWait);
      crtpReceived = waitForPacket(crRadio);
    }
  }
  
  if(bDeleteAfterwards) {
    delete crtpSend;
  }
  
  return crtpReturnvalue;
}

list<CCRTPPacket*> popLoggingPackets(CCrazyRadio* crRadio) {
  list<CCRTPPacket*> lstPackets = crRadio->m_lstLoggingPackets;
  (crRadio->m_lstLoggingPackets).clear();
  
  return lstPackets;
}

//Test sending packet
bool sendDummyPacket(CCrazyRadio* crRadio) {
  CCRTPPacket *crtpReceived = NULL;
  //CCRTPPacket *crtpDummy = new CCRTPPacket(0);
  //CCRTPPacket *crtpDummy = (CCRTPPacket *)malloc(sizeof(CCRTPPacket)); //CCRTPPacket(0x00, 0);
  CCRTPPacket *crtpDummy = new CCRTPPacket;
  CCRTPPacketInit1(crtpDummy,0);

  setIsPingPacket(crtpDummy,true);
  
  crtpReceived = sendPacket(crRadio,crtpDummy, true);
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}
