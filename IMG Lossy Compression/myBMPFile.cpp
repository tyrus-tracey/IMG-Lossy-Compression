#include "myBMPFile.h"

myBMPFile::myBMPFile(const wxString filepath)
    : wxFFile(filepath, "rb") {
    fileSize = 0;
    pixelOffset = 0;
    headerSize = 0;
    imageWidth = 0;
    imageHeight = 0;
    numberOfPlanes = 0;
    bitsPerPixel = 0;
    compressionFactor = 0;
}

myBMPFile::~myBMPFile() {
    delete pixelVector;
}

wxSize myBMPFile::getImageSize()
{
    return wxSize(imageWidth, imageHeight);
}


vector<vector<wxColor>>* myBMPFile::getPixelVector() const
{
    return pixelVector;
}

//High level function combining reads of file and info header
bool myBMPFile::readMetaData() {
    if (!readFileHeader() | !readInfoHeader()) {
        return false;
    }
    return true;
}

// Create a vector to store RGB data, using wxColor class
// Calculate byte padding, then begin reading data from BMP file.
void myBMPFile::readImageData()
{
    //Calculate size of byte padding;
    unsigned int bytesPerRow = imageWidth * bitsPerPixel / 8;
    unsigned int bytePadding;
    if (bytesPerRow % 4 == 0) {
        bytePadding = 0;
    }
    else {
        bytePadding = 4 - (bytesPerRow % 4);
    }
    char *paddingBuffer = new char[bytePadding];
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    unsigned int numberOfPixels = imageWidth * imageHeight;
    unsigned int bytesPerPixel = bitsPerPixel / 8;
    vector<wxColor> colVector(imageWidth);
    pixelVector = new vector<vector<wxColor>>(imageHeight, colVector);
    vector<vector<wxColor>>::iterator row = pixelVector->begin();
    vector<wxColor>::iterator col = row->begin();

    while (row != pixelVector->end()) {
        col = row->begin();
        while (col != row->end()) {
            Read(&Blue, 1);
            Read(&Green, 1);
            Read(&Red, 1);
            *col = wxColor(Red, Green, Blue);
            col++;
        }
        Read(paddingBuffer, bytePadding);
        row++;
    }
    delete[] paddingBuffer;
    Close();
}

// Reads initial file header of BMP file. Returns true if successful.
bool myBMPFile::readFileHeader()
{
    char charBuff[3];
    Read(charBuff, 2);
    charBuff[2] = '\0';
    if (strcmp(charBuff, "BM")) {
        wxMessageBox("Error: Selected file is not a BMP image.");
        return false;
    }

    Read(&fileSize, 4);
    Read(charBuff, 2);  // Reserved spaces
    Read(charBuff, 2);  // not needed
    Read(&pixelOffset, 4);

    return true;
}

// Reads info header of BMP file. Returns true if successful.
bool myBMPFile::readInfoHeader()
{
    int junkBuffer;

    Read(&headerSize, 4);
    if (headerSize != 40) {
        wxMessageBox("Error: Header size is not size 40.");
        return false;
    }

    Read(&imageWidth, 4);
    Read(&imageHeight, 4);
    Read(&numberOfPlanes, 2);
    Read(&bitsPerPixel, 2);
    Read(&compressionFactor, 4);
    Read(&junkBuffer, 4); // Compressed image size - not necessary
    Read(&junkBuffer, 4); // X px per meter - not necessary
    Read(&junkBuffer, 4); // Y px per meter - not necessary
    Read(&junkBuffer, 4); // Total colors - not used in 24bit BMP
    Read(&junkBuffer, 4); // Important colors - not used in 24bit BMP

    if (bitsPerPixel != 24) {
        wxMessageBox("Error: Unsupported bit depth.");
        return false;
    }
    if (compressionFactor != 0) {
        wxMessageBox("Error: Compressed images are not supported.");
        return false;
    }
    if (numberOfPlanes != 1) {
        wxMessageBox("Warning: Loading non-standard color planes.");
    }
    return true;
}
