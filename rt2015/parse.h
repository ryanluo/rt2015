typedef struct Flag_ Flag;


#ifndef PARSE_H
#define PARSE_H


#include "common.h"
#include <iostream>

class ShapeGroup;
class RayFile;

using namespace std;

/*
* Use these methods to get doubles, ints, and char*s off of the istream
* ignoring whitespace and comments.
*/
double	nextDouble (istream& in);
int	nextInt (istream& in);
char*	nextToken (istream& in, char* token, int size);
char*   peekToken (istream& in, char* token, int size);
void    chompToken(istream& in);


/*
* To add the ability to parse additional shape directives, like
* #cone or #cylinder, modify this method to handle them appropriately
*/
ShapeGroup*	parseShapeGroup (istream& in, char* token, int size, RayFile *file);


/*
* Structure to encapsulate the necessary information for a flag
* of the "-flag arg" style.  Flags can take zero or one argument, of type
* int, double, or string, and can only occur once.  Flag parsing will
* stop when either the stream fails or a "--" is encountered.  For
* flags of type BOOL, no argument is read - the val is set to true
* if the flag exists, and false otherwise.
*
* Symbol is the entire string flag to search for, such as
* "-l", "-name" or "--version".  the first character should be a '-'
*
* Valid types are STRING, INT, DOUBLE, or BOOL, as per the FlagType enum
*
* Set required to true if the flag must appear in the stream.  If
* required is true and the flag is not found, parseFlags will print
* an error and return -1.
*
* size should be the size of the variable to put the value into,
* sizeof(int), sizeof(double) or the number of bytes in the char*
* If type is BOOL, size should be sizeof(bool).
*
* val should contain a pointer to the variable to store the flag's
* argument in.
*
* When parseFlags returns, num will store the number of flags parsed.
* parseFlags returns 0 on success and -1 on parse error.
*/
enum FlagType
{
	STRING,
	INT,
	DOUBLE,
	BOOL
};

struct Flag_
{
	char*		symbol;
	FlagType	type;
	bool		required;
	int			size;
	void*		val;
};

int	parseFlags (istream& in, Flag* flags, int& num);


#endif // PARSE_H
