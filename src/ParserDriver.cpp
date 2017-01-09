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
KLF::ParserDriver::parse(KLFun *klf, std::istream& inStream, string _name, vector<string> &errorList)
{
	if (klf == nullptr) {
		errorList.push_back("NUll function given to store script|");
		return ERR_ERROR;
	}
	if (klf->clear()) {
		errorList.push_back("NUll function given to store script|");
		return ERR_ERROR;
	}

	fun = klf;
	name = _name;
	clickTime = 0;
	sustainTime = 0;
	errors = &errorList;

	scanner.yyrestart(&inStream);
	const int accept( 0 );
	if( parser.parse() != accept ) {
		errorList.push_back("Oops! parser failed and the error recovery is awful");
		postParseCleanup();
		return ERR_ERROR;
	}
	postParseCleanup();
	return ERR_OK;
}

std::ostream& 
KLF::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}

/**
 * append an error to the caller-provided list of error messages to report
 */
void KLF::ParserDriver::addErrorMessage(const string &s)
{
	if (errors != nullptr) {
		errors->push_back(s);
	}
}

/**
 * any necessary post-parse cleanup
 */
void KLF::ParserDriver::postParseCleanup()
{
	errors = nullptr;
}
