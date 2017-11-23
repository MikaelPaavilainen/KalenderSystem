#include "odbc.h"

void Database::Connect() {
	string conString;
	OdbcConnection cnn;
	conString = "Driver={MySQL ODBC 5.3 ANSI Driver};DBQ=kalender;";
	cnn = new OdbcConnection(conString);
	try
	{
		cnn.Open();
		MessageBox.Show("Connection Open ! ");
		cnn.Close();
	}
	catch (Exception ex)
	{
		MessageBox.Show("Can not open connection ! ");
	}
}