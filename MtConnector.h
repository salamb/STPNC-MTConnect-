
#pragma once
using namespace System;
using namespace System::Net;
using namespace System::Xml;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
//connects to an MTConnect server and collects position data
ref class MtConnector
{
private:
	String^ server;
	String^query1;
	String^query2;
	Boolean startedPush;
	XmlDocument ^doc ;
	WebClient ^wc;
	int getMessageSize(String^bound,StreamReader^ reader);
	array<wchar_t>^ getMessage(int size,StreamReader ^ reader);
	void printXMLData(array<wchar_t>^ buff,DateTime ^%,bool firstTime,StreamWriter^ writer);
public:
	MtConnector();
	array<double>^ getPos();
	void printData();
	Boolean startPush(String ^ str);
	//submit a request to an MTconnect server for positon information
	//
	void getRequest(String^ str,String^ dataLocation);
	//under the incorrect assumption that MTconnect would open a new connection every so often
	//
	
	void EventHandler(Object^ sender, OpenReadCompletedEventArgs^ e);


};
//delegate void OpenReadCompletedEventHandler (Object^ sender, OpenReadCompletedEventArgs^ e);
	
