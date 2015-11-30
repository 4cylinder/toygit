#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
using namespace std;

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
	if (data[data.size()-1]=='\r' || data[data.size()-1]=='\n')
		data = data.substr(0,data.size()-1);
	return data;
}

int main () {
	// expected output, actual output
	string exp, act;
	cout << "System tests for toygit.\n";
	// Clean up all files for a fresh start
	execute("rm .toygit");
	execute("rm -r .testfile.txt");
	
	// Test 1: init function
	act = execute("./toygit init");
	exp = "Creating toygit project...\nThis directory is now a toygit project.";
	assert(exp==act);
	assert(execute("ls .toygit")==".toygit");
	cout << "Test 1 passed" << endl;
	
	// Test 2: checkin function
	execute("touch testfile.txt");
	execute("echo \"hello\" > testfile.txt");
	act = execute("./toygit checkin testfile.txt \"First Commit\"");
	exp = "Checked in testfile.txt to branch main\ntestfile.txt is now version 0";
	assert(exp==act);
	assert(execute("ls .testfile.txt")=="main");
	assert(execute("ls .testfile.txt/main")=="0");
	cout << "Test 2 passed" << endl;
	
	// Test 3: checkin function again
	execute("echo \"hello world\" > testfile.txt");
	act = execute("./toygit checkin testfile.txt \"Second Commit\"");
	exp = "Checked in testfile.txt to branch main\ntestfile.txt is now version 1";
	assert(exp==act);
	assert(execute("ls .testfile.txt/main")=="0\n1");
	cout << "Test 3 passed" << endl;
	
	// Test 4: list function
	act = execute("./toygit list testfile.txt");
	exp = "Current working branch: main\n";
	exp += "Version: 0\nFirst Commit\n";
	exp += "----------------------------------------------------\n";
	exp += "Version: 1\nSecond Commit\n";
	exp += "----------------------------------------------------";
	assert(exp==act);
	cout << "Test 4 passed" << endl;
	
	// Test 5: checkout function (specify version 0)
	act = execute("./toygit checkout testfile.txt 0");
	exp = "Checked out version 0 of testfile.txt from branch main";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="hello");
	cout << "Test 5 passed" << endl;
	
	// Test 6: checkout function (specify version 1)
	act = execute("./toygit checkout testfile.txt 1");
	exp = "Checked out version 1 of testfile.txt from branch main";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="hello world");
	cout << "Test 6 passed" << endl;
	
	// Test 7: checkout function (specify no version)
	execute("echo \"hello\" > testfile.txt");
	act = execute("./toygit checkout testfile.txt");
	exp = "Checked out version 1 of testfile.txt from branch main";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="hello world");
	cout << "Test 7 passed" << endl;
	
	// Test 8: checkout function (specify too big of a version)
	execute("echo \"hello\" > testfile.txt");
	act = execute("./toygit checkout testfile.txt 13");
	exp = "Checked out version 1 of testfile.txt from branch main";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="hello world");
	cout << "Test 8 passed" << endl;
	
	// Test 9: check current working branch
	assert(execute("tail .testfile.txt/.branch")=="main");
	cout << "Test 9 passed" << endl;
	
	// Test 10: make a new working branch
	act = execute("./toygit branch testfile.txt new");
	exp = "Previous working branch for testfile.txt is main\n";
	exp += "New branch new created for testfile.txt\n";
	exp += "Current working branch for testfile.txt is new\n";
	exp += "Next checkin will be version 0";
	assert(exp==act);
	assert(execute("tail .testfile.txt/.branch")=="new");
	cout << "Test 10 passed" << endl;
	
	// Test 11: Switch back to main branch
	act = execute("./toygit branch testfile.txt main");
	exp = "Previous working branch for testfile.txt is new\n";
	exp += "Current working branch for testfile.txt is main\n";
	exp += "Next checkin will be version 2";
	assert(exp==act);
	assert(execute("tail .testfile.txt/.branch")=="main");
	cout << "Test 11 passed" << endl;
	
	// Test 12: first checkin on new branch
	execute("./toygit branch testfile.txt new");
	assert(execute("tail .testfile.txt/.branch")=="new");
	execute("echo \"newbranch\" > testfile.txt");
	act = execute("./toygit checkin testfile.txt \"First Newbranch Commit\"");
	exp = "Checked in testfile.txt to branch new\ntestfile.txt is now version 0";
	assert(exp==act);
	assert(execute("ls .testfile.txt/new")=="0");
	cout << "Test 12 passed" << endl;
	
	// Test 13: list on new branch
	act = execute("./toygit list testfile.txt");
	exp = "Current working branch: new\n";
	exp += "Version: 0\nFirst Newbranch Commit\n";
	exp += "----------------------------------------------------";
	assert(exp==act);
	cout << "Test 13 passed" << endl;
	
	// Test 14: second checkin on new branch
	execute("echo \"newbranch modify\" > testfile.txt");
	act = execute("./toygit checkin testfile.txt \"Second Newbranch Commit\"");
	exp = "Checked in testfile.txt to branch new\ntestfile.txt is now version 1";
	assert(exp==act);
	assert(execute("ls .testfile.txt/new")=="0\n1");
	cout << "Test 14 passed" << endl;
	
	// Test 15: checkout version 0 from new branch
	act = execute("./toygit checkout testfile.txt 0");
	exp = "Checked out version 0 of testfile.txt from branch new";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="newbranch");
	cout << "Test 15 passed" << endl;
	
	// Test 16: checkout version 1 from new branch
	act = execute("./toygit checkout testfile.txt 1");
	exp = "Checked out version 1 of testfile.txt from branch new";
	assert(exp==act);
	assert(execute("tail testfile.txt")=="newbranch modify");
	cout << "Test 16 passed" << endl;
	
	// Test 17: list again on new branch
	act = execute("./toygit list testfile.txt");
	exp = "Current working branch: new\n";
	exp += "Version: 0\nFirst Newbranch Commit\n";
	exp += "----------------------------------------------------\n";
	exp += "Version: 1\nSecond Newbranch Commit\n";
	exp += "----------------------------------------------------";
	assert(exp==act);
	cout << "Test 17 passed" << endl;
	
	// Test 18: Suggest function
	
	return 0;
}
