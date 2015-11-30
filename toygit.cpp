/******************************************************************************
 * @file
 * @brief This file implements the functions defined in toygit.h
 *****************************************************************************/
#include "toygit.h"
using namespace std;

/******************************************************************************
* @brief Helper function: Check if a file exists in the current directory
* @param fileName The name of the file
* @return Returns true if the file exists and false otherwise.
******************************************************************************/
bool checkFile(string fileName) {
    struct stat fileInfo;
    return stat(fileName.c_str(), &fileInfo) == 0;
}

/******************************************************************************
* @brief Helper function: Execute a unix command and save its output as a string
* @param cmd The full command to be executed
* @return Returns a string containing the command's output (it may be blank)
******************************************************************************/
string execute(string cmd) {
	string data;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");
	// Create a stream to contain the output of the unix command
	FILE * stream;
	stream = popen(cmd.c_str(), "r");
	if (stream) {
		// Loop through the stream and append its contents to the string that
		// is returned when this function exits
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) 
				data.append(buffer);
		pclose(stream);
	}
	// strip off carriage return character at the end
	data = data.substr(0,data.size()-1);
	return data;
}

/******************************************************************************
* @brief Helper function: Find the current working branch for a certain file.
* @param fileName The name of the file we're looking at.
* @return The name of the current working branch for this file.
******************************************************************************/
string getBranch (string fileName) { 
	// Every file in the project is associated with a hidden folder that has
	// the same name as the file. Each hidden folder contains a hidden file 
	// called ".branch" that has the name of the current working branch.
	string cmd = "."+fileName+"/.branch";
	string branch;
	// Open that file and grab its contents to return
	fstream f;
	f.open(cmd.c_str(),fstream::in);
	getline(f,branch);
	f.close();
	return branch;
}

/******************************************************************************
* @brief Lists helpful info about how to use toygit (similar to "man" in Unix)
* @return None
******************************************************************************/
void help(){
	cout << "usage: toygit" << endl;
}

/******************************************************************************
* @brief Call functions matching arguments invoked when toygit is executed
* @param argc The number of arguments (passed in from the main)
* @param argv The arguments as an array of strings (passed in from the main)
* @return Returns the return value of the desired function (default false)
******************************************************************************/
bool processInput (int argc, char *argv[]) {
	string cmd = argv[1];
	if (cmd=="init") {
		return init();
	} else if (cmd=="branch") {
		return branch(argv[2],argv[3]);
	} else if (cmd=="checkin") {
		if (argc==3)
			return checkIn(argv[2],"");
		else if (argc==4)
			return checkIn(argv[2],argv[3]);
	} else if (cmd=="checkout") {
		if (argc==3)
			return checkOut(argv[2],"");
		else if (argc==4)
			return checkOut(argv[2],argv[3]);
	}else if (cmd=="list") {
		return listVersions(argv[2]);
	} 
	return false;
}

/******************************************************************************
* @brief Initialize the current working directory as a toygit project
* @return True if a new project is successfully set up, and false otherwise
******************************************************************************/
bool init(){
	// Check to see if the directory is already a toygit project, by checking 
	// for the existence of the file ".toygit"
	if (checkFile(".toygit")==true) {
		cout << "This directory is already a toygit project.\n";
		return true;
	}
	// Create a file called ".toygit" to mark the current directory as a project
	cout << "Creating toygit project...\n";
	execute("touch .toygit");
	if (checkFile(".toygit")==false) {
		cout << "Error creating toygit project\n";
		return false;
	}
	cout << "This directory is now a toygit project.\n";
	return true;
}

/******************************************************************************
* @brief Create a new branch for a file and switch to the new branch
* 		(or switch immediately if the branch already exists)
* @param fileName The file to be branched
* @param branchName The name of the branch to be created (or switched to)
* @return Returns true if the operation is successful and false otherwise
******************************************************************************/
bool branch(string fileName, string branchName) {
	// Check to see if the file exists in the current directory
	if (checkFile(fileName)==false){
		cout << "Error: The file "+fileName+" does not exist\n";
		return false;
	}
	// Check to see if the file has been checked in at least once
	if (checkFile("."+fileName+"/.branch")==false) {
		cout << "Error: Please check in this file at least once before ";
		cout << "creating a new branch for it.\n";
		return false;
	}
	
	// Get the name of the current working branch before switching
	cout << "Previous working branch for "+fileName+" is ";
	cout << getBranch(fileName) << endl;
	
	// If this branch doesn't already exist, create a subfolder for it inside
	// the hidden folder associated with this file.
	execute("mkdir ."+fileName+"/"+branchName);
	
	// Mark the current working branch as "branchName" by overwriting the
	// contents of the ".branch" hidden file inside the hidden folder.
	string cmd = "."+fileName+"/.branch";
	fstream f;
	f.open(cmd.c_str(),fstream::out);
	f << branchName;
	f.close();
	
	// See what the latest version is for this branch
	string version = execute("ls ."+fileName+"/"+branchName+"/ -lR | grep ^d | wc -l");
	
	// Newly created branch would have latest version = 0
	if (atoi(version.c_str())==0)
		cout << "New branch "+branchName+" created for "+fileName << endl;
	
	cout << "Current working branch for "+fileName+" is "+branchName << endl;
	cout << "Next checkin will be version " << version << endl;
	return true;
}

/******************************************************************************
* @brief Check in a file after creating or modifying it
* @param fileName The file to be branched
* @param comment A short description of the reason for modifying/adding the file
* @return Returns true if the operation is successful and false otherwise
******************************************************************************/
bool checkIn(string fileName, string comment) {
	// Check to see if the file exists in the current directory
	if (checkFile(fileName)==false){
		cout << "Error: The file "+fileName+" does not exist\n";
		return false;
	}
	
	// Every file in the project needs a hidden folder with the same name as 
	// the file. Each hidden folder contains subfolders for the file's branches.
	// Every file starts with a "main" branch by default.
	execute("mkdir ."+fileName);
	execute("mkdir ."+fileName+"/main");
	
	// Every hidden folder contains a hidden file ".branch" to store the name of
	// the current working branch. If this is the first time the file is being
	// checked in, then create this file and write "main" inside (for main branch).
	fstream f;
	string cmd;
	if (checkFile("."+fileName+"/.branch")==false) {
		cmd = "."+fileName+"/.branch";
		f.open(cmd.c_str(),fstream::out);
		f << "main";
		f.close();
	}
	
	// This variable = the directory for this file's branch
	string dir = "."+fileName+"/"+getBranch(fileName);
	
	// Count the number of files in the branch to be checked into
	// That tells us the version number for this check-in
	string version = execute("ls "+dir+"/ -lR | grep ^d | wc -l");
	dir = dir+"/"+version;
	
	// Make yet another subfolder for that version
	execute("mkdir "+dir);
	
	// Copy and paste the file into that branch version.
	execute("cp "+fileName+" "+dir);
	// Insert the comment
	cmd = dir+"/comment.txt";
	f.open(cmd.c_str(),fstream::out);
	f << comment;
	f.close();
	cout << "Checked in "+fileName+" to branch "+getBranch(fileName)+"\n";
	cout << fileName+" is now version "+version << endl;
	return true;
}

/******************************************************************************
* @brief Show all versions of a file with associated comments
* @param fileName The file whose versions we want to see
* @return Returns true if the operation is successful and false otherwise
******************************************************************************/
bool listVersions (string fileName) {
	// Check to see if the file exists in the current directory
	if (checkFile(fileName)==false){
		cout << "Error: The file "+fileName+" does not exist\n";
		return false;
	}

	// This variable = the directory for this file's branch
	string dir = "."+fileName+"/"+getBranch(fileName);
	cout << "Current working branch: " << getBranch(fileName) << endl;
	
	// Find the number of versions checked into this branch
	string version = execute("ls "+dir+"/ -lR | grep ^d | wc -l");

	// Loop through all versions in the branch and print their comments
	for (int i=0;i<atoi(version.c_str());i++){
		cout << "Version: " << i << endl;
		stringstream s;
		s << i;
		// Open the file where the checkin message was stored and obtain its
		// contents to print to the console.
		string cmd = dir+"/"+s.str()+"/comment.txt";
		ifstream f;
		f.open(cmd.c_str());
		string line;
		while (getline(f,line))
			cout << line << endl;
		f.close();
		// Print this to separate different versions to make them easier to
		// tell apart on the console (especially those w/ long comments)
		cout << "----------------------------------------------------\n";
	}
	return true;
}

/******************************************************************************
* @brief Check out an older version of a file by version number
* @param fileName The file we want to check out
* @param version The specific version to be checked out 
* @return Returns true if the operation is successful and false otherwise
******************************************************************************/
bool checkOut (string fileName, string version) {
	// Check to see if the file exists in the current directory
	if (checkFile(fileName)==false){
		cout << "Error: The file "+fileName+" does not exist\n";
		return false;
	}
	
	// This variable = the directory for this file's branch
	string dir = "."+fileName+"/"+getBranch(fileName);
	
	// If the inputted version number is too large, then it's automatically set
	// to the most recent version. E.g. if the latest version is 14 and the user
	// requested version 25, then he'll get version 14.
	string c = execute("ls "+dir+"/ -lR | grep ^d | wc -l");
	int count = atoi(c.c_str());
	int ver;
	// If the user didn't input a version number, then just return the latest
	if (version=="")
		ver = count-1;
	else
		ver = atoi(version.c_str()) % count;
	
	// Convert back to a string for easier concatenation
	stringstream stream;
	stream << ver;
	version = stream.str();
	
	// Location of that older version
	dir = "."+fileName+"/"+getBranch(fileName)+"/"+version+"/"+fileName;
	execute("cp "+dir+" .");
	cout << "Checked out version "+version+" of "+fileName;
	cout << " from branch " << getBranch(fileName) << endl;
	return true;	
}

