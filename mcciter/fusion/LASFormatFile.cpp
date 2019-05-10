// LASFormatFile.cpp: implementation of the CLASFormatFile class.
//
//////////////////////////////////////////////////////////////////////

#include "LASFormatFile.h"
#include <string.h>
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLASFormatFile::CLASFormatFile()
{
	m_FileHandle = NULL;
	m_ValidHeader = false;
	m_CurrentPointIndex = -1;
}

CLASFormatFile::~CLASFormatFile()
{
	Close();
}

// header block
CLASPublicHeaderBlock::CLASPublicHeaderBlock()
{
	// initialize header
	strcpy(FileSignature, "LASF");
	Reserved.Reserved = 0;
	GUIDData1 = 0;
	GUIDData2 = 0;
	GUIDData3 = 0;
	memset(GUIDData4, 0, 9);
	VersionMajor = 1;
	VersionMinor = 0;
	memset(SystemIdentifier, 0, 33);
	memset(GeneratingSoftware, 0, 33);
	FlightDateJulian = 0;
	Year = 0;
	HeaderSize = HEADERSIZE;
	OffsetToData = 1026;		// leave some space for variable length records 1024 plus start of data signature
	NumberOfVariableLengthRecords = 0;
	PointDataFormatID = 0;
	PointDataRecordLength = 20;
	NumberOfPointRecords = 0;
	NumberOfPointsByReturn[0] = 0;
	NumberOfPointsByReturn[1] = 0;
	NumberOfPointsByReturn[2] = 0;
	NumberOfPointsByReturn[3] = 0;
	NumberOfPointsByReturn[4] = 0;
	XScaleFactor = YScaleFactor = ZScaleFactor = 1.0;
	XOffset = YOffset = ZOffset = 0.0;
	MinX = MinY = MinZ = MaxX = MaxY = MaxZ = 0.0;
}

CLASPublicHeaderBlock::~CLASPublicHeaderBlock()
{
}

// variable length record
CLASVariableLengthRecord::CLASVariableLengthRecord()
{
}

CLASVariableLengthRecord::~CLASVariableLengthRecord()
{
}

// point class...format 0
CLASPointDataRecord::CLASPointDataRecord()
{
	X = Y = Z = 0;
	Intensity = 0;
	ReturnNumber = 0;
	NumberOfReturns = 0;
	ScanDirectionFlag = 0;
	EdgeOfFlightLineFlag = 0;
	Classification = 0;
	ScanAngleRank = 0;
	FileMarker = 0;
	UserBitField = 0;
}

CLASPointDataRecord::~CLASPointDataRecord()
{

}

void CLASPointDataRecord::UnpackReturnBitInfo(bool FixFormatError)
{
	if (FixFormatError) {
		ReturnNumber = (ReturnBitInfo & 0xE0) >> 5;
		NumberOfReturns = (ReturnBitInfo & 0x1C) >> 2;
		ScanDirectionFlag = 0;
		EdgeOfFlightLineFlag = 0;
	}
	else {
		ReturnNumber = ReturnBitInfo & 0x07;
		NumberOfReturns = (ReturnBitInfo & 0x38) >> 3;
		ScanDirectionFlag = (ReturnBitInfo & 0x40) >> 6;
		EdgeOfFlightLineFlag = (ReturnBitInfo & 0x80) >> 7;
	}
}

void CLASPointDataRecord::PackReturnBitInfo()
{
	ReturnBitInfo = 0;
	ReturnBitInfo |= (ReturnNumber & 0x07);
	ReturnBitInfo |= (NumberOfReturns & 0x07) << 3;
	ReturnBitInfo |= (ScanDirectionFlag & 0x01) << 6;
	ReturnBitInfo |= (EdgeOfFlightLineFlag & 0x01) << 7;
}

// point class...format 1
CLASPointDataRecordFormat1::CLASPointDataRecordFormat1() : CLASPointDataRecord()
{
	GPSTime = 0.0;
}

CLASPointDataRecordFormat1::~CLASPointDataRecordFormat1()
{

}

// classification info
CLASClassificationLookupTable::CLASClassificationLookupTable()
{
}

CLASClassificationLookupTable::~CLASClassificationLookupTable()
{
}

// flight line info
CLASFlightLineHeaderLookupTable::CLASFlightLineHeaderLookupTable()
{
}

CLASFlightLineHeaderLookupTable::~CLASFlightLineHeaderLookupTable()
{
}

// histogram
CLASHistogram::CLASHistogram()
{
}

CLASHistogram::~CLASHistogram()
{
}

// text description
CLASTextAreaDescription::CLASTextAreaDescription()
{
}

CLASTextAreaDescription::~CLASTextAreaDescription()
{
}

bool CLASFormatFile::VerifyFormat(const std::string & FileName)
{
	bool retcode = false;
	char sig[5];

	// open file and read first 4 bytes...look for signature "LASF"
	FILE* f = fopen(FileName.c_str(), "rb");
	if (f) {
		if (fread(sig, sizeof(char), 4, f) == 4) {
			sig[4] = '\0';
			if (strcmp(sig, "LASF") == 0)
				retcode = true;
		}
		fclose(f);
	}
	return(retcode);
}

bool CLASFormatFile::Open(const std::string & FileName)
{
	bool retcode = false;

	if (VerifyFormat(FileName)) {
		// open file
		m_FileHandle = fopen(FileName.c_str(), "rb");
		if (m_FileHandle) {
			retcode = Header.Read(m_FileHandle);
		}
	}
	m_ValidHeader = retcode;
	return(retcode);
}

bool CLASPublicHeaderBlock::Read(FILE *FileHandle)
{
	bool retcode = false;
	if (FileHandle) {
		// read all fields from header...may not always work to read as structure since byte packing differs depending on compiler options
		int cnt = 0;
		cnt += fread(FileSignature, sizeof(char), 4, FileHandle);
		cnt += fread(&Reserved, sizeof(ulong), 1, FileHandle);
		cnt += fread(&GUIDData1, sizeof(ulong), 1, FileHandle);
		cnt += fread(&GUIDData2, sizeof(ushort), 1, FileHandle);
		cnt += fread(&GUIDData3, sizeof(ushort), 1, FileHandle);
		cnt += fread(GUIDData4, sizeof(char), 8, FileHandle);
		cnt += fread(&VersionMajor, sizeof(uchar), 1, FileHandle);
		cnt += fread(&VersionMinor, sizeof(uchar), 1, FileHandle);
		cnt += fread(SystemIdentifier, sizeof(char), 32, FileHandle);
		cnt += fread(GeneratingSoftware, sizeof(char), 32, FileHandle);
		cnt += fread(&FlightDateJulian, sizeof(ushort), 1, FileHandle);
		cnt += fread(&Year, sizeof(ushort), 1, FileHandle);
		cnt += fread(&HeaderSize, sizeof(ushort), 1, FileHandle);
		cnt += fread(&OffsetToData, sizeof(ulong), 1, FileHandle);
		cnt += fread(&NumberOfVariableLengthRecords, sizeof(ulong), 1, FileHandle);
		cnt += fread(&PointDataFormatID, sizeof(uchar), 1, FileHandle);
		cnt += fread(&PointDataRecordLength, sizeof(ushort), 1, FileHandle);
		cnt += fread(&NumberOfPointRecords, sizeof(ulong), 1, FileHandle);
		cnt += fread(NumberOfPointsByReturn, sizeof(ulong), 5, FileHandle);
		cnt += fread(&XScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fread(&YScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fread(&ZScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fread(&XOffset, sizeof(double), 1, FileHandle);
		cnt += fread(&YOffset, sizeof(double), 1, FileHandle);
		cnt += fread(&ZOffset, sizeof(double), 1, FileHandle);
		cnt += fread(&MaxX, sizeof(double), 1, FileHandle);
		cnt += fread(&MinX, sizeof(double), 1, FileHandle);
		cnt += fread(&MaxY, sizeof(double), 1, FileHandle);
		cnt += fread(&MinY, sizeof(double), 1, FileHandle);
		cnt += fread(&MaxZ, sizeof(double), 1, FileHandle);
		cnt += fread(&MinZ, sizeof(double), 1, FileHandle);

		// terminate strings
		FileSignature[4] = '\0';
		GUIDData4[8] = '\0';
		SystemIdentifier[32] = '\0';
		GeneratingSoftware[32] = '\0';

		// build the version identifier
		char ts[32];
		sprintf(ts, "%i.%i", VersionMajor, VersionMinor);
		Version = (float) atof(ts);

		// cnt should be 107...total number of values read including individual characters in strings
		if (cnt == 107)
			retcode = true;
	}
	return(retcode);
}

bool CLASPublicHeaderBlock::Write(FILE *FileHandle)
{
	bool retcode = false;
	if (FileHandle) {
		int i, start;

		// clean up the strings...need to pad with null chars
		for (i = strlen(FileSignature); i <= 4; i ++)
			FileSignature[i] = '\0';

		for (i = 0; i <= 8; i ++) {
			if (GUIDData4[i] == 0) {
				start = i;
				break;
			}
		}
		for (i = start; i <= 8; i ++)
			GUIDData4[i] = 0;

		for (i = strlen(SystemIdentifier); i <= 32; i ++)
			SystemIdentifier[i] = '\0';
		for (i = strlen(GeneratingSoftware); i <= 32; i ++)
			GeneratingSoftware[i] = '\0';

		// write all fields from header...may not always work to read as structure since byte packing differs depending on compiler options
		int cnt = 0;
		cnt += fwrite(FileSignature, sizeof(char), 4, FileHandle);
		cnt += fwrite(&Reserved, sizeof(ulong), 1, FileHandle);
		cnt += fwrite(&GUIDData1, sizeof(ulong), 1, FileHandle);
		cnt += fwrite(&GUIDData2, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(&GUIDData3, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(GUIDData4, sizeof(char), 8, FileHandle);
		cnt += fwrite(&VersionMajor, sizeof(uchar), 1, FileHandle);
		cnt += fwrite(&VersionMinor, sizeof(uchar), 1, FileHandle);
		cnt += fwrite(SystemIdentifier, sizeof(char), 32, FileHandle);
		cnt += fwrite(GeneratingSoftware, sizeof(char), 32, FileHandle);
		cnt += fwrite(&FlightDateJulian, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(&Year, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(&HeaderSize, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(&OffsetToData, sizeof(ulong), 1, FileHandle);
		cnt += fwrite(&NumberOfVariableLengthRecords, sizeof(ulong), 1, FileHandle);
		cnt += fwrite(&PointDataFormatID, sizeof(uchar), 1, FileHandle);
		cnt += fwrite(&PointDataRecordLength, sizeof(ushort), 1, FileHandle);
		cnt += fwrite(&NumberOfPointRecords, sizeof(ulong), 1, FileHandle);
		cnt += fwrite(NumberOfPointsByReturn, sizeof(ulong), 5, FileHandle);
		cnt += fwrite(&XScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fwrite(&YScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fwrite(&ZScaleFactor, sizeof(double), 1, FileHandle);
		cnt += fwrite(&XOffset, sizeof(double), 1, FileHandle);
		cnt += fwrite(&YOffset, sizeof(double), 1, FileHandle);
		cnt += fwrite(&ZOffset, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MaxX, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MinX, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MaxY, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MinY, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MaxZ, sizeof(double), 1, FileHandle);
		cnt += fwrite(&MinZ, sizeof(double), 1, FileHandle);

		// cnt should be 107...total number of values written including individual characters in strings
		if (cnt == 107)
			retcode = true;
	}
	return(retcode);
}

void CLASFormatFile::JumpToPointRecord(int index)
{
	if (IsValid()) {
		fseek(m_FileHandle, Header.OffsetToData + (index * Header.PointDataRecordLength), SEEK_SET);
		m_CurrentPointIndex = index;
	}
}

bool CLASFormatFile::IsValid()
{
	return(m_ValidHeader && m_FileHandle != NULL);
}

bool CLASFormatFile::ReadNextPoint()
{
	if (!IsValid())
		return(false);

	// if we haven't done offset to start of data, do it now
	if (m_CurrentPointIndex < 0)
		JumpToPointRecord();

	// read a point
	bool retcode = PointRecord.ReadBaseValues(m_FileHandle);
	if (retcode && Header.PointDataFormatID == 1)
		retcode = PointRecord.ReadAdditionalValues(m_FileHandle);

	if (retcode) {
		// do scale and offset correction
		PointRecord.ScaleX(Header.XOffset, Header.XScaleFactor);
		PointRecord.ScaleY(Header.YOffset, Header.YScaleFactor);
		PointRecord.ScaleZ(Header.ZOffset, Header.ZScaleFactor);

		// if reading version 1.1 or newer, extract bits from classification field
		if (Header.Version > 1.0) {
			PointRecord.V11Classification = PointRecord.Classification & 0x1F;	// bits 0-4: classification code for return...corresponds to variable length record in CLASClassificationLookupTable
			PointRecord.V11Synthetic = (uchar) (PointRecord.Classification & 0x20 >> 5);	// bit 5: 0 if point is from scan data, 1 if point was created by another method
			PointRecord.V11KeyPoint = (uchar) (PointRecord.Classification & 0x40 >> 6);	// bit 6: 0 if point is not a model key-point, 1 otherwise...key-points should not be removed by processing operations
			PointRecord.V11Withheld = (uchar) (PointRecord.Classification & 0x80 >> 7);	// bit 7: 0 if point is valid, 1 if point should not be included in processing (synonymous with deleted)
		}
		else {
			PointRecord.V11Classification = PointRecord.Classification;
			PointRecord.V11Synthetic = 0;
			PointRecord.V11KeyPoint = 0;
			PointRecord.V11Withheld = 0;
		}

		m_CurrentPointIndex ++;
	}
	return(retcode);
}

bool CLASPointDataRecord::ReadBaseValues(FILE *FileHandle)
{
	// read format 0 fields
	bool retcode = false;

	int cnt = 0;
	cnt += fread(&RawX, sizeof(long), 1, FileHandle);
	cnt += fread(&RawY, sizeof(long), 1, FileHandle);
	cnt += fread(&RawZ, sizeof(long), 1, FileHandle);
	cnt += fread(&Intensity, sizeof(ushort), 1, FileHandle);
	cnt += fread(&ReturnBitInfo, sizeof(uchar), 1, FileHandle);
	cnt += fread(&Classification, sizeof(uchar), 1, FileHandle);
	cnt += fread(&ScanAngleRank, sizeof(char), 1, FileHandle);
	cnt += fread(&FileMarker, sizeof(uchar), 1, FileHandle);
	cnt += fread(&UserBitField, sizeof(ushort), 1, FileHandle);

	// unpack the return bit info into separate variables
	UnpackReturnBitInfo();

	if (cnt == 9)
		retcode = true;

	return(retcode);
}

bool CLASPointDataRecordFormat1::ReadAdditionalValues(FILE *FileHandle)
{
	// read GPS time for format 1 records
	return(fread(&GPSTime, sizeof(double), 1, FileHandle) == 1);
}

void CLASPointDataRecord::ScaleX(double Offset, double Scale)
{
	X = (RawX * Scale) + Offset;
}

void CLASPointDataRecord::ScaleY(double Offset, double Scale)
{
	Y = (RawY * Scale) + Offset;
}

void CLASPointDataRecord::ScaleZ(double Offset, double Scale)
{
	Z = (RawZ * Scale) + Offset;
}

void CLASFormatFile::Close()
{
	if (IsValid()) {
		if (m_FileHandle)
			fclose(m_FileHandle);
		m_FileHandle = NULL;
		m_ValidHeader = false;
		m_CurrentPointIndex = -1;
	}
}

bool CLASFormatFile::JumpToVariableRecord(int index)
{
	CLASVariableLengthRecord VarRec;
	if (IsValid() && Header.NumberOfVariableLengthRecords && index < (int) Header.NumberOfVariableLengthRecords) {
		fseek(m_FileHandle, Header.HeaderSize, SEEK_SET);

		if (index) {
			for (int i = 0; i < index; i ++) {
				// read the record
				VarRec.Read(m_FileHandle);

				// jump to end of record data
				fseek(m_FileHandle, VarRec.RecordLengthAfterHeader, SEEK_CUR);
			}
		}
		return(true);
	}
	return(false);
}

bool CLASVariableLengthRecord::Read(FILE *FileHandle)
{
	bool retcode = false;

	int cnt = 0;
	cnt += fread(&RecordSignature, sizeof(ushort), 1, FileHandle);
	cnt += fread(UserID, sizeof(char), 16, FileHandle);
	cnt += fread(&RecordID, sizeof(ushort), 1, FileHandle);
	cnt += fread(&RecordLengthAfterHeader, sizeof(ushort), 1, FileHandle);
	cnt += fread(Description, sizeof(char), 32, FileHandle);

	// add terminators
	UserID[16] = '\0';
	Description[32] = '\0';

	if (cnt == 51)
		retcode = true;

	return(retcode);
}

bool CLASPointDataRecord::WriteBaseValues(FILE *FileHandle)
{
	// read format 0 fields
	bool retcode = false;

	// pack the return bit info into separate variables
	PackReturnBitInfo();

	int cnt = 0;
	cnt += fwrite(&RawX, sizeof(long), 1, FileHandle);
	cnt += fwrite(&RawY, sizeof(long), 1, FileHandle);
	cnt += fwrite(&RawZ, sizeof(long), 1, FileHandle);
	cnt += fwrite(&Intensity, sizeof(ushort), 1, FileHandle);
	cnt += fwrite(&ReturnBitInfo, sizeof(uchar), 1, FileHandle);
	cnt += fwrite(&Classification, sizeof(uchar), 1, FileHandle);
	cnt += fwrite(&ScanAngleRank, sizeof(char), 1, FileHandle);
	cnt += fwrite(&FileMarker, sizeof(uchar), 1, FileHandle);
	cnt += fwrite(&UserBitField, sizeof(ushort), 1, FileHandle);

	if (cnt == 9)
		retcode = true;

	return(retcode);
}

bool CLASPointDataRecord::Write(FILE *FileHandle)
{
	return(WriteBaseValues(FileHandle));
}

bool CLASPointDataRecordFormat1::Write(FILE *FileHandle)
{
	if (CLASPointDataRecord::Write(FileHandle))
		return(WriteAdditionalValues(FileHandle));
	else
		return(false);
}

bool CLASPointDataRecordFormat1::WriteAdditionalValues(FILE *FileHandle)
{
	// write GPS time for format 1 records
	return(fwrite(&GPSTime, sizeof(double), 1, FileHandle) == 1);
}
