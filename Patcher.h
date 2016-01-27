using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Collections::Generic;

#ifndef __PATCHER__
#define __PATCHER__
#include"WS.h"
#include"WP.h"
#include "ToolPath.h"
#include "MtConnector.h"

ref class Patcher{

public:
	WP^ getAllExec(__int64 root,STEPNCLib::Finder ^find,WP^plan,__int64 index);


	Patcher(){root=nullptr;}
	void PullFromServer(MtConnector ^ mt);\
//creates raw tool paths with no patching
	void appendToFile(String^ file_name,String^ original,String^ new_file_name,bool convert_to_inches);
private:
	WP^ root;
	String^ originalSTPNC;
	String^newSTPNC;
public:
	//open and init finder and apt obect file
	//append a new workplan to the end of the workplans nested under the main work plan

	void OpenFile(String^file,String^ desiredWorkPlanPath,String^newWorkPlanName,STEPNCLib::Finder ^%find,STEPNCLib::AptStepMaker^%apt, __int64% wpID);
	//Delete all workplans and executables in the same root as wpid and before wpid
	//void DeleteBefore(__int64 wpid);

	//returns coordinates in inches
	List<array<double>^> ^  coordinates(String ^fileName,bool convert_to_inches);
	//get the stpnc work plan id following the path of enabled workplans
	//ie  HARDMOLDY/Profiling/Boeing
	//Possible problems:
	//the desired workplan does not exist or is not enabled or a workplan listed in 
	//in the path does not exist or is not enabled
	long long getWorkPlanByPath(array<String^>^ nameList,STEPNCLib::Finder ^find);
	//recurse from a given workplan and get all enabled Workingsteps in all nested
	//workplans
	//workingsteps STPNC id are stored in wsList
	void getAllWorksteps(long long root,STEPNCLib::Finder ^find,List<long long>^ wsList);
	//given an xml file with a name fileName assuming the xml file adheres to the mtCOnnect 
	// xlm schema (see mtcup.org) pulls only position data and uses the timestamps on the 
	//position data to calculate the  
	List<array<double>^> ^  coordinatesWithTime(String ^fileName,bool convert_to_inches);
	//get all of the nested enabled working steps in a workplan and return all of the tool
	//paths in, each nested list represents a WS
	List<List<long long>^> ^getWSPaths(long long workplan, STEPNCLib::Finder ^find);

	List<List<long long>^> ^ getAllRapidPaths(long long workplan, STEPNCLib::Finder ^find);
	double actualFeedRate(array<double>^ coor1,array<double>^ coor2);
	List<double> ^getAllPoints(List<long long>^ paths,STEPNCLib::Finder ^find);
	void patchRapid(List<long long>^ pathList,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find,double max_feed);
	void appendPatchWorkPlan(String^partFile,String^coorFile,String^outName,String^ path,bool toInches );
	void createPatchedFile(String^ partFile,String^ WPpath,String^newFileName,String^newWorkPlan,String^ coor);
	//ToolPath^ firstPath(__int64 root);
	void DeleteBefore(__int64 wpid, STEPNCLib::Finder ^find, STEPNCLib::AptStepMaker^apt);
	void generateWPForToolPath(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, ToolPath^newtool, ToolPath^old);
	void generateWSForToolPath(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, WS^, ToolPath^tp);
	ToolPath^ patchRapidToolPaths(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, ToolPath^tp);
	void rapidToPoints(STEPNCLib::AptStepMaker^apt, STEPNCLib::Finder ^ find, List<__int64>^STPNCpath);
	bool rapidStarted(List<bool>^ state);
	bool isFeedState(List<bool>^state, int count);
	void debugOut(STEPNCLib::AptStepMaker^apt, STEPNCLib::Finder^ find,double feed);
};
#endif