/******************************************************************************
 * @file
 * @brief This is the toygit unit tester for all the individual functions
 *****************************************************************************/
 #include "toygit.h"
 #include <assert.h>
 using namespace std;
 
int main() {
	cout << "Running toygit unit tests...\n";
	// Test checkFile helper function
	assert (checkFile("unittest.txt")==true);
	assert (checkFile("fail.txt")==false);
	
	// Test execute helper function
	assert (execute("pwd")=="/home/tzuoshuin/a2");
	assert (execute("ls unittest.txt")=="unittest.txt");
	
	// Test getBranch helper function
	assert (getBranch("sample.txt")!="");
	assert (getBranch("sample.txt")=="main");
	
	cout << "All toygit unit tests succeeded!\n";
	return 0;
}
