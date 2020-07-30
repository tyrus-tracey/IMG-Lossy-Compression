#include "myApp.h"

wxIMPLEMENT_APP(myApp);

myApp::myApp() {

}

myApp::~myApp() {
}

wxSize myApp::getDimensions() {
	return dimensions;
}

//Create a frame, and return true to indicate successful startup.
bool myApp::OnInit() {
	frame = new myFrame(dimensions);

	return true;
}
