/*!
 * h264parser.cpp:         Wels Parser for SVC
 */

#include "h264parser.h"

#include "error_code.h"
#include "codec_def.h"
#include "codec_app_def.h"
#include "codec_api.h"
#include "typedefs.h"
#include "measure_time.h"

#include <string.h>
#include <stdio.h>

#if defined(ANDROID_NDK)
#include <android/log.h>
#define LOG_TAG "welsparser"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) fprintf(stderr, __VA_ARGS__)
#define LOGI(...) fprintf(stderr, __VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#endif


class CWelsParser : public ISVCParser {
public:
  CWelsParser() {
    m_pDecoder = NULL;
    m_fnCallback = NULL;
    m_pUserData = NULL;
  }
  virtual ~CWelsParser() {
    Uninitialize();
    m_fnCallback = NULL;
    m_pUserData = NULL;
  }

  virtual long Initialize ();
  virtual long Uninitialize ();
  virtual long DecodeParser (const uint8_t *pBuf, size_t iSize);
  virtual void SetCallback (FnParserCallback callback, void *userdata);

private:
  ISVCDecoder *m_pDecoder;
  FnParserCallback m_fnCallback;
  void *m_pUserData;
};



long CWelsParser::DecodeParser (const uint8_t *pBuf, size_t iSize) {
  if (m_pDecoder == NULL)
    return -1;

  int32_t iRet = 0;
  int32_t iBufPos = 0;
  SParserBsInfo sDstParseInfo;

  while (true) {
    if (iBufPos >= iSize) { // no h264 data
      break;
    }

    // To acquire one nal from buffer
    int32_t i = 0;
    for (i = 0; i < iSize; i++) {
      if ((pBuf[iBufPos+i] == 0 && pBuf[iBufPos+i+1] == 0 && pBuf[iBufPos+i+2] == 0 && pBuf[iBufPos+i+3] == 1 && i > 0) || 
          (pBuf[iBufPos+i] == 0 && pBuf[iBufPos+i+1] == 0 && pBuf[iBufPos+i+2] == 1 && i > 0)) {
          break;
        }
    }

    int32_t iSliceSize = i;
    if (iSliceSize < 4) { //too small size, no effective data, ignore
      iBufPos += iSliceSize;
      continue;
    }

    int iSteps = 2; // two steps for parsing
    const uint8_t *pAvcData = pBuf+iBufPos;
    iBufPos += iSliceSize; // for next nal
    memset(&sDstParseInfo, 0, sizeof(SParserBsInfo));

    do {
      iRet = m_pDecoder->DecodeParser(pAvcData, iSliceSize, &sDstParseInfo);
      if (sDstParseInfo.iNalNum > 0) {
        int32_t iNalNum = sDstParseInfo.iNalNum;
        int32_t iWidth  = sDstParseInfo.iSpsWidthInPixel;
        int32_t iHeight = sDstParseInfo.iSpsHeightInPixel;

        int32_t iDstPos = 0;
        for (int k=0; k < iNalNum; k++) {
          int32_t iFrameSize = sDstParseInfo.iNalLenInByte[k];
          if (m_fnCallback && iFrameSize > 4) {
            m_fnCallback(sDstParseInfo.pDstBuff+iDstPos, iFrameSize, 1, iWidth, iHeight, m_pUserData);
          }
          iDstPos += iFrameSize;
        }
      }

      if (iRet != 0) {
        LOGE("DecodeParser - step%d - error: %d, pos: %d\n", (3-iSteps), iRet, iBufPos);
      }

      if (iSteps == 2) {
        pAvcData = NULL;
        iSliceSize = 0;
      }
    } while((--iSteps) > 0);
  }

  return iRet;
}

void CWelsParser::SetCallback(FnParserCallback callback, void *userdata) {
  m_fnCallback = callback;
  m_pUserData = userdata;
}

long CWelsParser::Initialize() {
  if (m_pDecoder) {
    LOGE("Decoder exist.\n");
    return -1;
  }

  ISVCDecoder* pDecoder = NULL;
  int iRet = WelsCreateDecoder (&pDecoder);
  if (iRet != 0) {
    LOGE("Create Decoder failed.\n");
    return iRet;
  }

  SDecodingParam sDecParam = {0};
  sDecParam.uiTargetDqLayer = -1; // required
  sDecParam.bParseOnly = true;
  sDecParam.sVideoProperty.size = sizeof (sDecParam.sVideoProperty);

  int iLevelSetting = (int) WELS_LOG_INFO;
  pDecoder->SetOption (DECODER_OPTION_TRACE_LEVEL, &iLevelSetting);

  iRet = pDecoder->Initialize (&sDecParam);
  if (iRet != cmResultSuccess) {
    pDecoder->Uninitialize();
    WelsDestroyDecoder (pDecoder);
    LOGE("Decoder initialization failed.\n");
    return iRet;
  }

  m_pDecoder = pDecoder;
  return cmResultSuccess;
}

long CWelsParser::Uninitialize() {
  if (m_pDecoder) {
    m_pDecoder->Uninitialize();
    WelsDestroyDecoder (m_pDecoder);
    m_pDecoder = NULL;
  }
  return cmResultSuccess;
}


long WelsCreateParser (ISVCParser **ppParser) {
  if (NULL == ppParser) {
    return WelsDec::ERR_INVALID_PARAMETERS;
  }

  *ppParser = new CWelsParser();
  if (NULL == *ppParser) {
    return WelsDec::ERR_MALLOC_FAILED;
  }

  return WelsDec::ERR_NONE;
}

void WelsDestroyParser(ISVCParser *pParser) {
  if (pParser) {
    delete pParser;
  }
}

