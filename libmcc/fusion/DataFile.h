// DataFile.h: interface for the CDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAFILE_H__BF8B5780_DC45_11D2_A124_FC0D08C10B02__INCLUDED_)
#define AFX_DATAFILE_H__BF8B5780_DC45_11D2_A124_FC0D08C10B02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#define		READ		"rb"

#define		SKIPCOMMENTS			";%!"
#define		SKIPCOMMENTSCOMMANDS	";%!#"

class CDataFile  
{
public:
	bool ReadValue(double* val);
	bool ReadRawBytes(char* buf, int cnt, bool LessIsOK = false);
	int m_CurrentLineNumber;
	std::string m_FileName;
	bool ReadAll(char* buf);
	long GetFileSize();
	CDataFile(const std::string & FileName);
	CDataFile();
	virtual ~CDataFile();

	bool NewReadASCIILine(char *buf, bool SkipBlanks = true);
	bool ReadASCIILine(char *buf, bool SkipBlanks = true);
	bool ReadDataLine(char *buf, const char * flagchr, bool SkipBlanks = true);

	bool IsValid();
	bool Open(const std::string & FileName);
	void Close();

	bool SetPosition(long Offset, int FromWhere = SEEK_SET);
	long GetPosition();
	void Rewind();

private:
	void SetTerminatorLength();
	int m_TerminatorLength;
	FILE* m_FileHandle;
};

#endif // !defined(AFX_DATAFILE_H__BF8B5780_DC45_11D2_A124_FC0D08C10B02__INCLUDED_)
