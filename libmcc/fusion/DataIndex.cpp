// DataIndex.cpp: implementation of the CDataIndex class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "DataIndex.h"
#include "DataFile.h"
#include "LidarData.h"
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#if WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

int CompareIndexEntries(const void *a1, const void *a2)
{
	if (((INDEXENTRY*) a1)->Column > ((INDEXENTRY*) a2)->Column)
		return(1);
	else if (((INDEXENTRY*) a1)->Column < ((INDEXENTRY*) a2)->Column)
		return(-1);
	else {
		if (((INDEXENTRY*) a1)->Row > ((INDEXENTRY*) a2)->Row)
			return(1);
		else if (((INDEXENTRY*) a1)->Row < ((INDEXENTRY*) a2)->Row)
			return(-1);
		else
			return(0);
	}
}

// helper function for calling fopen with boost::filesystem::path
FILE * fopen(const boost::filesystem::path & path,
             const char *                    mode)
{
  return fopen(boost::filesystem::complete(path).file_string().c_str(), mode);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataIndex::CDataIndex()
{
	m_Valid = false;
	m_FileHandle = NULL;
	m_HaveDataRange = false;
	m_HaveStarts = false;
}

CDataIndex::CDataIndex(const std::string & DataFileName, int Format, int Width, int Height)
{
	m_Valid = false;
	m_FileHandle = NULL;
	m_HaveDataRange = false;
	m_HaveStarts = false;

	m_Valid = Open(DataFileName, true);

	if (!m_Valid)
		CreateIndex(DataFileName, Format, Width, Height);
}

CDataIndex::~CDataIndex()
{
	Close();
}

bool CDataIndex::CreateIndex(const std::string & DataFileName, int Format, int Width, int Height, int NumberOfPoints, double MinX, double MaxX, double MinY, double MaxY, double MinZ, double MaxZ, bool Optimize)
{
	if (m_HaveStarts) {
		delete [] m_CellStarts;
		m_HaveStarts = false;
	}

	// create index for the given data file
	m_DataFileName = DataFileName;

	// build index filename
  boost::filesystem::path fs(DataFileName);
	fs.replace_extension(".ldx");

	// populate header values
	strcpy(m_Header.Signature, "LDAindex");
	m_Header.Version = 1.1f;
	m_Header.Checksum = ComputeChecksum(DataFileName);
	m_Header.Format = Format;
	m_Header.MinX = 9999999999.0;
	m_Header.MinY = 9999999999.0;
	m_Header.MinZ = 9999999999.0;
	m_Header.MaxX = -9999999999.0;
	m_Header.MaxY = -9999999999.0;
	m_Header.MaxZ = -9999999999.0;
	m_Header.GridCellsAcross = Width;
	m_Header.GridCellsUp = Height;
	m_Header.TotalPointsIndexed = 0;

	// open data file
	CLidarData dat(DataFileName);
	LIDARRETURN pt;
	if (dat.IsValid()) {
		// create index file and write header
		m_FileHandle = fopen(fs, "wb+");
		if (m_FileHandle) {
			if (WriteHeader()) {
				if (MinX + MaxX + MinY + MaxY + MinZ + MaxZ == 0.0) {
					// scan data file for min/max values
					while (dat.ReadNextRecord(&pt)) {
						m_Header.MinX = std::min(m_Header.MinX, pt.X);
						m_Header.MinY = std::min(m_Header.MinY, pt.Y);
						m_Header.MinZ = std::min(m_Header.MinZ, (double) pt.Elevation);
						m_Header.MaxX = std::max(m_Header.MaxX, pt.X);
						m_Header.MaxY = std::max(m_Header.MaxY, pt.Y);
						m_Header.MaxZ = std::max(m_Header.MaxZ, (double) pt.Elevation);

						m_Header.TotalPointsIndexed ++;
					}
					dat.Rewind();
				}
				else {
					// use values passed in
					m_Header.MinX = MinX;
					m_Header.MinY = MinY;
					m_Header.MinZ = MinZ;
					m_Header.MaxX = MaxX;
					m_Header.MaxY = MaxY;
					m_Header.MaxZ = MaxZ;


					m_Header.TotalPointsIndexed = NumberOfPoints;
				}

				// add 0.1 to max values to make sure we have a range that works for cell calculation
				// removed addition of 0.1 to header max values 3/21/2006
//				m_Header.MaxX += 0.1;
//				m_Header.MaxY += 0.1;
//				m_Header.MaxZ += 0.1;

				// modified cell size calculation to add 0.1 to max data values 3/21/2006
				double cellw = ((m_Header.MaxX + 0.1) - m_Header.MinX) / (double) m_Header.GridCellsAcross;
				double cellh = ((m_Header.MaxY + 0.1) - m_Header.MinY) / (double) m_Header.GridCellsUp;

				// read data file lines and build index
				int ptnum = 0;
				long offset = dat.GetPosition();
				while (dat.ReadNextRecord(&pt)) {
					ptnum ++;

					m_Point.Offset = offset;
					m_Point.Column = (unsigned char) ((pt.X - m_Header.MinX) / cellw);
					m_Point.Row = (unsigned char) ((pt.Y - m_Header.MinY) / cellh);
//						m_Point.PointNumber = ptnum;

					WritePoint();
					offset = dat.GetPosition();
				}

				// rewrite the header to get min/max values and number of points
				WriteHeader();

				// improve the index
				if (m_Header.TotalPointsIndexed && Optimize) {
					// allocate space for entire index
					INDEXENTRY* list = new INDEXENTRY[m_Header.TotalPointsIndexed];
					if (list) {
						// rewind to start of index entries
						fseek(m_FileHandle, 128, SEEK_SET);

						for (int i = 0; i < m_Header.TotalPointsIndexed; i ++) {
							ReadPoint();
							list[i].Column = m_Point.Column;
							list[i].Row = m_Point.Row;
							list[i].Offset = m_Point.Offset;
						}

						// sort list
						qsort(list, m_Header.TotalPointsIndexed, sizeof(INDEXENTRY), CompareIndexEntries);

						// re-write
						// rewind to start of index entries
						fseek(m_FileHandle, 128, SEEK_SET);

						for (int i = 0; i < m_Header.TotalPointsIndexed; i ++) {
							m_Point.Column = list[i].Column;
							m_Point.Row = list[i].Row;
							m_Point.Offset = list[i].Offset;
							WritePoint();
						}

						// create cell starting point file
						int colrow;
						m_CellStarts = new long[m_Header.GridCellsAcross * m_Header.GridCellsUp];
						if (m_CellStarts) {
							// initialize starting points
							for (int i = 0; i < m_Header.GridCellsAcross * m_Header.GridCellsUp; i ++)
								m_CellStarts[i] = -1;

							for (int i = 0; i < m_Header.TotalPointsIndexed; i ++) {
								colrow = list[i].Column * m_Header.GridCellsUp + list[i].Row;
								if (m_CellStarts[colrow] < 0)
									m_CellStarts[colrow] = i;
							}

							// write file
							boost::filesystem::path ifs(boost::filesystem::complete(fs));
							ifs.replace_extension(".ldi");

							FILE* f = fopen(ifs, "wb");
							if (f) {
								// use same header as index with slightly different signature
								strcpy(m_Header.Signature, "LDAstarts");
								fwrite(&m_Header, sizeof(header), 1, f);

								// seek to end of header area
								fseek(f, 128, SEEK_SET);

								// write starting points
								fwrite(m_CellStarts, sizeof(long), m_Header.GridCellsAcross * m_Header.GridCellsUp, f);

								fclose(f);

								// reset header to correct signature
								strcpy(m_Header.Signature, "LDAindex");
							}

							m_HaveStarts = true;
						}

						delete [] list;
					}
				}
				fclose(m_FileHandle);

				// reopen in read mode and go to start of points
				m_FileHandle = fopen(fs, "rb");
				fseek(m_FileHandle, 128, SEEK_SET);

				m_Valid = true;
			}
		}
		dat.Close();
	}
	else {
		m_Valid = false;
	}
	return(m_Valid);
}

bool CDataIndex::VerifyChecksum(const std::string & DataFileName, int UseOldMethod)
{
	// test checksum to see if data file has changed since index was created
	return(m_Header.Checksum == ComputeChecksum(DataFileName, UseOldMethod));
}

bool CDataIndex::ReadHeader(FILE* handle)
{
	FILE* f = handle;
	if (!handle)
		f = m_FileHandle;

	// read header
	if (f) {
		int cnt = 0;
		rewind(f);
//		cnt += fread(m_Header.Signature, sizeof(char), 12, f);
//		cnt += fread(&m_Header.Version, sizeof(float), 1, f);
//		cnt += fread(&m_Header.Checksum, sizeof(long), 1, f);
//		cnt += fread(&m_Header.Format, sizeof(int), 1, f);
//		cnt += fread(&m_Header.MinX, sizeof(double), 1, f);
//		cnt += fread(&m_Header.MinY, sizeof(double), 1, f);
//		cnt += fread(&m_Header.MinZ, sizeof(double), 1, f);
//		cnt += fread(&m_Header.MaxX, sizeof(double), 1, f);
//		cnt += fread(&m_Header.MaxY, sizeof(double), 1, f);
//		cnt += fread(&m_Header.MaxZ, sizeof(double), 1, f);
//		cnt += fread(&m_Header.GridCellsAcross, sizeof(int), 1, f);
//		cnt += fread(&m_Header.GridCellsUp, sizeof(int), 1, f);
//		cnt += fread(&m_Header.TotalPointsIndexed, sizeof(int), 1, f);
		cnt += fread(&m_Header, sizeof(header), 1, f);

		fseek(f, 128, SEEK_SET);

		if (cnt == 1)			// was 24 with item by item read
			return(true);
	}
	return(false);
}

bool CDataIndex::Open(const std::string & DataFileName, bool UseStarts)
{
	// construct index filename from data filename and read header, then verify checksum
	if (m_Valid)
		Close();

	// create index for the given data file
	m_DataFileName = DataFileName;

	// build index filename
	boost::filesystem::path fs(DataFileName);
	fs.replace_extension(".ldx");

	// build cell starts filename
	boost::filesystem::path ifs(DataFileName);
	ifs.replace_extension(".ldi");

	// open index file and read header
	m_FileHandle = fopen(fs, "rb");
	if (m_FileHandle) {
		if (ReadHeader()) {
			if (VerifyChecksum(DataFileName)) {
				m_Valid = true;
			}
			else {
				fclose(m_FileHandle);
				m_Valid = false;

				// check for old header checksum
				if (VerifyChecksum(DataFileName, true)) {
					// update index and starts file headers
					UpdateChecksum(DataFileName);

					// this has to work since we have already opened the file
					m_FileHandle = fopen(fs, "rb");
					if (m_FileHandle) {
						ReadHeader();
						m_Valid = true;
					}
				}
			}
		}
	}

	// see if there is a starts file
	if (m_Valid && UseStarts) {
		FILE* f = fopen(ifs, "rb");
		if (f) {
			if (ReadHeader(f)) {
				if (VerifyChecksum(DataFileName)) {
					m_CellStarts = new long[m_Header.GridCellsAcross * m_Header.GridCellsUp];
					if (m_CellStarts) {
						if (fread(m_CellStarts, sizeof(long), m_Header.GridCellsAcross * m_Header.GridCellsUp, f) == (unsigned int) (m_Header.GridCellsAcross * m_Header.GridCellsUp))
							m_HaveStarts = true;
						else
							delete [] m_CellStarts;
					}
				}
			}
			fclose(f);

			// re-read index file header...starts file header was read into m_Header
			ReadHeader();
		}
		else {
			// could not read starts file...this is an error since UseStarts was true
			if (m_FileHandle)
				fclose(m_FileHandle);

			m_Valid = false;
			m_FileHandle = NULL;
		}
	}

	return(m_Valid);
}

long CDataIndex::ComputeChecksum(const std::string & DataFileName, int UseOldMethod)
{
	long chksum = 0;
#ifdef WIN32
	// using low DWORD components should catch all the details.  Not likely the modification time will keep the 
	// same nanosecond count or that the file size will change by 2Gb chunks
	WIN32_FILE_ATTRIBUTE_DATA attribdata;
	SYSTEMTIME  systime;
	if (GetFileAttributesEx(DataFileName.c_str(), GetFileExInfoStandard, &attribdata)) {
		if (UseOldMethod) {
			// old method has problems with FAT or FAT32 drives and daylight savings time change
			// this was the method used up to 5/1/2006
			chksum = attribdata.nFileSizeLow + attribdata.ftLastWriteTime.dwLowDateTime;
		}
		else {
			FileTimeToSystemTime(&attribdata.ftLastWriteTime, &systime);
			// new checksum calculation 5/1/2006...uses minutes and seconds of last write time and low 4 bytes of file size
			// this should eliminate all problems due to time zone and DST changes on all drive types
			// FAT based drives record file times in local time and NTFS records times in UTC
			chksum = attribdata.nFileSizeLow + (unsigned long) systime.wMinute + (unsigned long) systime.wSecond + (unsigned long) systime.wMilliseconds;
		}
	}

	// using _stat() function results in different modification date/time depending on time zone and daylight savings time
	// this was the original method used in early FUSION version
//	struct _stat finfo;
//	if (_stat(DataFileName, &finfo) == 0) {
//		chksum = finfo.st_mtime + finfo.st_size;
//	}
	else
#endif
		chksum = -1;

	return(chksum);
}

bool CDataIndex::IsValid()
{
	return(m_Valid);
}

void CDataIndex::Close()
{
	if (m_Valid)
		fclose(m_FileHandle);

	if (m_HaveStarts) {
		delete [] m_CellStarts;
		m_HaveStarts = false;
	}

	m_Valid = false;
	m_FileHandle = NULL;
}

bool CDataIndex::WriteHeader(FILE* handle)
{
	FILE* f = handle;
	if (!handle)
		f = m_FileHandle;

	if (f) {
		int cnt = 0;
		rewind(f);
//		cnt += fwrite(m_Header.Signature, sizeof(char), 12, f);
//		cnt += fwrite(&m_Header.Version, sizeof(float), 1, f);
//		cnt += fwrite(&m_Header.Checksum, sizeof(long), 1, f);
//		cnt += fwrite(&m_Header.Format, sizeof(int), 1, f);
//		cnt += fwrite(&m_Header.MinX, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.MinY, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.MinZ, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.MaxX, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.MaxY, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.MaxZ, sizeof(double), 1, f);
//		cnt += fwrite(&m_Header.GridCellsAcross, sizeof(int), 1, f);
//		cnt += fwrite(&m_Header.GridCellsUp, sizeof(int), 1, f);
//		cnt += fwrite(&m_Header.TotalPointsIndexed, sizeof(int), 1, f);
		cnt += fwrite(&m_Header, sizeof(header), 1, f);

		fseek(f, 128, SEEK_SET);

		if (cnt == 1)			// was 24 with item by item read
			return(true);
	}
	return(false);
}

bool CDataIndex::WritePoint(FILE* handle)
{
	FILE* f = handle;
	if (!handle)
		f = m_FileHandle;

	if (f) {
		int cnt = 0;
//		cnt += fwrite(&m_Point.Column, sizeof(unsigned char), 1, f);
//		cnt += fwrite(&m_Point.Row, sizeof(unsigned char), 1, f);
//		cnt += fwrite(&m_Point.Offset, sizeof(long), 1, f);
		cnt += fwrite(&m_Point, sizeof(pt), 1, m_FileHandle);

		if (cnt == 1)
			return(true);
	}
	return(false);
}

bool CDataIndex::ReadPoint(FILE* handle)
{
	FILE* f = handle;
	if (!handle)
		f = m_FileHandle;

	if (f) {
		int cnt = 0;
//		cnt += fread(&m_Point.Column, sizeof(unsigned char), 1, f);
//		cnt += fread(&m_Point.Row, sizeof(unsigned char), 1, f);
//		cnt += fread(&m_Point.Offset, sizeof(long), 1, f);
		cnt += fread(&m_Point, sizeof(pt), 1, m_FileHandle);

		if (cnt == 1)
			return(true);
	}
	return(false);
}

long CDataIndex::JumpToNextPointInRange(double minx, double miny, double maxx, double maxy, FILE* datafile)
{
	int i, j;
	bool GoodJump;

	if (!m_HaveDataRange) {
		m_Range_MinX = minx;
		m_Range_MinY = miny;
		m_Range_MaxX = maxx;
		m_Range_MaxY = maxy;

		// do test to see if area of interest is within data set extents
		if (maxx < m_Header.MinX || minx > m_Header.MaxX || maxy < m_Header.MinY || miny > m_Header.MaxY)
			return(-1);

		// compute range of valid cells
		// modified cell size calculation to add 0.1 to max data values 3/21/2006
		double cellw = ((m_Header.MaxX + 0.1) - m_Header.MinX) / (double) m_Header.GridCellsAcross;
		double cellh = ((m_Header.MaxY + 0.1) - m_Header.MinY) / (double) m_Header.GridCellsUp;
		m_Range_MinCellColumn = (unsigned char) (std::max<float>(0, (minx - m_Header.MinX) / cellw));
		m_Range_MaxCellColumn = (unsigned char) (std::min((double) m_Header.GridCellsAcross - 1.0, (maxx - m_Header.MinX) / cellw));
		m_Range_MinCellRow = (unsigned char) (std::max<float>(0, (miny - m_Header.MinY) / cellh));
		m_Range_MaxCellRow = (unsigned char) (std::min((double) m_Header.GridCellsUp - 1.0, (maxy - m_Header.MinY) / cellh));

		m_HaveDataRange = true;

		// if using starts...jump to first point in LL cell
		GoodJump = false;
		if (m_HaveStarts) {
			for (i = m_Range_MinCellColumn; i <= m_Range_MaxCellColumn; i ++) {
				for (j = m_Range_MinCellRow; j <= m_Range_MaxCellRow; j ++) {
					if (m_CellStarts[i * m_Header.GridCellsUp + j] >= 0) {
						fseek(m_FileHandle, 128 + m_CellStarts[i * m_Header.GridCellsUp + j] * sizeof(pt), SEEK_SET);
						GoodJump = true;
						break;
					}
				}
				if (GoodJump)
					break;
			}
			if (!GoodJump)
				return(-1);
		}
	}

	while (ReadPoint()) {
		if (m_Point.Column >= m_Range_MinCellColumn && m_Point.Column <= m_Range_MaxCellColumn) {
			if (m_Point.Row >= m_Range_MinCellRow && m_Point.Row <= m_Range_MaxCellRow) {
				if (datafile)
					fseek(datafile, m_Point.Offset, SEEK_SET);
				return(m_Point.Offset);
			}
			else if (m_HaveStarts) {
				// if using starts, and point is in rows above search area, try to jump to point in next column and bottom cell of search area
				if (m_Point.Row > m_Range_MaxCellRow) {
					for (i = m_Point.Column + 1; i <= m_Range_MaxCellColumn; i ++) {
						for (j = m_Range_MinCellRow; j <= m_Range_MaxCellRow; j ++) {
							if (m_CellStarts[i * m_Header.GridCellsUp + j] >= 0) {
								fseek(m_FileHandle, 128 + m_CellStarts[i * m_Header.GridCellsUp + j] * sizeof(pt), SEEK_SET);
								GoodJump = true;
								break;
							}
						}
						if (GoodJump)
							break;
					}
					if (!GoodJump)
						break;
				}
			}
		}
		else if (m_HaveStarts) {
			// since index is sorted on columns then rows, we must be outside the search area
			break;
		}
	}

	m_HaveDataRange = false;
	return(-1);
}

long CDataIndex::GetCellStartValue(int Row, int Col)
{
	if (m_Valid) {
		if (Row < m_Header.GridCellsUp && Col < m_Header.GridCellsAcross)
			return(m_CellStarts[Col * m_Header.GridCellsUp + Row]);
	}
	return(-1);
}

void CDataIndex::ResetRange()
{
	m_HaveDataRange = false;
}

bool CDataIndex::UpdateChecksum(const std::string & DataFileName)
{
	bool retcode = false;

	// build index filename
	boost::filesystem::path fs(DataFileName);
	fs.replace_extension(".ldx");

	// build cell starts filename
	boost::filesystem::path ifs(DataFileName);
	ifs.replace_extension(".ldi");

	// update the checksum for the index file
	FILE* f = fopen(fs, "rb+");
	if (f) {
		if (ReadHeader(f)) {
			// make sure we have a valid data index file
			if (strcmp(m_Header.Signature, "LDAindex") == 0) {
				m_Header.Checksum = ComputeChecksum(DataFileName);
				m_Header.Version = 1.1f;

				WriteHeader(f);
				fclose(f);

				retcode = true;
			}
		}
	}

	// update the checksum for the starts file
	if (retcode) {
		f = fopen(ifs, "rb+");
		if (f) {
			if (ReadHeader(f)) {
				if (strcmp(m_Header.Signature, "LDAstarts") == 0) {
					m_Header.Checksum = ComputeChecksum(DataFileName);
					m_Header.Version = 1.1f;

					WriteHeader(f);
					fclose(f);
				}
			}
		}
	}

	return(retcode);
}
