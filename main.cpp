#include "linker.h"


int main(int argc, char** argv) {

	Linker linker;

	//linker.readAllBinaryFiles();

	//sortuj PLACE OBAVEZNO

	linker.readBinaryFile("");

	linker.makeSectionHelper();
	linker.mergeSections();




}