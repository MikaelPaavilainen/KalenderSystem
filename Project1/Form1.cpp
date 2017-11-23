#include "Form1.h"

#include <stdio.h>  // for printf

using namespace Project1;
using namespace System::Data::Odbc;

[STAThreadAttribute]
int main(int argc, char* argv[])
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew Form1());
	
	return 0;
}
