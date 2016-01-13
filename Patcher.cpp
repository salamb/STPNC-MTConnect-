#include "Patcher.h"
WP^ Patcher::getAllExec(__int64 root,STEPNCLib::Finder ^find,WP^plan,__int64 index){
	WP^ mainWP =nullptr;
	String^ name;
	WS^ mainWS=nullptr;
	ToolPath^mainTP=nullptr;
	__int64 toolID;
	double feed,spindle;
	bool rapid,coolant;
	if (find->IsProgramStructure(root)){
		name=find->GetExecutableName(root);
		
		mainWP=gcnew WP(name,root,index,plan);
		if(plan!=nullptr){
		plan->addExecutable(mainWP);
		}
		List<long long>^nestedPlans =find->GetNestedExecutableAllEnabled (root);
		for (int i=0;i<nestedPlans->Count;i++){
			
				if(find->IsProgramStructure(nestedPlans[i]))
					{
						getAllExec(nestedPlans[i],find,mainWP,i);
					
					}
				else if (find->IsWorkingstep(nestedPlans[i])&&find->IsEnabled(nestedPlans[i])){
					name=find->GetExecutableName(nestedPlans[i]);
					toolID=find->GetWorkingstepTool(nestedPlans[i]);	
					mainWS=gcnew WS(name,nestedPlans[i],i,mainWP,toolID);
					mainWP->addExecutable(mainWS);
					//need to add all toolpaths
					List<__int64>^ pathID=find->GetWorkingstepPathAll(nestedPlans[i]);
					for (int j=0;j<pathID->Count;j++){
						find->GetPathProcess(pathID[j],feed,spindle,rapid,coolant);
						mainTP=gcnew ToolPath(mainWS,rapid,feed,spindle,j,pathID[j]);
						mainWS->addPath(mainTP);
					}
				}
				
				}
		
		
		}
	return mainWP;
	}


//generates position information from the xml file
List<array<double>^> ^  Patcher::coordinates(String ^fileName,bool convert_to_inches){
//create instance of the xml reader
	
	XmlDocument ^doc = gcnew XmlDocument();
	doc->Load(fileName);
	XmlNode ^root = doc->DocumentElement;
		
		XmlNamespaceManager ^nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		List<array<double>^> ^Lst = gcnew List<array<double>^>();
		String ^str;
		XmlNodeList^ pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		
		array<double> ^coor;
		for (int i=0;i<pathList->Count;i++){
			XmlNode^ temp=pathList->Item(i);
			str=temp->InnerText;
			if(str!="UNAVAILABLE"){
			//Console::WriteLine(str);
			array<String^>^ arr;
			arr=str->Split(' ');
			coor=gcnew array<double>(3);
			try
			{
			coor[0]=Convert::ToDouble(arr[0]);
			coor[1]=Convert::ToDouble(arr[1]);
			coor[2]=Convert::ToDouble(arr[2]);
			if(convert_to_inches){
			coor[0]=coor[0]*0.0393701;
			coor[1]=coor[1]*0.0393701;
			coor[2]=coor[2]*0.0393701;
			
			}
			//Console::WriteLine(arr[0]);
			//Console::WriteLine(coor[0]);
			Lst->Add((array<double> ^)coor->Clone());
			}
			catch (Exception^ e){
			Console::WriteLine(e);
			
			}
			}
		
		}
return Lst;
}
//request a sampel of position information form server
void Patcher::PullFromServer(MtConnector ^ mt){






STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
   apt->Inches();


   apt->Open238 ("hardmoldy_ext.stpnc");
    find->Open238 ("hardmoldy_ext.stpnc");
	//feat->OpenSpecificWorkpiece("test_with_rawpiece2.stpnc","HardModly_design.stp");
	
    int wp_id = find->GetMainWorkplan();
	Console::WriteLine("Main Workplan name %S\n", (find->GetExecutableName(wp_id)));

    // add a new workplan at the end of the project
    apt->NestWorkplanAfter("Workplan for new results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);

    // Starting first workingstep in new plan

    // Create a trivial STEP-NC file
    apt->Workingstep("Extension 1");



    // Create a trivial STEP-NC file
    apt->PartNo ("test part");
	 apt->Rawpiece ("HardMoldy_stock.stp");
    long raw_id = apt->GetCurrentRawpiece ();
	 apt->DefineTool(0.25, 5.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    apt->PutWorkpiecePlacement (raw_id, 0, 0, 0, 0, 0, -1, 1, 0, 0);
	apt->GeometryForToolNumber ("Iscar_2.stp", 1);
    apt->LoadTool (1);
     apt->SpindleSpeed (300);
    apt->Feedrate (30);


		apt->GoToXYZ("point0", 0,0,200);
		apt->GoToXYZ("point0", 0,0,200);

   String ^str;
	array<double>^ coor;
	for (int i=0;i<100;i++){
		//Console::WriteLine(i);
	coor=mt->getPos();
	if(coor!=nullptr){

		str= gcnew String(String::Format("point {0} ",i));
		Console::WriteLine(str);
        Console::WriteLine(coor[0]);

		apt->GoToXYZ(str, coor[0]*0.0393701, coor[1]*0.0393701, coor[2]*0.0393701);
	}
	}
	
	
    apt->SaveAsModules("hardmoldy_extended");
	Console::WriteLine("done");
	String ^readIn =Console::ReadLine();
 

}


void Patcher::appendToFile(String^ file_name,String^ original,String^ new_file_name,bool convert_to_inches){
	List<array<double>^> ^coor_List= coordinates(file_name,convert_to_inches);
    // Create a trivial STEP-NC file
	Console::WriteLine("Got Position Data From File");
    STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	   
	    //	    APT.Open238("new_hardmoldy.stpnc");
	    //	    Find.Open238("new_hardmoldy.stpnc");
	    
	    apt->Open238(original);

	    long wp_id = find->GetMainWorkplan();
	    Console::WriteLine("Main Workplan name " + find->GetExecutableName(wp_id));

	    // add a new workplan at the end of the project
	    apt->NestWorkplanAfter("Workplan for MTConnect results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);

	    // Starting first workingstep in new plan
	    apt->Workingstep("testing");



//////////////////////////
    //apt->PartNo("test part");
    //apt->DefineTool(0.25, 5.0, 1.0, 2.0, 3.0, 4.0, 5.0);
	//apt->GeometryForToolNumber ("Iscar_2.stp", 1);


		//String^ boeingTool=find->GetToolNumberAsNumber()
		Console::WriteLine("is tool defined {0}",apt->IsToolDefined(1));
			apt->LoadTool(1);
    apt->SpindleSpeed (300);
    apt->Feedrate (30);
	String ^str;
	array<double>^ coor;
	for (int i=0;i<coor_List->Count;i++){
		coor=coor_List[i];
		
		str=String::Format("point {0} ",i+1);
		//Console::WriteLine(str);
		//Console::WriteLine(" {0} {1} {2}",coor[0],coor[1],coor[2]);

		apt->GoToXYZ(str, coor[0], coor[1], coor[2]);
	
	}
    
	
	apt->SaveAsModules(new_file_name);
	Console::WriteLine("done");
	
   
}
//use a path of wP1/wp2/wp3 to get id of wp3 
long long Patcher::getWorkPlanByPath(array<String^>^ nameList,STEPNCLib::Finder ^find){
	long long root=find->GetMainWorkplan();
	bool found=false;
	List<long long>^tempList=gcnew List<long long>();
	tempList->Add(root);
	for (int i=0;i<nameList->Length;i++){
		found=false;
		for each(long long x in tempList){
			if(find->IsProgramStructure(x)){
			if(find->GetExecutableName(x)==nameList[i]){
				//Console::WriteLine(find->GetExecutableName(x));
			root =x;
			tempList=find->GetNestedExecutableAll(root);
			found=true;
			break;
			}
			}
		
		}
		if(found==false){
		return -1;
		}
	
	
	}
	
	Console::WriteLine("Found WorkPlan{0}",find->GetExecutableName(root));
	return root;
}
//recursively get all enabled worksteps
void Patcher::getAllWorksteps(long long root,STEPNCLib::Finder ^find,List<long long>^ wsList){
	if (find->IsProgramStructure(root)){
		List<long long>^nestedPlans =find->GetNestedExecutableAllEnabled (root);
		for each (long long x in nestedPlans){
			
				if(find->IsProgramStructure(x))
					{
						getAllWorksteps(x,find,wsList);
					
					}
				else if (find->IsWorkingstep(x)&&find->IsEnabled(x)){
					wsList->Add(x);
				}
				
				}
		
		
		}
	
	}
//each array is the tool coordinate with delta t to reach the particular pt
//the first cooridinate has a time stamp of 0
List<array<double>^> ^  Patcher::coordinatesWithTime(String ^fileName,bool convert_to_inches){
//create instance of the xml reader
	
	XmlDocument ^doc = gcnew XmlDocument();
	doc->Load(fileName);
	XmlNode ^root = doc->DocumentElement;
		
		XmlNamespaceManager ^nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		List<array<double>^> ^Lst = gcnew List<array<double>^>();
		String ^str;
		XmlNodeList^ pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		DateTime  tempTime=  DateTime();
		
		DateTime  tempTime2=  DateTime();
		array<double> ^coor;
		bool firstTimeStamp=true;
		for (int i=0;i<pathList->Count;i++){
			XmlNode^ temp=pathList->Item(i);
			str=temp->InnerText;
			if(str!="UNAVAILABLE"){
				
			//Console::WriteLine(str);
			array<String^>^ arr;
			arr=str->Split(' ');
			coor=gcnew array<double>(4);
			if(firstTimeStamp){
					tempTime=tempTime.Parse(temp->Attributes["timestamp"]->Value);
				coor[3]=0;
				firstTimeStamp=false;
			}else{
			tempTime2=tempTime2.Parse(temp->Attributes["timestamp"]->Value);
			//Console::WriteLine("    timestamp {0}",tempTime2);
			System::TimeSpan diff=(tempTime2.Subtract(tempTime));
			//Console::WriteLine("diff {0}",diff.TotalSeconds);
			coor[3]=diff.TotalSeconds;
			tempTime=tempTime2;

				
			
			
			}

			try
			{
			coor[0]=Convert::ToDouble(arr[0]);
			coor[1]=Convert::ToDouble(arr[1]);
			coor[2]=Convert::ToDouble(arr[2]);
			if(convert_to_inches){
			coor[0]=coor[0]*0.0393701;
			coor[1]=coor[1]*0.0393701;
			coor[2]=coor[2]*0.0393701;
			
			}
			//Console::WriteLine(arr[0]);
			//Console::WriteLine(coor[0]);
			Lst->Add((array<double> ^)coor->Clone());
			}
			catch (Exception^ e){
			Console::WriteLine(e);
			
			}
			}
		
		}
return Lst;
}
List<List<long long>^> ^Patcher::getWSPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	
	List<List<long long>^> ^ pathList=gcnew List<List<long long>^>();
	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->Add(find->GetWorkingstepPathAll(x));
		
	}
	return pathList;
}
/*
List<List<long long>^> ^getAllFeedPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	List<List<long long>^> ^ feedToolPaths=gcnew List<List<long long>^>();
	List<long long>^ tempList=nullptr;
	bool startedRapid=true;
	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	for each (long long path in pathList){
		if (tempList==nullptr){
			tempList=gcnew List<long long>();
			startedRapid=true;
		}

		if(!find->GetPathRapid(path)){

			tempList->Add(path);
			startedRapid=false;
		}else{
			if(startedRapid==false){
				feedToolPaths->Add(tempList);
				tempList=nullptr;
			
			}

		
		}
	
	}
	return feedToolPaths;
}
*/
//return list of array of all rapid tool paths
//each array repesents a consecutive rapid movement
List<List<long long>^> ^ Patcher::getAllRapidPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	List<long long>^tempPathList=nullptr;
	List<List<long long>^> ^ rapidToolPaths=gcnew List<List<long long>^>();
	//get all enabled working steps form a specified work plan
	getAllWorksteps(workplan,find,wsList);
	Console::WriteLine("workplan {0}",workplan);
	//for each ws append all of the tool paths within that
	//ws to pathList
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	bool prevPathIsRapid=false;
	bool currentPathIsRapid=false;
	
	//current path is index i-1
	for( int i=0;i<pathList->Count;i++){
		currentPathIsRapid=find->GetPathRapid(pathList[i]);

		if(currentPathIsRapid &&tempPathList==nullptr){
			tempPathList=gcnew List<long long>();
			tempPathList->Add(pathList[i]);
		}else if(currentPathIsRapid &&tempPathList!=nullptr){
			tempPathList->Add(pathList[i]);
		}else if(!currentPathIsRapid &&tempPathList!=nullptr){
			rapidToolPaths->Add(tempPathList);
			tempPathList=nullptr;
		}


		
	
	}
	return rapidToolPaths;
}

double Patcher::actualFeedRate(array<double>^ coor1,array<double>^ coor2){
	double dx=coor1[0]-coor2[0];
	double dy=coor1[1]-coor2[1];
	double dz=coor1[2]-coor2[2];
	
	double dist= Math::Sqrt( Math::Pow(dx,2)+Math::Pow(dy,2)+Math::Pow(dz,2))/coor2[3];
	
		//Console::WriteLine("dx {0} dy{1} dz {2}  deltat {3} speed{4}",dx,dy,dz,coor2[3],dist);
	
	return dist;
}
List<double> ^Patcher::getAllPoints(List<long long>^ paths,STEPNCLib::Finder ^find){
	List<long long >^ curves=nullptr;
	List<double> ^pts=gcnew List<double>();
	for each (long long path in paths){
		curves=find->GetPathCurveAll(path);
		for each (long long curve in curves){
			pts->AddRange(find->GetPathPolylinePointAll(curve));
		
		
		}
	
	}
return pts;
}
//takes coordinates from curves in the paths form pathlsit and generates tool paths
void Patcher::patchRapid(List<long long>^ pathList,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find,double max_feed){
	apt->Rapid();
	apt->Workingstep("starting Rapid");
		List<double>^ pts=getAllPoints(pathList,find);
		int totalPts=pts->Count/3;
		for (int i=0;i<totalPts;i++){
			apt->GoToXYZ("rapid",pts[i*3],pts[i*3+1],pts[i*3+2]);
		
		}

	apt->Feedrate(max_feed);
	apt->Workingstep("starting Feed");
}

//given a stpnc file and a workplan within the stepnc file
// append a workplan with tool paths genereated from the xml data
//that contains tool position
//the tool paths represented in the xml file should be identical to the
//toolpaths of the ws in the identified workplan
void Patcher::appendPatchWorkPlan(String^partFile,String^coorFile,String^outName,String^ path,bool toInches ){
	Console::WriteLine("input \n part {0}\n xml file{1} \n path to desired workplan {2}",partFile,coorFile,path);
	//assume all units are inches or inches per sec
	double max_feed=14.0/60;//inches per sec
	//get path position coordinates from the xml file with delta t in seconds at end
	List<array<double>^> ^coorList=	coordinatesWithTime(coorFile,toInches);
	Console::WriteLine("Got coordinates list");
	STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	array<String^>^ nameList=path->Split('/');

	STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	apt->Inches();

	//open the part file
	apt->Open238 (partFile);

	find->Open238 (partFile);
	//get the workplan id 
	long long id= getWorkPlanByPath(nameList,find);
	//get all tool paths within a work plan that has a transition from feed to rapid or
	//rapid to feed
	List<List<long long>^> ^transitionPaths= getAllRapidPaths(id, find);

	//begin the extended version of the stpnc file


	long wp_id = find->GetMainWorkplan();
	Console::WriteLine("Main Workplan name " + find->GetExecutableName(wp_id));

	// add a new workplan at the end of the project
	apt->NestWorkplanAfter("Workplan for MTConnect results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);
	// Starting first workingstep in new plan

	apt->Workingstep("testing");


	Console::WriteLine("is tool defined {0}",apt->IsToolDefined(1));
	apt->LoadTool(1);
	apt->SpindleSpeed (300);
	apt->Feedrate (max_feed);


	String ^str;
	array<double>^ coor1;
	array<double>^coor2;

	bool startRapid=false;
	bool currentCoorisRapid=true;
	int currentTransition=0;
	Console::WriteLine("number of path coordinates {0}",coorList->Count);
	Console::WriteLine("number of transitions {0}",transitionPaths->Count);
	//determine if starting position is in rapid mode or feed
	if(coorList->Count>1){
		coor1=coorList[0];
		coor2=coorList[1];
	
		if (actualFeedRate(coor1,coor2)>max_feed){
			currentCoorisRapid=true;
			

		}else{

			currentCoorisRapid=false;
		}
			
		if(currentCoorisRapid){
				apt->Rapid();
			}else{
				apt->Feedrate(max_feed);
			}
		}
	
	double actualFeed=0;
	//iterate over all pair of consecutive points
	for (int i=1;i<coorList->Count;i++){
		coor1=coorList[i-1];
		coor2=coorList[i];

		str=String::Format("point {0} ",i+1);
		actualFeed=actualFeedRate(coor1,coor2);
		if (actualFeed>max_feed){
			currentCoorisRapid=true;
			//Console::WriteLine("actualFeed {0}",actualFeed);
		}else{

			currentCoorisRapid=false;
		}
		
		if (!startRapid&&currentCoorisRapid){
			
			if(currentTransition<transitionPaths->Count){

				//Console::WriteLine("rapid at coor ({0}{1}{2})",coor1[0],coor1[1],coor1[2]);
				startRapid=true;
				patchRapid(transitionPaths[currentTransition],apt,find,max_feed);
				currentTransition+=1;
			}
		
		}else if(currentCoorisRapid==false){
		
			startRapid=false;
			apt->GoToXYZ("feed",coor2[0],coor2[1],coor2[2]);
		}
		/*if(currentTransition<transitionPaths->Count){
			Console::WriteLine(" coor ({0}{1}{2}) actual {3} max {4}",coor2[0],coor2[1],coor2[2],actualFeed,max_feed);
			}*/
		

	}
	apt->SaveAsModules(outName);
	Console::WriteLine("done");



}
//open a stpnc file locate the desired workplan via path 
//modify find and apt pointers to instance of of thier objects
//pass the reference the WPID of the desired workplan
//force the apt builder and the find object to work in inches
void Patcher::OpenFile(String^file,String^ desiredWorkPlanPath,String^newWorkPlanName,STEPNCLib::Finder ^%find,STEPNCLib::AptStepMaker^%apt, __int64% wpID){

	apt = gcnew STEPNCLib::AptStepMaker();
	find=gcnew STEPNCLib::Finder();
	array<String^>^ nameList=desiredWorkPlanPath->Split('/');
	__int64 id= getWorkPlanByPath(nameList,find);
	apt->Inches();
	find->APIUnitsInch();
}
//Delete all workplans on directly nested under the main workplan
//except for wpid
void DeleteBefore(__int64 wpid,STEPNCLib::Finder ^find,STEPNCLib::AptStepMaker^apt){
	__int64 mainWP=find->GetMainWorkplan();
	List<__int64> ^WPList=find->GetNestedExecutableAll(mainWP);
	for (int i=0;i< WPList->Count;i++){
		if(WPList[i]!=wpid){
			apt->DeleteExecutable(WPList[i]);
		}
	
	
	}


}

void Patcher::createPatchedFile(String^ partFile,String^ WPpath,String^newFileName,String^newWorkPlan,String coor,bool toInches){
	STEPNCLib::AptStepMaker^ apt;
	STEPNCLib::Finder^find;
	__int64 oldWPID;
	
	OpenFile(partFile,WPpath,newWorkPlan,find,apt,oldWPID);
	array<String^>^ nameList=WPpath->Split('/');
	//get the workplan id 
	
	
	
	root=getAllExec(oldWPID,find,nullptr,0);




}