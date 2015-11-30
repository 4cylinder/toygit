/******************************************************************************
 * @file
 * @brief This is the main file for toygit, where the user's arguments on the
 * command line are read and processed.
 *****************************************************************************/
#include "toygit.h"
using namespace std;

int main (int argc, char *argv[]) {
	// If no arguments present, print help msg and exit
	if (argc < 2)
		help();
	else {
		processInput(argc, argv);
	}
	return 0;
}
