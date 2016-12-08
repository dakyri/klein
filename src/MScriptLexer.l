%{
/* C++ string header, for string ops below */
#include <string>
/* Implementation of yyFlexScanner */ 
#include "MScriptLexer.h"

/* typedef to make the returns for the tokens shorter */
typedef MSParse::MScriptParser::token token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

// ** include files here

%}
 
%option debug 
%option nodefault 
%option yyclass="MSciptLexer" 
%option noyywrap 
%option yylineno
%option c++

%x C_COMMENT

DIGIT [0-9]
INTEGER {DIGIT}+
EXPONENT [eE][+-]?{INTEGER}
ALF	[a-zA-Z_]
ALNUM [a-zA-Z0-9_]
IDENT {ALF}{ALNUM}*
REAL {INTEGER}("."{INTEGER})?{EXPONENT}?

%%

[+-]?{INTEGER} { 
	yylval->intval = atof(yytext);
	return token::LITERAL_INT;     
}

[+-]?{REAL} {
	yylval->floatval = atof(yytext);
	return token::LITERAL_FLOAT;
}

{INTEGER}":"{INTEGER}"."{INTEGER} {
/* TODO XXX FIXME ??? best implementation for this may be as a triplet
 */
//	TypedValue *v = new TypedValue();
//	yylval->typedval = v;
	return token::LITERAL_TIME;
}

\"([^\\\"]|\\.)*\" {
	/*matches string-literal on a single line*/;
	yylval->stringval = new std::string(yytext+1, strlen(yytext)-1);
	return token::LITERAL_STRING;
}

"qua" { return token::QUA; }
"voice" { return token::VOICE; }
"pool" { return token::POOL; }
"sample" { return token::SAMPLE; }
"channel" { return token::CHANNEL; }
"function" { return token::LAMBDA; }
"lambda" { return token::LAMBDA; }
"struct" { return token::STRUCT; }
"input" { return token::INPUT; }
"output" { return token::OUTPUT; }
"take" { return token::TAKE; }
"clip" { return token::CLIP; }
"vst" { return token::VST; }
"vstparam" { return token::VST; }

{IDENT} {
/*
	int type_id = findType(yytext);
	if (type_id != TypedValue::S_UNKNOWN) {
		yylval->intval = 0;
		return token::TYPE;
	}
	type_id = findDfltEventType(yytext);
	if (type_id != TypedValue::S_UNKNOWN) {
		yylval->inval = 0;
		return token::EVENT;
	}*/
	yylval->stringval = new std::string(yytext);
	return token::IDENT;
}

"=" { return token::ASSGN; }

"[" { return token::LSQB; }
"]" { return token::RSQB; }
"{" { return token::LBRA; }
"}" { return token::RBRA; }
"(" { return token::LB; }
")" { return token::RB; }

"," { return token::COMMA; }
":" { return token::COLON; }
"::" { return token::COLON2; }
";" { return token::SEMI; }
"." { return token::DOT; }

"+" { return token::PLUS; }
"-" { return token::MINUS; }
"*" { return token::STAR; }
"/" { return token::SLASH; }



[ \t] {
	// skip white space
}

\n { 
	// skip white space, but perhaps also give an interactive prompt
}

. { 
	; // oops      
}

"/*" 			{ BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }


%%


