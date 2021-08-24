#include "linker.h"
#include <algorithm>
#include <string>
int main(int argc, char** argv) {

	Linker linker;

	vector<string> files;

	files.push_back("inputBinary1");
	files.push_back("inputBinary2");
	linker.readAllBinaryFiles(files);

	linker.mode = 0;

	//sortuj PLACE OBAVEZNO
	vector<pair<string, int>> placeSections;
	string s = "500";



	placeSections.push_back(pair<string, int>("isr", stoi(s, 0, 16)));

	sort(placeSections.begin(), placeSections.end(), [](auto& left, auto& right) {
		return left.second < right.second;
		});

	linker.placeSections = placeSections;
	//linker.readBinaryFile("");

	linker.makeSectionHelper();
	linker.mergeSections();
	linker.mergeRelocations();
	linker.mergeSymbolTable();
	linker.mergeDataSections();
	linker.fixRelocationData();

	linker.print();
	//linker.makeTxtFile();
}