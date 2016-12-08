#ifndef __MCSCANNER_HPP__
#define __MCSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef  YY_DECL
#define YY_DECL int  MSParse::MScriptLexer::yylex()

#include "MScriptParser.hpp"

namespace MSParse {

	class MScriptLexer : public yyFlexLexer{
	public:

		MScriptLexer(std::istream *in) : yyFlexLexer(in),  yylval( nullptr ){};

		int yylex(MSParse::MScriptParser::semantic_type *lval)
		{
			yylval = lval;
			return( yylex() );
		}


	private:
		int yylex();
		// yyval ptr 
		MSParse::MScriptParser::semantic_type *yylval;
	};

}

#endif /* END __MCSCANNER_HPP__ */
