#include <cctype>
#include <iostream>
#include <cassert>

#include "ParserDriver.h"

using namespace std;

KLF::ParserDriver::ParserDriver()
	: scanner(&cin)
	, parser(scanner, *this) {
}

KLF::ParserDriver::~ParserDriver() {
}

void 
KLF::ParserDriver::parse(std::istream& inStream)
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
KLF::ParserDriver::print( std::ostream &stream )
{
	return(stream);
}
