/*
 * FileManager.h
 *
 *  Created on: Feb 20, 2018
 *      Author: yousaf
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#include <string>
using namespace std;


class FileManager {
public:

	void findFile(string name, string &contents, int &size);

	string getFileRange(string filename, int start, int end);

	int getFileSize(string name);
	string addPadding(string data, int size);



	FileManager();
	virtual ~FileManager();

private:



};

#endif /* FILEMANAGER_H_ */
