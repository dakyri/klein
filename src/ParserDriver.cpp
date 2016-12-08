#include <cctype>
#include <iostream>
#include <cassert>

#include "ParserDriver.h"

using namespace std;

MSParse::ParserDriver::ParserDriver()
	: scanner(&cin)
	, parser(scanner, *this) {
}

MSParse::ParserDriver::~ParserDriver() {
}

void 
MSParse::ParserDriver::parse(std::istream& inStream)
{
	/*
	vstplugins.clear();
	schedulables.clear();
	lambdas.clear();
	*/
	scanner.yyrestart(&inStream);
	const int accept( 0 );
	if( parser.parse() != accept ) {
		std::cerr << "Oops! parser failed and the error recovery is awful\n";
	}
}

std::ostream& 
MSParse::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}
