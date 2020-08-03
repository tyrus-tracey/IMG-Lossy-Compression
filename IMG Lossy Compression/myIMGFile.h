#pragma once
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/wfstream.h"
#include "myBMPFile.h"
#include <vector>

// simple YCoCg struct
struct colSpace {
	colSpace();
	colSpace(wxColor triColor);
	short Y; 
	short Co; 
	short Cg; 
};

class myIMGFile
{
public:
	myIMGFile();
	myIMGFile(const wxString filepath);
	myIMGFile(myBMPFile& bmp);
	void readFromFile(const wxString filepath);
	void writeToFile(const wxString filepath);
	wxSize getSize() const;
	const vector<vector<colSpace>>& getPixelVector();
	vector<vector<colSpace>>::iterator row;
	vector<colSpace>::iterator col;

private:
	void readBMP(myBMPFile& bmp);
	//compression functions
	void quantizePixels();
	void flattenExtremes();
	void convertToStrings();
	void encode();
	//read functions
	void decode();
	void reconstructPixels();

	wxSize imageSize;
	vector<vector<colSpace>> data;
	vector<int16_t> dataY;
	vector<int16_t> dataCo;
	vector<int16_t> dataCg;
	
};

