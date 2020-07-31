#include "myPanel.h"

//Bind paint events to this panel
wxBEGIN_EVENT_TABLE(myPanel, wxPanel)
EVT_PAINT(myPanel::paintEvent)
END_EVENT_TABLE()

//Using the base class constructor, create a panel and an associated myBMPFile as a child.
myPanel::myPanel(wxFrame* parent, const wxString filepath)
	: wxPanel(parent, wxID_ANY, wxPoint(0, 0), parent->GetSize()), bmpFile(filepath)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	// Read file data to panel members
	if (bmpFile.IsOpened()) { 
		if (bmpFile.readMetaData()) {
			resizeToImage();
			bmpFile.readImageData();
			image = *bmpFile.getPixelVector();
		}
		imgFile = myIMGFile(bmpFile);
	}
	else {
		wxMessageBox("Error: Selected file not open for reading.");
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
		while (fileCol != fileRow->end()) {
			wxColor temp = *fileCol;
			*panelCol = temp;
			fileCol++, panelCol++;
		}
		fileRow++, panelRow++;
	}
}

// Take a wxColor RGB value and produces HSL values
// Calculation explained in report
void myPanel::RGBtoHSL(wxColor rgb, double& H, double& S, double& L)
{
	double Rf = double(rgb.Red()) / 255;
	double Gf = double(rgb.Green()) / 255;
	double Bf = double(rgb.Blue()) / 255;
	double Cmax = max(Rf, max(Gf, Bf));
	double Cmin = min(Rf, min(Gf, Bf));
	double delta = Cmax - Cmin;
	L = (Cmax + Cmin) / 2;

	if (delta == 0) {
		H = 0;
		S = 0;
	}
	else {
		if (L < 0.5) {
			S = (Cmax - Cmin) / (Cmax + Cmin);
		}
		else {
			S = (Cmax - Cmin) / (2.0 - Cmax - Cmin);
		}
		if (Cmax == Rf) {
			H = (Gf - Bf) / (Cmax - Cmin);
		}
		else if (Cmax == Gf) {
			H = 2.0 + (Bf - Rf) / (Cmax - Cmin);
		}
		else {
			H = 4.0 + (Rf - Gf) / (Cmax - Cmin);
		}
	}
	H *= 60;
	if (H < 0) { H += 360; }
	return;
}

// Take an HSL value and convert to wxColor RGB value
// Calculation explained in report
void myPanel::HSLtoRGB(wxColor& rgb, double H, double S, double L)
{
	double temp1;
	double temp2;
	double Rf, Gf, Bf;
	int R, G, B;
	if (S == 0) {
		int mono = round(L * 255);
		R = mono;
		G = mono;
		B = mono;
		
	}
	else {
		temp1 = (L < 0.5 ? L * (S + 1.0) : L + S - (L * S));
		temp2 = 2 * L - temp1;
		H /= 360;
		Rf = H + 0.333;
		Gf = H;
		Bf = H - 0.333;
		if (Rf < 0) { Rf += 1; }
		if (Rf > 1) { Rf -= 1; }
		if (Gf < 0) { Gf += 1; }
		if (Gf > 1) { Gf -= 1; }
		if (Bf < 0) { Bf += 1; }
		if (Bf > 1) { Bf -= 1; }

		if ((Rf * 6) < 1) {
			Rf = temp2 + (temp1 - temp2) * 6 * Rf;
		}
		else if ((Rf * 2) < 1) {
			Rf = temp1;
		}
		else if ((3 * Rf) < 2 ) { 
			Rf = temp2 + (temp1 - temp2) * (0.666 - Rf) * 6;
		}
		else {
			Rf = temp2;
		}

		if ((Gf * 6) < 1) {
			Gf = temp2 + (temp1 - temp2) * 6 * Gf;
		}
		else if ((Gf * 2) < 1) {
			Gf = temp1;
		}
		else if ((3 * Gf) < 2) {
			Gf = temp2 + (temp1 - temp2) * (0.666 - Gf) * 6;
		}
		else {
			Gf = temp2;
		}

		if ((Bf * 6) < 1) {
			Bf = temp2 + (temp1 - temp2) * 6 * Bf;
		}
		else if ((Bf * 2) < 1) {
			Bf = temp1;
		}
		else if ((3 * Bf) < 2) {
			Bf = temp2 + (temp1 - temp2) * (0.666 - Bf) * 6;
		}
		else {
			Bf = temp2;
		}

		R = round(Rf * 255);
		G = round(Gf * 255);
		B = round(Bf * 255);
	}
	rgb = wxColor(R, G, B);
	return;
}

// Return pixel vector RGB element at index
wxColor myPanel::getPixelColor(const int row, const int col) const {
	if (row >= maxHeight || col >= maxWidth) {
		return wxColor("Black");
	}
	return image[row][col];
}
