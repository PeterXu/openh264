#ifndef _H264PARSER_H_
#define _H264PARSER_H_

#include <stdint.h>
#include <sys/types.h>

/**
 * It will be called when AVC stream generated, the same thread with DecoderParser
 *
 * @param frame   avc frame buffer
 * @param size    the size of frame
 * @param nalmum  the number nals in frame
 * @param width   the width
 * @param height  the height
 * @param userdata  userdata which is passed in DecoderParser
 */
typedef void (* FnParserCallback)(const uint8_t *frame, size_t size, int nalnum, int width, int height, void *userdata);


/**
 * How to use: 
 *
 * ISVCParser *pParser = NULL;
 * step1: WelsCreateParser(&pParser);
 * step2: pParser->Initialize();
 * step3: pParser->SetCallback(..);
 * step4: pParser->DecoderParser(svc, svc_size);
 *        if parsing success and generating AVC stream, then the callback function(config in step3) will be called.
 *        This step can be called multi-times until there are no SVC stream.
 * step5: WelsDestroyParser(&pParser); it contains the call of pParser->Uninitialize().
 */

class ISVCParser {
public:
  virtual ~ISVCParser() {}

  virtual long Initialize () = 0;
  virtual long Uninitialize() = 0;

  /**
   * Config the callback function when AVC stream generated
   *
   * @param callback    the callback function
   * @param userdata    the userdata which will be returned in callback function
   */
  virtual void SetCallback(FnParserCallback callback, void *userdata) = 0;

  /**
   * Parse SVC to AVC stream(this api can be called many times until no svc nal).
   *
   * @param pBuf    the svc nal data(one or many nals, but each nal must have h264 prefix header(0x00000001))
   *                However, suggestion: the max size of passed data is not more than 1M each time.
   * @param iSize   the size of nal data in pBuf
   */
  virtual long DecodeParser (const uint8_t *pBuf, size_t iSize) = 0;
};


long WelsCreateParser (ISVCParser **ppParser);
void WelsDestroyParser(ISVCParser *pParser);

#endif // _H264PARSER_H_
