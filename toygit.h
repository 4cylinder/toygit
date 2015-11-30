/******************************************************************************
 * @file
 * @brief This file defines the interface between the toygit functions and
 * the main driver.
 * The functions here are implemented in toygit.cpp
 *****************************************************************************/
#ifndef TOYGIT_H
#define TOYGIT_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sstream>

// Helper functions
bool init();
std::string getBranch (std::string fileName);
std::string execute(std::string cmd);
bool checkFile(std::string fileName);

// Main functions
void help();
bool processInput (int argc, char *argv[]);
bool branch(std::string fileName, std::string branchName);
bool checkIn (std::string fileName, std::string comment);
bool listVersions (std::string fileName);
bool checkOut(std::string fileName, std::string version);
#endif
