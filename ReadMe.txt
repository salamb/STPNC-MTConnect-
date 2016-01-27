========================================================================
    APPLICATION : MTConnect Project Overview
========================================================================


MTConnect

Problem statement:
When recording raw position coordinates from an mtconnect server
it is possible to keep track of tool coordinates while a cnc
machine is executing a program. Ideally it it would be possible to 
use position data to generate/recreate the tool paths that the machine actually
took while machining. A 3d simulation can be constructed to represent actual movements
of the machine while executing a program 

These generated toolpaths could potentially be used to determine 
if a part was machined correctly.Furthermore if the simulation was
generated in reatime machinists could get realtime feed back if a part is 
being machined correctly.

The general algorithm and potential use cases of the general algorithm was inspired by:

Digital Manufacturing Final Report
MTConnect Model Reconstruction and Tolerance Analysis
By RPI students in Professor Hardwick's Digital Manufacturing Class



General Algorithm-

PreProcessing-
Take a STPNC file and get the all of the enabled ToolPaths
Record every instance where the current tool path not a rapid
and the next toolpath is a rapid   
Record the mtconnect data when the machine is executing the all
of the enabled tool paths of the specified STNC file


Get each pair of coordinates from MTConnect Data
Get the distance between the points (dist)
Using the time stamps provided by mtconnect get the time span between consecutive readings
Calculate the current speed of the tool
If the speed of the tool is greateer than the maximum feedrate then asume the tool is 
executing a rapid otherwise the tool is not executing a rpaid

Current Algorithm-

Data Recording and storage-

Collecting Raw position Information-

All mt-connect data is pre-recorded using the MTConnector class
getRequest(String^ str,String^ dataLocation)

str is the query to the mt connect server
The query must request the mtconnect server(agent) to push data to 
the client(this program) over time intervals 
It is ideal for the request to contain an xpath to limit the amount 
of info to path position only inorder to improve performance
an example of str would be:

http://okuma-matata:5000/sample?from=123?count=1000&interval=1000&path=//Path/DataItems/DataItem[@type=\"PATH_POSITION\"]"

The xpath query is case dependent and this query requests 1000 new samples every 1000ms or 1 second
"from" indicates what sequence number to start from in the mtconnect server buffer
if from is not specified all data in the mtconnect server  buffer will be retrieved before retrieving current
data

the data will be printed to console when available  other wise the size of the empty packet will be printed
to console


data is recorded in the following format:

xcoordinate ycoordinate zcoordinate timeDiffInSeconds sequence number
xcoordinate ycoordinate zcoordinate timeDiffInSeconds sequence number

coordinates are the workpiece coordinates of the tool path
timeDiffInSeconds is the time difference between the previous
	position recording and the current recording
sequence number is the mtConnect identifier signifying when in chronologicla order
the sample was taken in reference of all ofthe other samples that were taken
(refereenced in the query strign above)

After the cnc machine is done executing you can close the console
(data is written to the specified text file for every line so data will not 
be lost in a write stream)

Running the Patch Algorithm-

create an instance of Patcher
and run
                         
pat->createPatchedFile("hardmoldy_imts_signed.stpnc", "HARDMOLDY", "patchedMoldy", "PatchWP", "Data.txt");
 parameters
	original stpnc file to reference 
	the workplan of the stpncfile that the machine executed if the workplan is nested(not the main workplan a path can be specified using mainWprkplanName/subPlanName/...)
	Name of the new stpnc file to be created with the recorded tool paths 
	Name of the appended workplan containing the recorded tool paths of the stpnc machine
	Name of the text file with the prerecorded data

A new stpnc file is created with to recoreded machine tool paths recorded and properly 
divided into thier respective working steps and working plans

Current Algorithm Logistics-

The current algorithm will find the specified stnc file and span over all workplans untill it finds the specified workplan 
The specified worklan is the workplan the recorded data represents(usually the mian workplan as shown in the example above).

The General Algorithm is performed with the exception that the that for a current position to recorded as feed state in
the stpnc file the past three positons must have been feedstates(moments when the machine is not performing a rapid).As a 
result if current postion is recorded when a tool is moving slowly, that position is considerd to be a feed state.
Even if the positon is considered a feed state it will not be recorded unless three preceding recordings 
represent feed states. As a result a small amount of data is lost. This procedure acts as a filter to make sure 
that transitions from rapids to feed are not considerd feed states. 


Notes:
The mtconnect data had to be "cleaned" the first few coordinated of the mtconeect data had to be deleted since
it represented tool movement not part of workplan being executed
The simulation of an okuma running hardmoldy was used to generate raw data. 
The simulation executed feed rates much faster than the specified feed rate in the 
stpnc file
HardMoldy was used to test this algorithm
A hardcoded .6inches per second was used to differentiate when a machine was executing a 
rapid or not the acutal feed rate chould be ~.22inches per second
Due to the nature of the STPNC dll objects representing working steps, workplans and Toolpaths 
were created to make accessing data easier.

Future Plans:
reference the feed rate of the current working step to determine if the macine is currently on a rapid
rather than referencing 2 points at a time have a buffer of n points and use the buffer to determine when a 
feed starts or end.
Have the program read in mtconnect data in realtime and output generated tool paths in realtime for display and 
analysis
Handle cases setups cause a transformatio  change
Handle bad data at the begining of a mtconnect recording
With knowledge of what tool path you are currently on calcualte te minimum distance between the current curve
the tool path should be on and its cautal position



 








