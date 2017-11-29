#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]//leave this as is
void main(array<String^>^ args) {

	//DB Setup
	

	//startform
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Kalender::Form1 form;
	Application::Run(%form);

	


}
