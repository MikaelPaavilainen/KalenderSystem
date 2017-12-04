#include "Form1.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]//leave this as is
int main(array<String^>^ args) {



	//startform
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Kalender::Form1 form(args);
	Application::Run(%form);
	return 0;
}
