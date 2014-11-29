#ifndef __C_TOC_H__
#define __C_TOC_H__


#include <list>
#include <string>
#include <stdlib.h>


#include "CCrazyRadio.h"
#include "CCRTPPacket.h"

using namespace std;


/*! \brief Storage element for logged variable identities */
struct TOCElement {
  /*! \brief The numerical ID of the log element on the copter's
      internal table */
  int nID;
  /*! \brief The (ref) type of the log element */
  int nType;
  /*! \brief The string group name of the log element */
  string strGroup;
  /*! \brief The string identifier of the log element */
  string strIdentifier;
  bool bIsLogging;
  double dValue;
};


struct LoggingBlock {
  string strName;
  int nID;
  double dFrequency;
  list<int> lstElementIDs;
};


typedef struct _CTOC {
  int m_nPort;
  CCrazyRadio *m_crRadio;
  int m_nItemCount;
  list<struct TOCElement> m_lstTOCElements;
  list<struct LoggingBlock> m_lstLoggingBlocks;
  
}CTOC;
  
  bool requestInitialItem(CTOC* crCTOC);
  bool requestItem2(CTOC* crCTOC,int nID, bool bInitial);
  bool requestItem1(CTOC* crCTOC,int nID);
  bool processItem(CTOC* crCTOC,CCRTPPacket *crtpItem);
  
  CCRTPPacket *sendAndReceive(CCRTPPacket *crtpSend, int nChannel);
  
 
  void CTOCConstructor(CTOC* crCTOC,CCrazyRadio *crRadio, int nPort);
  
  bool sendTOCPointerReset(CTOC* crCTOC);
  bool requestMetaData(CTOC* crCTOC);
  bool requestItems(CTOC* crCTOC);
  
  struct TOCElement elementForName(CTOC* crCTOC,string strName, bool &bFound);
  struct TOCElement elementForID(CTOC* crCTOC,int nID, bool &bFound);
  int idForName(CTOC* cotc,string strName);
  int typeForName(CTOC* cotc,string strName);
  
  // For loggable variables only
  bool registerLoggingBlock(CTOC* cotc,string strName, double dFrequency);
  bool unregisterLoggingBlock(CTOC* cotc,string strName);
  struct LoggingBlock loggingBlockForName(CTOC* cotc,string strName, bool &bFound);
  struct LoggingBlock loggingBlockForID(CTOC* cotc,int nID, bool &bFound);
  
  bool startLogging(CTOC* cotc,string strName, string strBlockName);
  bool stopLogging(string strName);
  bool isLogging(string strName);
  
  double doubleValue(CTOC* cotc,string strName);
  
  bool enableLogging(CTOC* cotc,string strBlockName);
  
  void processPackets(CTOC* cotc,list<CCRTPPacket*> lstPackets,bool ifsethold);
  bool sendHoldParam(CTOC* ctoc,CCRTPPacket *crtpPacket,int nElementID);
  
  int elementIDinBlock(CTOC* crCTOC,int nBlockID, int nElementIndex);
  bool setFloatValueForElementID(CTOC* crCTOC,int nElementID, float fValue);
  bool addElementToBlock(CTOC* cotc,int nBlockID, int nElementID);
  bool unregisterLoggingBlockID(CTOC* cotc,int nID);



#endif /* __C_TOC_H__ */
