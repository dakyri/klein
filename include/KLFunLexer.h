#ifndef __MCSCANNER_HPP__
#define __MCSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef  YY_DECL
#define YY_DECL int  KLF::KLFunLexer::yylex()

#include "KLFunParser.hpp"

namespace KLF {

	class KLFunLexer : public yyFlexLexer{
	public:

		KLFunLexer(std::istream *in) : yyFlexLexer(in),  yylval( nullptr ){};

		int yylex(KLF::KLFunParser::semantic_type *lval)
		{
			yylval = lval;
			return( yylex() );
		}


	private:
		int yylex();
		// yyval ptr 
		KLF::KLFunParser::semantic_type *yylval;
	};

}

#endif /* END __MCSCANNER_HPP__ */
