// Linker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "linker.h"
#include <set>

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

void Linker::readAllBinaryFiles(vector<string> fileNames)
{
	for (auto& i : fileNames)
	{
		readBinaryFile(i);
	}
}

void Linker::readBinaryFile(string fileName)
{
	ifstream inputFile("inputBinary", ios::out | ios::binary);

	fileName = "inputBinary";

	unsigned int stringLenght;
	int intValue;
	bool boolValue;

	string s1, s2, s3;
	int i1, i2, i3, i4;
	bool b1;
	int symbolTableSize;
	inputFile.read((char*)&symbolTableSize, sizeof(symbolTableSize)); //velicina tablee sim

	for (size_t i = 0; i < symbolTableSize; i++)
	{

		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //ime
		s1.resize(stringLenght);
		inputFile.read((char*)s1.c_str(), stringLenght);

		inputFile.read((char*)&i1, sizeof(i1)); //value

		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //scope
		s2.resize(stringLenght);
		inputFile.read((char*)s2.c_str(), stringLenght);

		inputFile.read((char*)&i2, sizeof(i2)); //numberID

		inputFile.read((char*)&b1, sizeof(b1)); //isdefined

		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //section
		s3.resize(stringLenght);
		inputFile.read((char*)s3.c_str(), stringLenght);

		std::cout << s1 << "\t" << i1 << "\t" << s2 << "\t" << i2 << "\t" << b1 << "\t" << s3 << endl;

		Symbol tempSymbol;
		tempSymbol.symbolName = s1;
		tempSymbol.value = i1;
		tempSymbol.symbolScope = s2;
		tempSymbol.numberID = i2;
		tempSymbol.symbolSection = s1;

		allSymbolTables[fileName].push_back(tempSymbol);
	}

	int sizeRelocation;
	inputFile.read((char*)&sizeRelocation, sizeof(sizeRelocation));//


	for (size_t i = 0; i < sizeRelocation; i++)
	{
		RelocationRecord temp;
		//string tempString;
		inputFile.read((char*)&(temp.isData), sizeof(temp.isData));
		inputFile.read((char*)&(temp.offset), sizeof(temp.offset));


		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //ime
		temp.sectionName.resize(stringLenght);
		inputFile.read((char*)temp.sectionName.c_str(), stringLenght);

		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //ime
		temp.relocationType.resize(stringLenght);
		inputFile.read((char*)temp.relocationType.c_str(), stringLenght);


		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //ime
		temp.symbolName.resize(stringLenght);
		inputFile.read((char*)temp.symbolName.c_str(), stringLenght);

		//std::cout << temp.isData << "\t" << temp.offset << "\t" << temp.sectionName << "\t" << temp.relocationType << "\t" << temp.symbolName << endl;

		allRelocationRecords[fileName].push_back(temp);
	}

	int sizeOfSectionMap;
	inputFile.read((char*)&sizeOfSectionMap, sizeof(sizeOfSectionMap));// velicine mape sekcija

	string sectionName;
	int offsetSize;
	int sectionSize;
	int currOffset;
	int dataVectorSize;

	map<string, Section> smallMap;

	for (size_t i = 0; i < sizeOfSectionMap; i++)
	{
		inputFile.read((char*)&stringLenght, sizeof(stringLenght)); //ime
		sectionName.resize(stringLenght);
		inputFile.read((char*)sectionName.c_str(), stringLenght);

		inputFile.read((char*)&offsetSize, sizeof(offsetSize));

		Section currSection(sectionName);

		//cout << sectionName << "\t";
		for (size_t j = 0; j < offsetSize; j++)
		{
			inputFile.read((char*)&sectionSize, sizeof(sectionSize));
			//cout << sectionSize << "\t";
			currSection.setSectionSize(sectionSize);// daj mu velicinu i ime

			inputFile.read((char*)&currOffset, sizeof(currOffset));

			currSection.offsets.push_back(currOffset); // daj mu offset

			//cout << currOffset << "\t";
			inputFile.read((char*)&dataVectorSize, sizeof(dataVectorSize));

			vector<char> tempVector;
			for (size_t k = 0; k < dataVectorSize; k++)
			{

				inputFile.read((char*)&intValue, sizeof(intValue));
				tempVector.push_back(intValue);
				//cout << intValue << " ";

			}

			currSection.data.push_back(tempVector); // daj mu sav data
		}

		smallMap[sectionName] = currSection;
		//cout << endl;
	}

	printSectionMap(smallMap, allRelocationRecords[fileName]);

	allSectionMaps[fileName] = smallMap;

	inputFile.close();
}

void Linker::makeSectionHelper()
{
	// OVDE MORA DA SE PAZI NA medjubosno sjebavaanje

	for (auto& i : allSectionMaps)
	{

		for (auto& j : i.second)
		{
			if (j.second.getSectionName() == "UNDEFINED" || j.second.getSectionName() == "ABSOLUTE") cout << "absolute or undefined" << endl;
			else sectionSet.insert(j.first);
		}
	}

	int sectionSizeCounter = 0;


	if (mode == 0)//hex mod
	{

		for (auto& sectionName : placeSections)// kroz sekcije
		{

			sectionSizeCounter = sectionName.second; //VM GDE JE STAVLJENA SEKCIJA

			for (auto& j : allSectionMaps)// kroz fajlove
			{
				for (auto& i : j.second)//kroz mapu sekcija 
				{
					if (i.first == sectionName.first)
					{
						SectionHelp temp;

						temp.fileName = j.first;// ime fajla
						temp.sectionName = sectionName.first; //ime sekcije
						temp.leftBound = sectionSizeCounter; //velicina levog virtuelna memorija
						temp.size = i.second.getSectionSize(); //velicina sekcije
						temp.rightBound = temp.leftBound + temp.size;

						sectionSizeCounter += temp.size; //pomeri ga na desno

						sectionHelpVector.push_back(temp);
						break;
					}
				}
			}
		}

		//sectionsizeCounter ce ostati na desnoj velicini tj znace se gde pocinje sledeca

		int flag;
		for (auto& section : sectionSet)
		{
			for (auto& i : placeSections)
			{
				if (i.first == section) flag = 1;
			}

			if (flag == 1) continue;

			for (auto& j : allSectionMaps)// kroz fajlove
			{
				for (auto& i : j.second)//kroz mapu sekcija 
				{
					if (i.first == section)
					{
						SectionHelp temp;

						temp.fileName = j.first;// ime fajla
						temp.sectionName = section; //ime sekcije
						temp.leftBound = sectionSizeCounter; //velicina levog
						temp.size = i.second.getSectionSize(); //velicina sekcije
						temp.rightBound = temp.leftBound + temp.size;

						sectionSizeCounter += temp.size;

						sectionHelpVector.push_back(temp);
						break;
					}
				}
			}

		}



	}
	else
	{

		for (auto& sectionName : sectionSet)// kroz sekcije
		{

			for (auto& j : allSectionMaps)// kroz fajlove
			{
				for (auto& i : j.second)//kroz mapu sekcija 
				{
					if (i.first == sectionName)
					{
						SectionHelp temp;

						temp.fileName = j.first;// ime fajla
						temp.sectionName = sectionName; //ime sekcije
						temp.leftBound = sectionSizeCounter; //velicina levog
						temp.size = i.second.getSectionSize(); //velicina sekcije
						temp.rightBound = temp.leftBound + temp.size;

						sectionSizeCounter += temp.size;

						sectionHelpVector.push_back(temp);
						break;
					}
				}
			}
		}
	}


}

void Linker::mergeSections()
{
	int cnt = 0;
	for (auto& section : sectionSet)
	{
		Section temp(section);
		for (auto& sectionHelper : sectionHelpVector)
		{
			if (section == sectionHelper.sectionName)
			{
				if (cnt++ == 0) temp.virtualAddress = sectionHelper.leftBound; // daj mu virtuelnu adresu najprvljeg 
				temp.increaseSize(sectionHelper.size);
			}
		}
		cnt = 0;
		outputSectionList.push_back(temp);
	}


}


void Linker::printSectionMap(map<string, Section> sectionMap, vector <RelocationRecord> relocationTable)
{
	cout << endl;
	for (auto& i : sectionMap)
	{
		cout << "Section data of " << i.first << " :" << endl;

		for (size_t j = 0; j < i.second.offsets.size(); j++) //j iterira po redovima jer isto ima offseta i redova
		{
			cout << hex << setfill('0') << setw(4) << i.second.offsets.at(j) << " : " << "\t";

			for (size_t k = 0; k < i.second.data[j].size(); k++)
			{
				cout << hex << setfill('0') << setw(2) << (0xff & i.second.data[j][k]) << " ";
			}

			cout << endl;
		}

		cout << endl;

		cout << "Relocation for " << i.first << endl;
		cout << "Offset " << "\t" << "IsData" << "\t" << "relocationType" << "\t" << "sectionName" << "\t" << "symbolName(value)" << endl;
		for (auto& j : relocationTable)
		{

			if (i.first == j.sectionName) {


				cout << j.offset << "\t" << j.isData << "\t" << j.relocationType << "\t" << j.sectionName << "\t\t" << j.symbolName << endl;
			}
		}
		cout << endl << endl;
	}

}

