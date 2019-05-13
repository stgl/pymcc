// LidarData.h: interface for the CLidarData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIDARDATA_H__DCBA98E0_2288_11D4_A124_A80F08C10B02__INCLUDED_)
#define AFX_LIDARDATA_H__DCBA98E0_2288_11D4_A124_A80F08C10B02__INCLUDED_

#include "DataFile.h"	// Added by ClassView
#include "LASFormatFile.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// save current structure packing value and set to 4 so LIDARRETURN structure doesn't have any extra padding
#pragma pack(push, before_lidardata, 4)

#define		INVALID			0
#define		ASCIIDATA		1
#define		BINDATA			2
#define		LASDATA			3

// ATTENTION.....
// I screwed up the original data conversion and switched Nadir angle and intensity when converting from ASCII text to LDA formats
// original order in structure was intensity then nadir
typedef struct {
	int PulseNumber;
	int ReturnNumber;
	double X;
	double Y;
	float Elevation;
	float NadirAngle;
	float Intensity;
} LIDARRETURN;

// structure for additional LAS info
typedef struct {
	unsigned char	NumberOfReturns;				// number of returns for the pulse
	unsigned char	ScanDirectionFlag;				// scan direction...1 indicates positive scan direction, 0 negative
	unsigned char	EdgeOfFlightLineFlag;			// flag indicating pulse is on edge of filght line...last pulse in scan line before changing direction
	uchar			V11Classification;				// bits 0-4: classification code for return...corresponds to variable length record in CLASClassificationLookupTable
	uchar			V11Synthetic;					// bit 5: 0 if point is from scan data, 1 if point was created by another method
	uchar			V11KeyPoint;					// bit 6: 0 if point is not a model key-point, 1 otherwise...key-points should not be removed by processing operations
	uchar			V11Withheld;					// bit 7: 0 if point is valid, 1 if point should not be included in processing (synonymous with deleted)
	uchar			UserData;						// file marker code...corresponds to variable length record in CLASFlightLineHeaderLookupTable
	ushort			PointSourceID;					// user bit field...version 1.1+ this is point source ID and it corresponds to the File Source ID in the public header
	double			GPSTime;						// GPS time at which the point was acquired
} LAS_RETURNFIELDS;

class CLidarData  
{
public:
	int GetFileFormat();
	bool ConvertToLAS(const std::string & OutputFileName, bool CreateIndex = false, bool PreserveLASInfo = false);
	bool ConvertToASCII(const std::string & OutputFileName, int Format = 0, int StartRecord = 0, int NumberOfRecords = -1);
	bool ConvertToBinary(const std::string & OutputFileName, bool CreateIndex = false, bool DecimateByPulse = false);
	long GetPosition();
	bool SetPosition(long Offset, int FromWhere);
	void Close();
	bool ReadNextRecord(LIDARRETURN* ldat, LAS_RETURNFIELDS* LASdat = NULL);
	void Rewind();
	bool Open(const std::string & szFileName);
	bool IsValid();
	CLASFormatFile m_LASFile;
	CLidarData(const std::string & szFileName);
	CLidarData();
	virtual ~CLidarData();

private:
	char* m_BinaryFileBuffer;
	bool m_HaveLocalBuffer;
	float m_Version;
	char m_LineBuffer[1024];
	int m_FileFormat;
	FILE* m_BinaryFile;
	CDataFile m_ASCIIFile;
	char buf2[16];			// dummy to prevent corruption of ASCII data file handle ?????
	bool m_Valid;
};

// restore structure packing
#pragma pack(pop, before_lidardata)

#endif // !defined(AFX_LIDARDATA_H__DCBA98E0_2288_11D4_A124_A80F08C10B02__INCLUDED_)
