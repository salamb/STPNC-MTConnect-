using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
#pragma once
#ifndef __EXEC__
#define __EXEC__

ref class WP;
 ref class Exec
{
private:
String ^ name;
__int64 id;
__int64 index;
WP^ parent;
bool typeWP;
public:

  bool isWP(){return typeWP;};
	//get name of Executable
	String^ getName(){return name;};
	// get stpnc id of Executable
	__int64 getId(){return id;};
	// index in list of enabled executalbes for an enabled parent WorkPlan
	__int64 getIndex(){return index;};

	WP^ getParent();
	Exec(String^ n,__int64 ID,__int64 in,WP^p,bool iswp){
	name=n;
	id=ID;
	index=in;
	parent=p;
	typeWP=iswp;
	}
};
#endif