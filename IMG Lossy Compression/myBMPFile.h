#pragma once
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/wfstream.h"
#include <vector>
using namespace std;

class myBMPFile : public wxFFile
{
public:
	myBMPFile(const wxString filepath);
	~myBMPFile();
	wxSize getImageSize();
	vector<wxColor>* getPixelVector() const;
	bool readMetaData();
	void readImageData();
	vector<wxColor>* pixelVector;

private:
	bool readFileHeader();
	bool readInfoHeader();

	unsigned int fileSize;
	unsigned int pixelOffset;
	unsigned int headerSize;
	int imageWidth;
	int imageHeight;
	unsigned int numberOfPlanes;
	unsigned int bitsPerPixel;
	unsigned int compressionFactor;
};

