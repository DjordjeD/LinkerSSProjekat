#include "linker.h"
#include <algorithm>

int main(int argc, char** argv) {

	Linker linker;

	vector<string> files;

	files.push_back("inputBinary1");
	files.push_back("inputBinary2");
	linker.readAllBinaryFiles(files);

	//sortuj PLACE OBAVEZNO
	vector<pair<string, int>> placeSections;

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