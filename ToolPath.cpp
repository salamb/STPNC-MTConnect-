#include "ToolPath.h";


ToolPath^ ToolPath::nextPath(){
	if(parent!=nullptr){
		return parent->getPath(getIndex()+1);
	
	}else{
		
		Console::WriteLine("Error path  is not nested in a WS");
		return nullptr;}
return nullptr;
}

ToolPath^ ToolPath::nextPath(bool % newWP, bool% newWS){
	ToolPath^temp=nextPath();
	Exec^ tempExec;
	if (temp!=nullptr){
		newWP=false;
		newWS=false;
	return temp;
	}
	else{
		WS^ tempWS=parent;
		ToolPath^ nextToolPath=nullptr;
		List<WP^>^path=pathtoRoot(tempWS);
		for (int i=0;i<path->Count;i++){
			nextToolPath=recurseToNextToolPath(path[i],tempWS->getIndex());
		
		
		}
	}
}
List<WP^>^pathtoRoot(WS^temp){
	List<WP^> ^path=gcnew List<WP^>();
	WP^ temp1=temp->getParent();

	if(temp!=nullptr){
		while (temp1!=nullptr){
		path->Add(temp1);
		temp1=temp1->getParent();
		
		
		}
	
	}

	return path;

}
ToolPath^ recurseToNextToolPath(WP^ current , __int64 startIndexAfter){
	ToolPath^ result=nullptr;
	for(int i=startIndexAfter+1;i<current->getExecutableCount();i++){
		recurseToolPath(current->getExecutable(i),result);
		if(result!=nullptr){
			return result;
		}
	}

	
}
ToolPath^ recurseToolPath(Exec^ current){
	WS^ tempWS=nullptr;
	WP^ tempWP=nullptr;
	WP^ tempWP2;
	if (current->isWP()){
		tempWP=dynamic_cast<WP^>(current);
	}
	else{
	tempWS=dynamic_cast<WS^>(current);
	}
	if(tempWS!=nullptr){
		if(tempWS->getPathCount()>0){
			 return tempWS->getPath(0);
			 
		}
	}
	if(tempWP!=nullptr){
		for (int i=0;i<tempWP->getExecutableCount();i++){
			
			return recurseToolPath(tempWP->getExecutable(i));
			
		
		}
	
	
	}
	return nullptr;
}