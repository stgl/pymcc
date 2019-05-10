// LidarData.cpp: implementation of the CLidarData class.
//
//////////////////////////////////////////////////////////////////////
//
// 11/9/2006
// added code to preserve LAS return info not included in LDA return record when converting
// a CLIDARData file to LAS (only if original file was LAS format)...still not perfect but a start at
// switching over to use LAS files for all FUSION tasks
//
#include <algorithm>
#include "LidarData.h"
#include "DataIndex.h"
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLidarData::CLidarData()
{
	m_FileFormat = INVALID;
	m_Valid = false;
	m_HaveLocalBuffer = false;
	m_BinaryFileBuffer = NULL;
}

CLidarData::CLidarData(const std::string & szFileName)
{
	m_FileFormat = INVALID;
	m_Valid = false;
	m_HaveLocalBuffer = false;
	m_BinaryFileBuffer = NULL;

	Open(szFileName);
}

CLidarData::~CLidarData()
{
	Close();
}

bool CLidarData::IsValid()
{
	return(m_Valid);
}

bool CLidarData::Open(const std::string & szFileName)
{
	char signature[9];

	if (m_Valid)
		Close();

	// determine file format by looking at first 8 bytes...ASCII or binary
	FILE* f = fopen(szFileName.c_str(), "rb");

	if (f) {
//		m_BinaryFileBuffer = new char[4096];
//		if (m_BinaryFileBuffer) {
//			if (setvbuf(f, m_BinaryFileBuffer, _IOFBF, 4096) == 0)
//				m_HaveLocalBuffer = true;
//		}

		fread(signature, sizeof(char), 8, f);
		signature[8] = '\0';

		if (strcmp(signature, "LIDARDAT") == 0) {
			// if ASCII use CDataFile member to access
			m_FileFormat = ASCIIDATA;

			// read version
			if (fscanf(f, "%f", &m_Version) != 1)
				m_FileFormat = INVALID;
		}
		else if (strcmp(signature, "LIDARBIN") == 0) {
			// if binary, use direct access
			m_FileFormat = BINDATA;

			// read version...two parts
			int major, minor;
			fread(&major, sizeof(int), 1, f);
			fread(&minor, sizeof(int), 1, f);
			m_Version = (float) major + (float) minor / 10.0f;
		}
		else {
			// look at first 4 characters for LAS format signature
			signature[4] = '\0';
			if (strcmp(signature, "LASF") == 0) {
				m_FileFormat = LASDATA;
				m_Version = 1.0f;
			}
			else
				m_FileFormat = INVALID;
		}
		
		fclose(f);

		if (m_FileFormat == ASCIIDATA) {
			m_ASCIIFile.Open(szFileName);

			if (m_ASCIIFile.IsValid()) {
				// read first line
				m_ASCIIFile.ReadASCIILine(m_LineBuffer);
			}
			else
				m_FileFormat = INVALID;
		}
		else if (m_FileFormat == BINDATA) {
			m_BinaryFile = fopen(szFileName.c_str(), "rb");
			if (m_BinaryFile)
				fseek(m_BinaryFile, 16, SEEK_SET);
			else
				m_FileFormat = INVALID;
		}
		else if (m_FileFormat == LASDATA) {
			m_LASFile.Open(szFileName);
			if (m_LASFile.IsValid()) {
				m_Version = (float) m_LASFile.Header.VersionMajor + (float) m_LASFile.Header.VersionMinor / 10.0f;
				m_LASFile.JumpToPointRecord(0);		// first point, index == 0
			}
			else
				m_FileFormat = INVALID;
		}

		if (m_FileFormat != INVALID)
			m_Valid = true;
		else
			m_Valid = false;

		// get rid of expanded buffer
//		if (!m_Valid) {
//			if (m_HaveLocalBuffer) {
//				m_HaveLocalBuffer = false;
//				delete [] m_BinaryFileBuffer;
//				m_BinaryFileBuffer = NULL;
//			}
//		}
	}

	return(m_Valid);
}

void CLidarData::Rewind()
{
	if (m_Valid) {
		if (m_FileFormat == ASCIIDATA) {
			if (m_ASCIIFile.IsValid()) {
				// rewind
				m_ASCIIFile.Rewind();

				// read first line
				m_ASCIIFile.ReadASCIILine(m_LineBuffer);
			}
		}
		else if (m_FileFormat == BINDATA) {
			if (m_BinaryFile) 
				fseek(m_BinaryFile, 16, SEEK_SET);
		}
		else if (m_FileFormat == LASDATA) {
			m_LASFile.JumpToPointRecord(0);		// first point, index == 0
		}
	}
}

bool CLidarData::ReadNextRecord(LIDARRETURN *ldat, LAS_RETURNFIELDS* LASdat)
{
	if (m_Valid) {
		if (m_FileFormat == ASCIIDATA) {
			if (m_ASCIIFile.IsValid()) {
				if (m_ASCIIFile.ReadDataLine(m_LineBuffer, SKIPCOMMENTS)) {
					// parse values into structure
					if (m_Version == 1.0f) {
						if (sscanf(m_LineBuffer, "%i %i %lf %lf %f %f %f", &ldat->PulseNumber, &ldat->ReturnNumber, &ldat->X, &ldat->Y, &ldat->Elevation, &ldat->NadirAngle, &ldat->Intensity) == 7)
							return(true);
					}
					else if (m_Version == 0.9f) {
						ldat->ReturnNumber = 0;
						if (sscanf(m_LineBuffer, "%i %lf %lf %f %f %f", &ldat->PulseNumber, &ldat->X, &ldat->Y, &ldat->Elevation, &ldat->NadirAngle, &ldat->Intensity) == 6)
							return(true);
					}
					else 
						return(false);
				}
			}
		}
		else if (m_FileFormat == BINDATA) {
			if (m_BinaryFile) {
				if (fread(ldat, sizeof(LIDARRETURN), 1, m_BinaryFile) == 1) {
					return(true);
				}
			}
		}
		else if (m_FileFormat == LASDATA) {
			if (m_LASFile.IsValid()) {
				if (m_LASFile.ReadNextPoint()) {
					ldat->X = m_LASFile.PointRecord.X;
					ldat->Y = m_LASFile.PointRecord.Y;
					ldat->Elevation = (float) m_LASFile.PointRecord.Z;
//					ldat->PulseNumber = (int) (fmod(m_LASFile.PointRecord.GPSTime, 1.0) * 10000.0);
					ldat->PulseNumber = (int) (m_LASFile.PointRecord.GPSTime);
					ldat->ReturnNumber = (int) m_LASFile.PointRecord.ReturnNumber;
					ldat->Intensity = (float) m_LASFile.PointRecord.Intensity;
					ldat->NadirAngle = (float) m_LASFile.PointRecord.ScanAngleRank;

					if (LASdat) {
						LASdat->NumberOfReturns = m_LASFile.PointRecord.NumberOfReturns;
						LASdat->ScanDirectionFlag = m_LASFile.PointRecord.ScanDirectionFlag;
						LASdat->EdgeOfFlightLineFlag = m_LASFile.PointRecord.EdgeOfFlightLineFlag;
						LASdat->V11Classification = m_LASFile.PointRecord.V11Classification;
						LASdat->V11Synthetic = m_LASFile.PointRecord.V11Synthetic;
						LASdat->V11KeyPoint = m_LASFile.PointRecord.V11KeyPoint;
						LASdat->V11Withheld = m_LASFile.PointRecord.V11Withheld;
						LASdat->UserData = m_LASFile.PointRecord.FileMarker;
						LASdat->PointSourceID = m_LASFile.PointRecord.UserBitField;
						LASdat->GPSTime = m_LASFile.PointRecord.GPSTime;
					}

					return(true);
				}
			}
		}
	}

	return(false);
}

void CLidarData::Close()
{
	if (m_Valid) {
		if (m_FileFormat == ASCIIDATA) {
			if (m_ASCIIFile.IsValid()) {
				// close data file
				m_ASCIIFile.Close();
			}
		}
		else if (m_FileFormat == BINDATA) {
			if (m_BinaryFile) 
				fclose(m_BinaryFile);
		}
		else if (m_FileFormat == LASDATA) {
			m_LASFile.Close();
		}

		// delete local buffer
		if (m_HaveLocalBuffer) {
			m_HaveLocalBuffer = false;
			delete [] m_BinaryFileBuffer;
			m_BinaryFileBuffer = NULL;
		}
	}
	m_Valid = false;
}

bool CLidarData::SetPosition(long Offset, int FromWhere)
{
	if (m_Valid) {
		if (m_FileFormat == ASCIIDATA)
			return(m_ASCIIFile.SetPosition(Offset, FromWhere));
		else if (m_FileFormat == BINDATA)
			return(fseek(m_BinaryFile, Offset, FromWhere) == 0);
		else if (m_FileFormat == LASDATA)
			return(fseek(m_LASFile.m_FileHandle, Offset, FromWhere) == 0);
	}
	return(false);
}

long CLidarData::GetPosition()
{
	if (m_Valid) {
		if (m_FileFormat == ASCIIDATA)
			return(m_ASCIIFile.GetPosition());
		else if (m_FileFormat == BINDATA)
			return(ftell(m_BinaryFile));
		else if (m_FileFormat == LASDATA)
			return(ftell(m_LASFile.m_FileHandle));
	}
	return(-1);
}

bool CLidarData::ConvertToBinary(const std::string & OutputFileName, bool CreateIndex, bool DecimateByPulse)
{
	LIDARRETURN pt;
	char *signature = {"LIDARBIN"};
	int major = 1;
	int minor = 1;

	double minx = 999999999.0;
	double miny = 999999999.0;
	double minz = 999999999.0;
	double maxx = -999999999.0;
	double maxy = -999999999.0;
	double maxz = -999999999.0;

	int ptcnt = 0;

	if (!m_Valid)
		return(false);

	if (m_FileFormat == BINDATA && !CreateIndex)
		return(true);

	// variables for pulse decimation
	int PulseCount = 0;
	double LastGPSTime = 0.0;
	bool ScanAngleIncreasing = false;
	int LastScanAngle = -1000;

	FILE* f = fopen(OutputFileName.c_str(), "wb");
	if (f) {
		Rewind();

		// Write new header
		fwrite(signature, sizeof(char), 8, f);
		fwrite(&major, sizeof(int), 1, f);
		fwrite(&minor, sizeof(int), 1, f);

		while (ReadNextRecord(&pt)) {
			if (m_FileFormat == LASDATA && DecimateByPulse) {
				if (m_LASFile.PointRecord.GPSTime != LastGPSTime) {
					PulseCount ++;
					LastGPSTime = m_LASFile.PointRecord.GPSTime;
				}

				// look at scan agle geometry
				if (ScanAngleIncreasing && m_LASFile.PointRecord.ScanAngleRank < LastScanAngle)
					ScanAngleIncreasing = false;
				else if (m_LASFile.PointRecord.ScanAngleRank > LastScanAngle)
					ScanAngleIncreasing = true;

				LastScanAngle = m_LASFile.PointRecord.ScanAngleRank;

				if (PulseCount % 2 != 0)
					continue;

				if (!ScanAngleIncreasing)
					continue;

// temp
//				if (PulseCount > 100000)
//					break;
			}

			fwrite(&pt, sizeof(LIDARRETURN), 1, f);

			if (CreateIndex) {
				minx = std::min(minx, pt.X);
				miny = std::min(miny, pt.Y);
				minz = std::min(minz, (double) pt.Elevation);
				maxx = std::max(maxx, pt.X);
				maxy = std::max(maxy, pt.Y);
				maxz = std::max(maxz, (double) pt.Elevation);
			}
			ptcnt ++;
		}
		fclose(f);

		if (CreateIndex) {
			CDataIndex index;
			index.CreateIndex(OutputFileName, 10, 256, 256, ptcnt, minx, maxx, miny, maxy, minz, maxz, true);
		}
		return(true);
	}

	return(false);
}

bool CLidarData::ConvertToASCII(const std::string & OutputFileName, int Format, int StartRecord, int NumberOfRecords)
{
	LIDARRETURN pt;
	LAS_RETURNFIELDS LASdat;

	if (!m_Valid)
		return(false);

	FILE* f = fopen(OutputFileName.c_str(), "wt");
	if (f) {
		Rewind();

		// Write header
		if (Format == 2)
			fprintf(f, ";PT#, X, Y, Elevation\n");
		else if (Format == 1)
			fprintf(f, "\"X\",\"Y\",\"Elevation\",\"Intensity\"\n");
		else if (Format == 0)		// default format
			fprintf(f, "\"X\",\"Y\",\"Elevation\",\"Intensity\",\"PulseNumber\",\"ReturnNumber\",\"NadirAngle\"\n");
		else if (Format == 3)		// point number plus default
			fprintf(f, "\"PtNumber\",\"X\",\"Y\",\"Elevation\",\"Intensity\",\"PulseNumber\",\"ReturnNumber\",\"NadirAngle\"\n");
		else if (Format == 4 && m_FileFormat == LASDATA)			// LAS everything
			fprintf(f, "\"Point number\",\"GPS time\",\"X\",\"Y\",\"Elevation\",\"Intensity\",\"Return number\",\"Number of returns\",\"Scan direction flag\",\"Flightline edge\",\"Classification\",\"V1.1 Synthetic pt\",\"V1.1 Key-point\",\"V1.1 Withheld\",\"Scan angle rank\",\"User data\",\"Point source ID\"\n");
		else if (Format == 5)		// all LDA fields...tab delimted
			fprintf(f, "\"X\"\t\"Y\"\t\"Elevation\"\t\"Intensity\"\t\"PulseNumber\"\t\"ReturnNumber\"\t\"NadirAngle\"\n");

		int cnt = 0;
//		int specialcnt = 0;
		while (ReadNextRecord(&pt, &LASdat)) {
			if (cnt >= StartRecord) {

/*				// code for testing classification codes...seems that class 1-4 are return numbers
				if (Format == 4 && m_FileFormat == LASDATA) {
					if (LASdat.Classification > 3) {
						fprintf(f, "%i,%lf,%lf,%lf,%f,%f,%i,%i,%i,%i,%i,%i,%i,%i,%f,%i,%i\n", cnt + 1, m_LASFile.PointRecord.GPSTime, pt.X, pt.Y, pt.Elevation, pt.Intensity, pt.ReturnNumber, LASdat.NumberOfReturns, LASdat.ScanDirectionFlag, LASdat.EdgeOfFlightLineFlag, LASdat.V11Classification, LASdat.V11Synthetic, LASdat.V11KeyPoint, LASdat.V11Withheld, pt.NadirAngle, LASdat.UserData, LASdat.PointSourceID);
						specialcnt ++;
					}
				}
*/

				// write point
				if (Format == 2)
					fprintf(f, "%i,%lf,%lf,%f\n", cnt + 1, pt.X, pt.Y, pt.Elevation);
				else if (Format == 1)
					fprintf(f, "%lf,%lf,%f,%f\n", pt.X, pt.Y, pt.Elevation, pt.Intensity);
				else if (Format == 0)		// default format
					fprintf(f, "%lf,%lf,%f,%f,%i,%i,%f\n", pt.X, pt.Y, pt.Elevation, pt.Intensity, pt.PulseNumber, pt.ReturnNumber, pt.NadirAngle);
				else if (Format == 3)		// point number plus default format
					fprintf(f, "%i,%lf,%lf,%f,%f,%i,%i,%f\n", cnt + 1, pt.X, pt.Y, pt.Elevation, pt.Intensity, pt.PulseNumber, pt.ReturnNumber, pt.NadirAngle);
				else if (Format == 4 && m_FileFormat == LASDATA)		// LAS dump
					fprintf(f, "%i,%lf,%lf,%lf,%f,%f,%i,%i,%i,%i,%i,%i,%i,%i,%f,%i,%i\n", cnt + 1, m_LASFile.PointRecord.GPSTime, pt.X, pt.Y, pt.Elevation, pt.Intensity, pt.ReturnNumber, LASdat.NumberOfReturns, LASdat.ScanDirectionFlag, LASdat.EdgeOfFlightLineFlag, LASdat.V11Classification, LASdat.V11Synthetic, LASdat.V11KeyPoint, LASdat.V11Withheld, pt.NadirAngle, LASdat.UserData, LASdat.PointSourceID);
				else if (Format == 5)		// all LDA fields...tab delimited
					fprintf(f, "%lf\t%lf\t%f\t%f\t%i\t%i\t%f\n", pt.X, pt.Y, pt.Elevation, pt.Intensity, pt.PulseNumber, pt.ReturnNumber, pt.NadirAngle);
			}

			cnt ++;

			if (NumberOfRecords > 0 && cnt >= (StartRecord + NumberOfRecords))
				break;
		}
		fclose(f);

		return(true);
	}

	return(false);
}

bool CLidarData::ConvertToLAS(const std::string & OutputFileName, bool CreateIndex, bool PreserveLASInfo)
{
	LIDARRETURN pt;
	CLASPublicHeaderBlock LASHeader;
	CLASPointDataRecord LASPt;
	unsigned char startofdata[3] = {0xDD,0xCC};

	LASHeader.MinX = 999999999.0;
	LASHeader.MinY = 999999999.0;
	LASHeader.MinZ = 999999999.0;
	LASHeader.MaxX = -999999999.0;
	LASHeader.MaxY = -999999999.0;
	LASHeader.MaxZ = -999999999.0;
	LASHeader.NumberOfPointRecords = 0;
	for (int i = 0; i < 5; i ++)
		LASHeader.NumberOfPointsByReturn[i] = 0;

	if (!m_Valid)
		return(false);

	// count returns and get min/max XYZ
	Rewind();
	while (ReadNextRecord(&pt)) {
		LASHeader.MinX = std::min(LASHeader.MinX, pt.X);
		LASHeader.MinY = std::min(LASHeader.MinY, pt.Y);
		LASHeader.MinZ = std::min(LASHeader.MinZ, (double) pt.Elevation);
		LASHeader.MaxX = std::max(LASHeader.MaxX, pt.X);
		LASHeader.MaxY = std::max(LASHeader.MaxY, pt.Y);
		LASHeader.MaxZ = std::max(LASHeader.MaxZ, (double) pt.Elevation);

		if (pt.ReturnNumber)
			LASHeader.NumberOfPointsByReturn[pt.ReturnNumber - 1] ++;

		LASHeader.NumberOfPointRecords ++;
	}

	if (!LASHeader.NumberOfPointRecords)
		return(false);

	// set up scaling
	LASHeader.XScaleFactor = 1.0 / 1000.0;
	LASHeader.YScaleFactor = 1.0 / 1000.0;
	LASHeader.ZScaleFactor = 1.0 / 1000.0;
	LASHeader.XOffset = LASHeader.MinX;
	LASHeader.YOffset = LASHeader.MinY;
	LASHeader.ZOffset = LASHeader.MinZ;

	// set generating software
	strcpy(LASHeader.GeneratingSoftware, "USDA-FS FUSION/LDV");

	FILE* f = fopen(OutputFileName.c_str(), "wb");
	if (f) {
		Rewind();

		// Write header
		LASHeader.Write(f);

		// offset to start of data
		fseek(f, LASHeader.OffsetToData - 2, SEEK_SET);

		// write start of data
		fwrite(startofdata, sizeof(unsigned char), 2, f);

		while (ReadNextRecord(&pt)) {
			LASPt.RawX = (long) ((pt.X - LASHeader.XOffset) / LASHeader.XScaleFactor);
			LASPt.RawY = (long) ((pt.Y - LASHeader.YOffset) / LASHeader.YScaleFactor);
			LASPt.RawZ = (long) (((double) pt.Elevation - LASHeader.ZOffset) / LASHeader.ZScaleFactor);

			LASPt.Intensity = (unsigned short) pt.Intensity;
			LASPt.ReturnNumber = pt.ReturnNumber;
			LASPt.ScanAngleRank = (char) pt.NadirAngle;

			// if reading LAS files, maintain the returns info specific to LAS
			if (m_FileFormat == LASDATA && PreserveLASInfo) {
				LASPt.NumberOfReturns = m_LASFile.PointRecord.NumberOfReturns;
				LASPt.ScanDirectionFlag = m_LASFile.PointRecord.ScanDirectionFlag;
				LASPt.EdgeOfFlightLineFlag = m_LASFile.PointRecord.EdgeOfFlightLineFlag;
				LASPt.Classification = m_LASFile.PointRecord.Classification;
				LASPt.FileMarker = m_LASFile.PointRecord.FileMarker;
			}

			// write point
			LASPt.Write(f);
		}
		fclose(f);

		if (CreateIndex) {
			CDataIndex index;
			index.CreateIndex(OutputFileName, 10, 256, 256, LASHeader.NumberOfPointRecords, LASHeader.MinX, LASHeader.MaxX, LASHeader.MinY, LASHeader.MaxY, LASHeader.MinZ, LASHeader.MaxZ, true);
		}

		return(true);
	}

	return(false);
}

int CLidarData::GetFileFormat()
{
	if (m_Valid)
		return(m_FileFormat);

	return(-1);
}
