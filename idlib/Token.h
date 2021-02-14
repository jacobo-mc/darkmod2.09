/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod (http://www.thedarkmod.com/)
 
******************************************************************************/

#ifndef __TOKEN_H__
#define __TOKEN_H__

/*
===============================================================================

	idToken is a token read from a file or memory with idLexer or idParser

===============================================================================
*/

// token types
#define TT_STRING					1		// string
#define TT_LITERAL					2		// literal
#define TT_NUMBER					3		// number
#define TT_NAME						4		// name
#define TT_PUNCTUATION				5		// punctuation

// number sub types
#define TT_INTEGER					0x00001		// integer
#define TT_DECIMAL					0x00002		// decimal number
#define TT_HEX						0x00004		// hexadecimal number
#define TT_OCTAL					0x00008		// octal number
#define TT_BINARY					0x00010		// binary number
#define TT_LONG						0x00020		// long int
#define TT_UNSIGNED					0x00040		// unsigned int
#define TT_FLOAT					0x00080		// floating point number
#define TT_SINGLE_PRECISION			0x00100		// float
#define TT_DOUBLE_PRECISION			0x00200		// double
#define TT_EXTENDED_PRECISION		0x00400		// long double
#define TT_INFINITE					0x00800		// infinite 1.#INF
#define TT_INDEFINITE				0x01000		// indefinite 1.#IND
#define TT_NAN						0x02000		// NaN
#define TT_IPADDRESS				0x04000		// ip address
#define TT_IPPORT					0x08000		// ip port
#define TT_VALUESVALID				0x10000		// set if intvalue and floatvalue are valid

// string sub type is the length of the string
// literal sub type is the ASCII code
// punctuation sub type is the punctuation id
// name sub type is the length of the name

class idToken : public idStr {

	friend class idParser;
	friend class idLexer;

public:
	int				type;								// token type
	int				subtype;							// token sub type
	int				line;								// line in script the token was on
	int				linesCrossed;						// number of lines crossed in white space before token
	int				flags;								// token flags, used for recursive defines

public:
					idToken( void );
					idToken( const idToken *token );
					~idToken( void );

	void			operator=( const idStr& text );
	void			operator=( const char *text );

	double			GetDoubleValue( void );				// double value of TT_NUMBER
	float			GetFloatValue( void );				// float value of TT_NUMBER
	unsigned int	GetUnsignedIntValue( void );		// unsigned int value of TT_NUMBER
	int				GetIntValue( void );				// int value of TT_NUMBER
	int				WhiteSpaceBeforeToken( void ) const;// returns length of whitespace before token
	void			ClearTokenWhiteSpace( void );		// forget whitespace before token

	void			NumberValue( void );				// calculate values for a TT_NUMBER

private:
	unsigned int	intvalue;							// integer value
	double			floatvalue;							// floating point value
	const char *	whiteSpaceStart_p;					// start of white space before token, only used by idLexer
	const char *	whiteSpaceEnd_p;					// end of white space before token, only used by idLexer
	idToken *		next;								// next token in chain, only used by idParser

	void			AppendDirty( const char a );		// append character without adding trailing zero
};

ID_INLINE idToken::idToken( void ) {
}

ID_INLINE idToken::idToken( const idToken *token ) {
	*this = *token;
}

ID_INLINE idToken::~idToken( void ) {
}

ID_INLINE void idToken::operator=( const char *text) {
	*static_cast<idStr *>(this) = text;
}

ID_INLINE void idToken::operator=( const idStr& text ) {
	*static_cast<idStr *>(this) = text;
}

ID_INLINE double idToken::GetDoubleValue( void ) {
	if ( type != TT_NUMBER ) {
		return 0.0;
	}
	if ( !(subtype & TT_VALUESVALID) ) {
		NumberValue();
	}
	return floatvalue;
}

ID_INLINE float idToken::GetFloatValue( void ) {
	return (float) GetDoubleValue();
}

ID_INLINE unsigned int	idToken::GetUnsignedIntValue( void ) {
    if ( type != TT_NUMBER ) {
		return 0;
	}
	if ( !(subtype & TT_VALUESVALID) ) {
		NumberValue();
	}
	return intvalue;
}

ID_INLINE int idToken::GetIntValue( void ) {
    return (int)GetUnsignedIntValue();
}

ID_INLINE int idToken::WhiteSpaceBeforeToken( void ) const {
	return ( whiteSpaceEnd_p > whiteSpaceStart_p );
}

ID_INLINE void idToken::AppendDirty( const char a ) {
	EnsureAlloced( len + 2, true );
	data[len++] = a;
}

#endif /* !__TOKEN_H__ */