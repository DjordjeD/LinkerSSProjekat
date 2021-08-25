#include "linker.h"
#include <algorithm>
#include <string>
#include <regex>
int main(int argc, char** argv) {


	bool output_file = false;

	bool place_file = false;
	regex place_regex("^-place=([a-zA-Z_][a-zA-Z_0-9]*)@(0[xX][0-9a-fA-F]+)$");
	smatch section_address;

	bool hex = false;
	bool linkable = false;


	vector<string> files;
	vector<pair<string, int>> placeSections;
	string outputFileName;

	for (int i = 1; i < argc; i++)
	{
		string current = argv[i];
		//cout << current << endl;
		if (current == "-o")
		{

			output_file = true;
			//cout << "IN:" << output_file << endl;
		}
		else if (current == "-linkable")
		{
			linkable = true;
		}
		else if (current == "-hex")
		{


			hex = true;
		}
		else if (regex_search(current, section_address, place_regex))
		{
			place_file = true;
			string section = section_address.str(1);
			int address = stoi(section_address.str(2), nullptr, 16);
			placeSections.push_back(pair<string, int>(section, address));
		}
		else if (output_file)
		{
			//cout << "OF:" << output_file << endl;
			outputFileName = current;
			output_file = false;
		}
		else
		{
			//cout << "File for link:" << current << endl;
			files.push_back(current);
		}
	}

	//files.push_back("inputBinary1");
	//files.push_back("inputBinary2");

	if (linkable == true && hex == true)
	{
		cout << "both linkable and hex options are not allowed" << endl;

	}
	if (linkable == false && hex == false)
	{
		cout << "At least one option from -linkable or -hex must be selected" << endl;

	}

	Linker linker(outputFileName);



	if (linkable)
		linker.mode = 1;
	else if (hex)
		linker.mode = 0;

	//sortuj PLACE OBAVEZNO

	//string s = "500";

	//placeSections.push_back(pair<string, int>("isr", stoi(s, 0, 16)));

	sort(placeSections.begin(), placeSections.end(), [](auto& left, auto& right) {
		return left.second < right.second;
		});

	linker.placeSections = placeSections;
	//linker.readBinaryFile("");

	linker.readAllBinaryFiles(files);
	linker.makeSectionHelper();
	linker.mergeSections();
	linker.mergeRelocations();
	linker.mergeSymbolTable();
	linker.mergeDataSections();
	linker.fixRelocationData();

	linker.print();
	linker.makeTxtFile();
}