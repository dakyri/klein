%{
/* C++ string header, for string ops below */
#include <string>
/* Implementation of yyFlexScanner */ 
#include "KLFunLexer.h"

/* typedef to make the returns for the tokens shorter */
typedef KLF::KLFunParser::token token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

// ** include files here

%}
 
%option debug 
%option nodefault 
%option yyclass="KLFunLexer" 
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

"on" { return token::ON; }
"if" { return token::IF; }
"else" { return token::ELSE; }
"int" { yylval->intval = KLFType::T_INTEGER; return token::TYPE; }
"float" { yylval->intval = KLFType::T_FLOAT; return token::TYPE; }
"bool" { yylval->intval = KLFType::T_BOOLEAN; return token::TYPE; }

"true" { yylval->intval = 1; return token::LITERAL_INT; }
"false" { yylval->intval = 0; return token::LITERAL_INT; }

"click" { yylval->intval = KLFEvent::CLICK; return token::EVENT; }
"endClick" { yylval->intval = KLFEvent::END_CLICK; return token::EVENT; }
"sustain" { yylval->intval = KLFEvent::SUSTAIN; return token::EVENT; }
"endSustain" { yylval->intval = KLFEvent::END_SUSTAIN; return token::EVENT; }

{IDENT} {
	Command *cmd = Command::find(yytext);
	if (cmd != nullptr) {
		yylval->command = cmd;
		return token::COMMAND;
	}
	Control *ctrl = Control::find(yytext);
	if (ctrl != nullptr) {
		yylval->control = ctrl;
		return token::CONTROL;
	}
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
"*" { return token::MULT; }
"/" { return token::DIVIDE; }
"%" { return token::MOD; }

"<" { return token::LT; }
">" { return token::GT; }
"<=" { return token::LE; }
">=" { return token::GE; }
"==" { return token::EQ; }
"!=" { return token::NE; }

"&&" { return token::AND; }
"||" { return token::OR; }
"!" { return token::NOT; }


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


