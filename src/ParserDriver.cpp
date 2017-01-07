#include <cctype>
#include <iostream>
#include <cassert>

#include "ParserDriver.h"

using namespace std;

KLF::ParserDriver::ParserDriver()
	: scanner(&cin)
	, parser(scanner, *this)
	, fun(nullptr) {
}

KLF::ParserDriver::~ParserDriver() {
}

status_t
KLF::ParserDriver::parse(KLFun *klf, std::istream& inStream, vector<string> &errorList)
{
	/*
	vstplugins.clear();
	schedulables.clear();
	lambdas.clear();
	*/
	if (klf == nullptr) {
		errorList.push_back("NUll function given to store script|");
		return ERR_ERROR;
	}
	if (klf->clear()) {
		errorList.push_back("NUll function given to store script|");
		return ERR_ERROR;
	}
	scanner.yyrestart(&inStream);
	const int accept( 0 );
	if( parser.parse() != accept ) {
		errorList.push_back("Oops! parser failed and the error recovery is awful");
		return ERR_ERROR;
	}
	return ERR_OK;
}

std::ostream& 
KLF::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}
