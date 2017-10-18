#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "codec/parser/inc/h264parser.h"


static int SaveData(const uint8_t *data, size_t len, const char* aFileName)
{
	FILE* f = fopen(aFileName, "a+");
	if (f == NULL||data == NULL||len < 4)
		return -1;
	int ret = fwrite(data, 1, len, f); 
	fclose(f);
    return ret;
}

static void SVCParserCallback(const uint8_t *frame, size_t size, int nalnum, int width, int height, void *userdata);

class CSVCConvertor{
public:
	CSVCConvertor() {
		m_pSVCParser = NULL;
		m_szOutName = "";
	}
	~CSVCConvertor() {
		Uninit();
	}

	void Init(std::string outName) {
		WelsCreateParser(&m_pSVCParser);
		m_pSVCParser->Initialize();
		m_pSVCParser->SetCallback(SVCParserCallback, (void *)(this));
		m_szOutName = outName;
	}
	void Uninit() {
		WelsDestroyParser(m_pSVCParser);
		m_pSVCParser = NULL;
		m_szOutName = "";
	}

	void ParseSvcNal(unsigned char *pNalData, unsigned long nNalLen) {
		if (m_pSVCParser)
			m_pSVCParser->DecodeParser(pNalData, nNalLen, true);
	}

	void OnParseOutAVCData(const uint8_t *frame, size_t size, int nalnum, int width, int height) {
		//printf("avc len=%u\n",size);
		if (m_szOutName.size() >= 4) {
			SaveData(frame, size, m_szOutName.c_str());
		}
	}

private:
	ISVCParser *m_pSVCParser;
	std::string m_szOutName;
};

void SVCParserCallback(const uint8_t *frame, size_t size, int nalnum, int width, int height, void *userdata)
{
	CSVCConvertor *pConvertor = (CSVCConvertor *)userdata;
	if (pConvertor)
		pConvertor->OnParseOutAVCData(frame, size, nalnum, width, height);
}

int main(int argc, char *argv[])
{
	const long max_fsize = 1024*1024*128;

	std::string inName, outName;
	FILE *fp = NULL;
	long fsize = 0;
	unsigned char *fbuffer = NULL;
	long fdatlen = 0;
	CSVCConvertor conv;

	if (argc == 2) {
		inName = argv[1];
	}else if (argc == 3) {
		inName = argv[1];
		outName = argv[2];
	}else {
		printf("usage: %s in_svcfile [out_avcfile]\n", argv[0]);
		printf("       supported max size of in_svcfile: %ldMB\n", max_fsize/1024/1024);
		printf("\n");
		exit(0);
	}

	fp = fopen(inName.c_str(), "rb");
	if (!fp) {
		printf("fail to open: %s\n", inName.c_str());
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	printf("svc file len=%ld\n", fsize);
	if (fsize > max_fsize) {
		printf("in file too large > %ld\n", max_fsize);
		goto fend;
	}

	fbuffer = new unsigned char[fsize]; 
	memset(fbuffer, 0, fsize);
	fseek(fp, 0L, SEEK_SET);
	fdatlen = fread(fbuffer, 1, fsize, fp);
	if (fsize != fdatlen) {
		printf("read len=%ld, not equal to file size=%ld\n", fdatlen, fsize);
		goto fend;
	}

	conv.Init(outName);
	conv.ParseSvcNal(fbuffer, fdatlen);

fend:
	if (fp)
		fclose(fp);
	if (fbuffer)
		delete fbuffer;
	return 0;
}
