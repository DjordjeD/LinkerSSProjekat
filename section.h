#pragma once

#include <iostream>
#include <vector>

using namespace std;

class Section {

public:
	int getID();
	std::string getSectionName();
	int getSectionSize();

	void setId(int _id);
	void setSectionName(std::string _sectionName);
	void setSectionSize(int _sectionSize);
	void increaseSize(int size);
	void increaseSize();

	Section() = default;
	Section(int _sectionSize, std::string _sectionName) : sectionName(_sectionName), sectionSize(_sectionSize) {}
	Section(std::string _sectionName) : sectionName(_sectionName) {}
	vector<vector<char>> data;
	vector<int> offsets;
	int virtualAddress;
private:

	//int id;
	std::string sectionName;
	int sectionSize;



};
