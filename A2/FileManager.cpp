/*
 * FileManager.cpp
 *
 *  Created on: Feb 20, 2018
 *      Author: yousaf
 */

#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

string FileManager::addPadding(string data, int size){
	if (size==8888 || size==1111){
		return data;
	}
	while((size+1)%8!=0){
		data+='~';
		size++;
	}

	return data;
}


int FileManager::getFileSize(string name){
	ifstream file( name.c_str(), ios::binary | ios::ate);
	return file.tellg();
}




string FileManager::getFileRange(string filename, int start, int length){
	string contents="";
	int size = -1;
	findFile(filename, contents, size);
	if (size<=0){
		return "Server error: File Not Found";
	} else {
		return contents.substr(start,length);
	}

}



void FileManager::findFile(string name, string &contents, int &size){

	size = FileManager::getFileSize(name);

	int sum = 0;
	string x = "";
	contents = "";
	    ifstream inFile;

	    inFile.open(name.c_str()); // Open
	    if (!inFile) { // Check
	        cout << "Unable to open file";
	    }

	    //while (inFile >> x){
	    //	contents+=x;
	    //}
	   // inFile.close();


	    std::ostringstream oss;
	    oss << inFile.rdbuf();
	    contents = oss.str();
}

FileManager::FileManager() {
	// TODO Auto-generated constructor stub

}

FileManager::~FileManager() {
	// TODO Auto-generated destructor stub
}

