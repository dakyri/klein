#pragma once

enum KLFType {
	INTEGER,
	FLOAT,
	BOOLEAN
};

enum KLFEvent {
	CLICK,
	END_CLICK,
	SUSTAIN,
	END_SUSTAIN
};

/**
 * a piece of KLF
 */
class KBlock {
public:
	KBlock();
	virtual ~KBlock();
};

/**
* a compiled and loaded KLF
*
* should provide
*  - a map for user defined variables which will be allocated when the script is mapped onto a key, control code or other input
*/
class KLFun
{
public:
	KLFun();
	virtual ~KLFun();

protected:
	KBlock *main;
};

