#include "myPanel.h"

//Bind paint events to this panel
wxBEGIN_EVENT_TABLE(myPanel, wxPanel)
EVT_PAINT(myPanel::paintEvent)
END_EVENT_TABLE()

//Using the base class constructor, create a panel and an associated myBMPFile as a child.
myPanel::myPanel(wxFrame* parent, const wxFileName filepath)
	: wxPanel(parent, wxID_ANY, wxPoint(0, 0), parent->GetSize()), bmpFile(filepath.GetFullPath()), maxHeight(0), maxWidth(0)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	// Read file data to panel members
	if (filepath.GetExt() == "bmp") {
		if (bmpFile.IsOpened()) {
			if (bmpFile.readMetaData()) {
				resizeToImage();
				bmpFile.readImageData();
				image = *bmpFile.getPixelVector();
			}
			imgFile = myIMGFile(bmpFile);
			writeIMG(filepath.GetPathWithSep() + wxString("OUTPUT.IMG"));
			parent->SetClientSize(GetSize().GetWidth() * 2, GetSize().GetHeight());
			IMGPanel = new myPanel(parent, (filepath.GetPathWithSep() + wxString("OUTPUT.IMG")));
			IMGPanel->SetPosition(wxPoint(GetSize().GetWidth()/2, 0));
		}
		else {
			wxMessageBox("Error: Selected file not open for reading.");
		}
	}
	else {
		imgFile = myIMGFile(filepath.GetFullPath());
		loadIMG();
	}

	
}

myPanel::~myPanel()
{

}

// Resizes panel to the size of the BMP file
void myPanel::resizeToImage()
{
	wxSize test = bmpFile.getImageSize();
	SetSize(bmpFile.getImageSize());
	maxWidth = GetSize().x;
	maxHeight = GetSize().y;
}

// 'dc' is a device context (the screen) that is used for drawing on the panel
void myPanel::paintEvent(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);
	PrepareDC(dc);
	drawImage(dc);
}

// Draws pixels from panel's stored pixel vector
void myPanel::drawImage(wxDC& dc)
{
	wxPen pen = dc.GetPen();
	for (int row = 0; row < maxHeight; row++) {
		for (int col = 0; col < maxWidth; col++) {
			pen.SetColour(getPixelColor(row, col));
			dc.SetPen(pen);
			dc.DrawPoint(col, maxHeight - row - 1);
		}
	}
	return;
}

void myPanel::writeIMG(wxString filepath)
{
	imgFile.writeToFile(filepath);
}


// Load BMP pixel data directly to panel vector
void myPanel::loadBMP() {
	// Iterators for bmp file pixels
	vector<vector<wxColor>>::iterator fileRow = bmpFile.getPixelVector()->begin();
	vector<wxColor>::iterator fileCol = fileRow->begin();

	// Iterators for panel image pixels
	vector<vector<wxColor>>::iterator panelRow = image.begin();
	vector<wxColor>::iterator panelCol = panelRow->begin();

	// Copy from bmp file to panel image
	while (fileRow != bmpFile.getPixelVector()->end()) {
		fileCol = fileRow->begin(); //TODO: just added, check for bug?
		while (fileCol != fileRow->end()) {
			*panelCol = *fileCol;
			fileCol++, panelCol++;
		}
		fileRow++, panelRow++;
	}
}

void myPanel::loadIMG() {
	maxSize = imgFile.getSize();
	maxWidth = maxSize.GetWidth();
	maxHeight = maxSize.GetHeight();
	SetSize(maxSize);
	vector<wxColor> width = vector<wxColor>(imgFile.getSize().GetWidth());
	image = vector<vector<wxColor>>(imgFile.getSize().GetHeight(), width);

	// Iterators for panel image pixels
	vector<vector<wxColor>>::iterator panelRow = image.begin();
	vector<wxColor>::iterator panelCol;
	// Iterators for IMG file pixels
	vector<vector<colSpace>> imgVector = imgFile.getPixelVector();
	vector<vector<colSpace>>::const_iterator imgRow = imgVector.begin();
	vector<colSpace>::const_iterator imgCol;
	int index = 0;
	int pixels = maxWidth * maxHeight;
	while (imgRow != imgVector.end()) {
		panelCol = panelRow->begin();
		imgCol = imgRow->begin();
		while(imgCol != imgRow->end()) {
			*panelCol = YCoCgtoRGB(*imgCol);
			panelCol++, imgCol++;
			index++;
		}
		panelRow++, imgRow++;
	}
}

wxColor myPanel::YCoCgtoRGB(colSpace yCoCg)
{
	double temp;
	unsigned char r;
	unsigned char g;
	unsigned char b;

	temp = double(yCoCg.Y) - (yCoCg.Cg >> 1);
	g = yCoCg.Cg + temp;
	b = temp - (yCoCg.Co >> 1);
	r = b + yCoCg.Co;
	return wxColor(r, g, b);
}

// Return pixel vector RGB element at index
wxColor myPanel::getPixelColor(const int row, const int col) const {
	if (row >= maxHeight || col >= maxWidth) {
		return wxColor("Black");
	}
	return image[row][col];
}
