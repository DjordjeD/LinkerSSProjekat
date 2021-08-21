#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>


#include "section.h"

using namespace std;

class Linker {



public:




	struct Symbol {

	public:

		string symbolName;
		int value;
		string symbolScope;
		int numberID;
		bool isDefinedAlready;
		string symbolSection;

	};

	struct RelocationRecord {
		bool isData;
		int offset; //gde krece offset
		string sectionName; //trenutna sekcija, gde da trazi relokaciju
		string relocationType;
		string symbolName;

		string fileName; // koji je fajl
		//bitno je da se razlikuje da li je offset u odnosu na sekciju ili u odnosu na sve sekcije (kod linkera)
	};

	struct SectionHelp {

		string fileName;
		string sectionName;
		int size;

		int leftBound; // [) je odnos
		int rightBound; //

	};

	bool mode; // hex =0 , linkable=1;

	vector<pair<string, int>> placeSections;

	set<string> sectionSet;//sve sekcije u setu
	vector<SectionHelp> sectionHelpVector; //za pomocne

	vector<Section> outputSectionList;
	vector<Symbol> outputSymbolTable;
	vector<RelocationRecord> outputRelocationTable;
	map<string, Section> outputSectionMap;

	map<string, map<string, Section>> allSectionMaps;
	map<string, vector<RelocationRecord>> allRelocationRecords;
	map<string, vector<Symbol>> allSymbolTables;

	void readAllBinaryFiles(vector<string> fileNames);
	void readBinaryFile(string file);

	void makeSectionHelper();
	void mergeSections();

	void printSectionMap(map<string, Section> sectionMap, vector <RelocationRecord> relocationTable);

};