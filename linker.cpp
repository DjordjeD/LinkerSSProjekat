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

Linker::Linker(string outputfile)
{
	outputFile = outputfile;
}

void Linker::readAllBinaryFiles(vector<string> fileNames)
{
	for (auto& i : fileNames)
	{
		readBinaryFile(i);
	}
}

void Linker::readBinaryFile(string fileName)
{
	ifstream inputFile(fileName, ios::out | ios::binary);

	//fileName = "inputBinary";

	unsigned int stringLenght;
	int intValue;
	bool boolValue;

	string s1, s2, s3;
	int i1, i2, i3, i4;
	bool b1, b2;
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

		inputFile.read((char*)&b2, sizeof(b2));

		//std::cout << s1 << "\t" << i1 << "\t" << s2 << "\t" << i2 << "\t" << b1 << "\t" << s3 << endl;

		Symbol tempSymbol;
		tempSymbol.symbolName = s1;
		tempSymbol.value = i1;
		tempSymbol.symbolScope = s2;
		tempSymbol.numberID = i2;
		tempSymbol.symbolSection = s3;
		tempSymbol.isSymbol = b2;
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
		temp.fileName = fileName;
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

	//printSectionMap(smallMap, allRelocationRecords[fileName]);

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
			/*if (j.second.getSectionName() == "UNDEFINED" || j.second.getSectionName() == "ABSOLUTE") cout << "absolute or undefined" << endl;
			else*/ sectionSet.insert(j.first);
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
						temp.size = i.second.getSectionSize(); //velicina sekcije
						if (i.first != "UNDEFINED" && i.first != "ABSOLUTE") {
							temp.leftBound = sectionSizeCounter; //velicina levog virtuelna memorija
							temp.rightBound = temp.leftBound + temp.size;

							sectionSizeCounter += temp.size; //pomeri ga na desno
						}
						sectionHelpVector.push_back(temp);
						break;
					}
				}
			}
		}

		//sectionsizeCounter ce ostati na desnoj velicini tj znace se gde pocinje sledeca

		int flag = 0;
		for (auto& section : sectionSet)
		{
			for (auto& i : placeSections)
			{
				if (i.first == section) flag = 1;
			}

			if (flag == 1) {
				flag = 0; continue;
			}
			for (auto& j : allSectionMaps)// kroz fajlove
			{

				for (auto& i : j.second)//kroz mapu sekcija 
				{

					if (i.first == section)
					{
						SectionHelp temp;

						temp.fileName = j.first;// ime fajla
						temp.sectionName = section; //ime sekcije
						temp.size = i.second.getSectionSize(); //velicina sekcije
						if (i.first != "UNDEFINED" && i.first != "ABSOLUTE") {
							temp.leftBound = sectionSizeCounter; //velicina levog virtuelna memorija
							temp.rightBound = temp.leftBound + temp.size;

							sectionSizeCounter += temp.size; //pomeri ga na desno
						}
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
						temp.size = i.second.getSectionSize(); //velicina sekcije
						if (i.first != "UNDEFINED" && i.first != "ABSOLUTE") {
							temp.leftBound = sectionSizeCounter; //velicina levog virtuelna memorija
							temp.rightBound = temp.leftBound + temp.size;

							sectionSizeCounter += temp.size; //pomeri ga na desno
						}

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
				if (cnt++ == 0 && section != "ABSOLUTE" && section != "UNDEFINED") temp.virtualAddress = sectionHelper.leftBound; // daj mu virtuelnu adresu najprvljeg 
				if (section != "ABSOLUTE" && section != "UNDEFINED")temp.increaseSize(sectionHelper.size);
			}
		}
		cnt = 0;
		outputSectionList.push_back(temp);
	}


}

void Linker::mergeSymbolTable()
{
	for (auto& i : allSymbolTables)
	{
		for (auto& symbol : i.second)
		{
			symbolSet.insert(symbol.symbolName);

		}
	}
	//TREBA RESITI SLUCAJ AKO JE SEKCIJA I UBACITI SEKCIJE !!!!!!

	int symDefNumber = 0;

	for (auto& symbolName : symbolSet)
	{

		for (auto& i : allSymbolTables)//trci kroz sve tabele
		{
			for (auto& currSymbol : i.second) //trci kroz vektor simbola ALI U JEDNOM FAJLU
			{
				if (currSymbol.isSymbol) {

					if (symbolName == currSymbol.symbolName) // ako si nabo simbol 
					{
						if (currSymbol.symbolSection != "UNDEFINED") symDefNumber++;
						else break; // valjda
						if (symDefNumber == 2) cout << "visestruko definisani simbol" << currSymbol.symbolName << endl;

						for (auto& vectorCurr : sectionHelpVector)
						{
							if (vectorCurr.fileName == i.first && vectorCurr.sectionName == currSymbol.symbolSection)//nabodi fajl
							{
								if (currSymbol.symbolSection != "ABSOLUTE") {
									currSymbol.value = vectorCurr.leftBound + currSymbol.value;// proveriti ovo
								}
								outputSymbolTable.push_back(currSymbol);


							}
						}

					}
				}
				else {
					//dodaj sekcije
					if (symbolName == currSymbol.symbolName) {
						bool exists = 0;
						for (auto& i : outputSymbolTable)
						{
							if (i.symbolName == symbolName) exists = 1;
						}
						if (!exists)
						{
							for (auto& i : outputSectionList)
							{
								if (currSymbol.symbolSection == i.getSectionName())
								{
									currSymbol.value = i.virtualAddress;
								}
							}
							outputSymbolTable.push_back(currSymbol);
						}
					}
				}
			}
		}

		symDefNumber = 0;
		//outputSymbolTable.push
	}


}

void Linker::mergeRelocations()
{

	for (auto& relocationRecordMap : allRelocationRecords)//trci kroz fajlove
	{
		for (auto& relocationRecord : relocationRecordMap.second)
		{
			for (auto& helper : sectionHelpVector)
			{
				if (relocationRecord.sectionName == helper.sectionName && relocationRecordMap.first == relocationRecord.fileName)
				{
					//if (mode == 1) //linkable mod moze odmah da  
					//	relocationRecord.offset += helper.leftBound;
					//else {

					//	int VA;

					//	for (auto& i : outputSectionList)
					//	{
					//		if (i.getSectionName() == relocationRecord.sectionName)
					//		{
					//			VA = i.virtualAddress; // uzmi virtuelnu adresu
					//		}
					//	}

					//	relocationRecord.offset = relocationRecord.offset + helper.leftBound - VA;
					//	if (relocationRecord.offset < 0) cout << "greska offset u minusu" << endl;
					relocationRecord.offset += helper.leftBound;
					outputRelocationTable.push_back(relocationRecord);
				}

			}
		}
	}
}

void Linker::mergeDataSections()//zamislio sam da ih spoji sve 
{
	for (auto& fileName : allSectionMaps)// kroz fajlove
	{
		for (auto& smallSectionMap : fileName.second)// kroz malu mapu
		{
			if (smallSectionMap.second.getSectionName() == "UNDEFINED" || smallSectionMap.second.getSectionName() == "ABSOLUTE" || smallSectionMap.second.getSectionSize() == 0) continue;

			int increaseOffset = 0;
			for (auto& section : sectionHelpVector)
			{
				if (smallSectionMap.second.getSectionName() == section.sectionName && section.fileName == fileName.first)
				{
					increaseOffset = section.leftBound; // vidi za koliko  treba da se poveca offset
				}
			}

			for (auto& i : smallSectionMap.second.offsets)//svaki offset povecaj
			{

				i += increaseOffset;
			}
			outputSectionMap[smallSectionMap.first] = smallSectionMap.second; // ubaci u konacu
		}

	}

	for (auto& i : outputSectionMap)//ubaci virtuelne adrese
	{
		for (auto& j : outputSectionList)
		{
			if (i.first == j.getSectionName())
			{
				i.second.virtualAddress = j.virtualAddress;
			}
		}
	}


}

void Linker::fixRelocationData()
{
	for (auto& relocationData : outputRelocationTable)
	{
		int findOffset = relocationData.offset;

		for (auto& i : outputSectionMap)
		{
			if (i.first == relocationData.sectionName)
			{
				int flag = 0;
				int row = 0;
				int first = 0;
				int second = 1;
				for (size_t j = 0; j < i.second.offsets.size(); j++) //j iterira po redovima jer isto ima offseta i redova
				{
					//cout << hex << setfill('0') << setw(4) << i.second.offsets.at(j) << " : " << "\t";



					if (i.second.offsets.at(j) == findOffset)
					{
						row = j;
						flag = 1;
						break;
					}


				}
				if (flag == 0) {
					for (size_t j = 0; j < i.second.offsets.size(); j++) //j iterira po redovima jer isto ima offseta i redova
					{
						if (i.second.offsets.at(j) < findOffset && findOffset < (j + 1 < i.second.offsets.size() ? i.second.offsets.at(j + 1) : i.second.offsets.at(j)))
						{
							row = j;
							int lineBegin = i.second.offsets.at(j);
							first = findOffset - lineBegin - 1;
							second = findOffset - lineBegin;
							//flag = 2;
							break;
						}
					}
				}
				int value;
				//izvuci value 

				if (relocationData.isData)
				{

					value = (int)((i.second.data[row][second] << 8) + (0xff & i.second.data[row][first]));
				}
				else
				{
					value = (int)((i.second.data[row][first] << 8) + (0xff & i.second.data[row][second]));
				}


				bool isSymbol = true;
				for (auto& i : sectionSet)
				{
					if (i == relocationData.symbolName) isSymbol = false;
				}

				if (relocationData.relocationType == "R_HYP_16") //apsolutno
				{

					if (isSymbol) {
						for (auto& symbol : outputSymbolTable)
						{
							if (symbol.symbolName == relocationData.symbolName)
							{
								value = value + symbol.value;
								//vrati data nazad;

							}
						}
						if (relocationData.isData)
						{
							i.second.data[row][first] = (0xff & (value));
							i.second.data[row][second] = (0xff & (value >> 8));
						}
						else {
							i.second.data[row][second] = (0xff & (value));
							i.second.data[row][first] = (0xff & (value >> 8));
						}
					}
					else {

						for (auto& i : sectionHelpVector)
						{
							if (i.fileName == relocationData.fileName && i.sectionName == relocationData.symbolName)
							{
								value = value + i.leftBound;
								//varti value

							}
						}

						if (relocationData.isData)
						{
							i.second.data[row][first] = (0xff & (value));
							i.second.data[row][second] = (0xff & (value >> 8));
						}
						else {
							i.second.data[row][second] = (0xff & (value));
							i.second.data[row][first] = (0xff & (value >> 8));
						}
					}
					break;
				}
				else if (relocationData.relocationType == "R_HYP_16_PC") //pcrelative
				{


					// ako je simbol
					if (isSymbol) {
						for (auto& symbol : outputSymbolTable)
						{
							if (symbol.symbolName == relocationData.symbolName)
							{
								value = value + symbol.value - relocationData.offset;// zbog big endian

								//vrati data nazad;

							}
						}
						if (relocationData.isData)
						{
							i.second.data[row][first] = (0xff & (value));
							i.second.data[row][second] = (0xff & (value >> 8));
						}
						else {
							value += 1;
							i.second.data[row][second] = (0xff & (value));
							i.second.data[row][first] = (0xff & (value >> 8));
						}

					}
					else {


						for (auto& i : sectionHelpVector)
						{
							if (i.fileName == relocationData.fileName && i.sectionName == relocationData.symbolName)
							{
								value = value + i.leftBound - relocationData.offset;
								//varti value

							}
						}
						if (relocationData.isData)
						{
							i.second.data[row][first] = (0xff & (value));
							i.second.data[row][second] = (0xff & (value >> 8));
						}
						else {
							value++;
							i.second.data[row][second] = (0xff & (value));
							i.second.data[row][first] = (0xff & (value >> 8));
						}
					}
					break;
				}






			}
		}
	}

}

void Linker::print()
{
	cout << "SECTION helper" << endl;
	for (auto& i : sectionHelpVector)
	{
		cout << i.fileName << "\t" << i.sectionName << "\t" << hex << i.size << "\t" << "[" << i.leftBound << ", " << i.rightBound << ")" << endl;
	}
	cout << endl;
	printSectionMap(outputSectionMap, outputRelocationTable);
	printSectionList();
	printSymbolTable();
}

void Linker::printSectionMap(map<string, Section> sectionMap, vector <RelocationRecord> relocationTable)
{
	cout << endl;
	int offsetCnt = 0;
	for (auto& i : sectionMap)
	{
		cout << "Section data of " << i.first << " :" << endl;

		int cnt = 1;
		offsetCnt = i.second.virtualAddress;
		cout << hex << setfill('0') << setw(4) << offsetCnt << " : " << "\t";
		for (auto& i : i.second.data)
		{
			for (auto& j : i)
			{
				if (cnt++ <= 8)
				{
					cout << hex << setfill('0') << setw(2) << (0xff & j) << " ";
					offsetCnt++;
				}
				else
				{
					cout << endl;
					cnt = 2;
					cout << hex << setfill('0') << setw(4) << offsetCnt++ << " : " << "\t";
					cout << hex << setfill('0') << setw(2) << (0xff & j) << " ";

				}
			}
		}

		cout << endl;

	}

	cout << endl << "LINKABLE ISPIS" << endl;
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

	}

	cout << "Relocation table " << endl;
	cout << "Offset " << "\t" << "IsData" << "\t" << "relocationType" << "\t" << "symbolName(value)" << "\t" << "sectionName" << endl;
	for (auto& j : relocationTable)
	{
		cout << hex << j.offset << "\t" << j.isData << "\t" << j.relocationType << "\t" << j.symbolName << "\t\t" << j.sectionName << endl;
	}
	cout << endl << endl;
}

void Linker::printSectionList()
{
	cout << "Section LIST " << endl;
	cout << "Section\t Size\t VA" << endl;
	for (auto& i : outputSectionList)
	{
		cout << hex << i.getSectionName() << "\t" << i.getSectionSize() << "\t" << i.virtualAddress << endl;
	}
	cout << endl;
}

void Linker::printSymbolTable()
{

	cout << "Symbol table:" << endl;
	cout << "Value\tType\tSection\t\tName\t\tId" << endl;

	for (auto& i : outputSymbolTable)
	{
		cout << hex << i.value << "\t " << i.symbolScope << "\t " << i.symbolSection << "\t " << i.symbolName << "\t " << i.numberID << endl;
	}
	cout << endl << " END OF SYMBOL TABLE" << endl << endl;

}

void Linker::txtSectionMap(map<string, Section> sectionMap, vector<RelocationRecord> relocationTable)
{
	ofstream outputFile("output.txt");


	outputFile << endl;
	for (auto& i : sectionMap)
	{
		outputFile << "Section data of " << i.first << " :" << endl;

		for (size_t j = 0; j < i.second.offsets.size(); j++) //j iterira po redovima jer isto ima offseta i redova
		{
			outputFile << hex << setfill('0') << setw(4) << i.second.offsets.at(j) << " : " << "\t";

			for (size_t k = 0; k < i.second.data[j].size(); k++)
			{
				outputFile << hex << setfill('0') << setw(2) << (0xff & i.second.data[j][k]) << " ";
			}

			outputFile << endl;
		}

		outputFile << endl;

		outputFile << "Relocation for " << i.first << endl;
		outputFile << "Offset " << "\t" << "IsData" << "\t" << "relocationType" << "\t" << "sectionName" << "\t" << "symbolName(value)" << endl;
		for (auto& j : relocationTable)
		{

			if (i.first == j.sectionName) {


				outputFile << j.offset << "\t" << j.isData << "\t" << j.relocationType << "\t" << j.sectionName << "\t\t" << j.symbolName << endl;
			}
		}
		outputFile << endl << endl;
	}
	outputFile.close();
}

void Linker::makeTxtFile()
{
	string path = outputFile;
	ofstream outputFile(path);

	outputFile << "SECTION helper" << endl;
	for (auto& i : sectionHelpVector)
	{
		outputFile << i.fileName << "\t" << i.sectionName << "\t" << hex << i.size << "\t" << "[" << i.leftBound << ", " << i.rightBound << ")" << endl;
	}
	outputFile << endl;



	outputFile << "Section LIST " << endl;
	outputFile << "Section\t Size\t VA" << endl;
	for (auto& i : outputSectionList)
	{
		outputFile << hex << "\t" << i.getSectionName() << "\t" << i.getSectionSize() << "\t" << i.virtualAddress << endl;
	}


	outputFile << "Symbol table:" << endl;
	outputFile << "Value\tType\tSection\t\tName\t\tId" << endl;

	for (auto& i : outputSymbolTable)
	{
		outputFile << hex << i.value << "\t " << i.symbolScope << "\t " << i.symbolSection << "\t " << i.symbolName << "\t " << i.numberID << endl;
	}
	outputFile << endl << " END OF SYMBOL TABLE" << endl << endl;

	if (this->mode == 1)
		txtSectionMap(outputSectionMap, outputRelocationTable);
	else {


		//outputFile << endl;
		int offsetCnt = 0;
		for (auto& i : outputSectionMap)
		{
			outputFile << "Section data of " << i.first << " :" << endl;

			int cnt = 1;
			offsetCnt = i.second.virtualAddress;
			outputFile << hex << setfill('0') << setw(4) << offsetCnt << " : " << "\t";
			for (auto& i : i.second.data)
			{
				for (auto& j : i)
				{
					if (cnt++ <= 8)
					{
						outputFile << hex << setfill('0') << setw(2) << (0xff & j) << " ";
						offsetCnt++;
					}
					else
					{
						outputFile << endl;
						cnt = 2;
						outputFile << hex << setfill('0') << setw(4) << offsetCnt++ << " : " << "\t";
						outputFile << hex << setfill('0') << setw(2) << (0xff & j) << " ";

					}
				}
			}

			outputFile << endl;

		}


		outputFile << "Relocation table " << endl;
		outputFile << "Offset " << "\t" << "IsData" << "\t" << "relocationType" << "\t" << "symbolName(value)" << "\t" << "sectionName" << endl;
		for (auto& j : outputRelocationTable)
		{
			outputFile << hex << j.offset << "\t" << j.isData << "\t" << j.relocationType << "\t" << j.symbolName << "\t\t" << j.sectionName << endl;
		}
		outputFile << endl << endl;

	}


	outputFile.close();
}

