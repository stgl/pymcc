// DataIndex.h: interface for the CDataIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAINDEX_H__24EDA234_FB3E_4E62_AACF_2CF9A4D55BB2__INCLUDED_)
#define AFX_DATAINDEX_H__24EDA234_FB3E_4E62_AACF_2CF9A4D55BB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

// save current structure packing value and set to 2 so LIDARRETURN structure doesn't have any extra padding
#pragma pack(push, before_dataindex, 2)

typedef struct IndexEntry {
	unsigned char	Row;				// grid row containing point
	unsigned char	Column;				// grid column containing point
	long			Offset;				// offset to start of point data
} INDEXENTRY;

class CDataIndex  
{
public:
	bool UpdateChecksum(const std::string & DataFileName);
	void ResetRange();
	long GetCellStartValue(int Row, int Col);
	struct header {
		char		Signature[12];			// file identifier "LDAindex"
		float		Version;				// version identifier
		long		Checksum;				// checksum computed from data file to detect file changes
		int			Format;					// format of data file
		double		MinX;					// minimum x value in data
		double		MinY;					// minimum y value in data
		double		MinZ;					// minimum z value in data
		double		MaxX;					// maximum x value in data
		double		MaxY;					// maximum y value in data
		double		MaxZ;					// maximum z value in data
		int			GridCellsAcross;		// number of grid cells in x direction
		int			GridCellsUp;			// number of grid cells in y direction
		int			TotalPointsIndexed;		// total number of points in data file
	};

	struct header m_Header;

	long JumpToNextPointInRange(double minx, double miny, double maxx, double maxy, FILE* datafile = NULL);
	void Close();
	bool IsValid();
	bool Open(const std::string & DataFileName, bool UseStarts = false);
	bool ReadHeader(FILE* handle = NULL);
	bool VerifyChecksum(const std::string & DataFileName, int UseOldMethod = false);
	bool CreateIndex(const std::string & DataFileName, int Format, int Width = 256, int Height = 256, int NumberOfPoints = 0, double MinX = 0.0, double MaxX = 0.0, double MinY = 0.0, double MaxY = 0.0, double MinZ = 0.0, double MaxZ = 0.0, bool Optimize = true);
	CDataIndex();
	CDataIndex(const std::string & DataFileName, int Format, int Width = 256, int Height = 256);
	virtual ~CDataIndex();

private:
	long* m_CellStarts;
	bool m_HaveStarts;
	double m_Range_MinX;
	double m_Range_MinY;
	double m_Range_MaxX;
	double m_Range_MaxY;
	unsigned char m_Range_MinCellColumn;
	unsigned char m_Range_MaxCellColumn;
	unsigned char m_Range_MinCellRow;
	unsigned char m_Range_MaxCellRow;

	bool m_HaveDataRange;
	bool WritePoint(FILE* handle = NULL);
	bool ReadPoint(FILE* handle = NULL);
	bool WriteHeader(FILE* handle = NULL);
	FILE* m_FileHandle;

	struct pt {
		unsigned char	Row;				// grid row containing point
		unsigned char	Column;				// grid column containing point
		long			Offset;				// offset to start of point data
	};
	bool m_Valid;

protected:
	long ComputeChecksum(const std::string & DataFileName, int UseOldMethod = false);
	struct pt m_Point;
	std::string m_DataFileName;
	std::string m_IndexFileName;
};

// restore structure packing
#pragma pack(pop, before_dataindex)

#endif // !defined(AFX_DATAINDEX_H__24EDA234_FB3E_4E62_AACF_2CF9A4D55BB2__INCLUDED_)
