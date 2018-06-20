#ifndef CPPWEBSERVER_UTILITIES_H
#define CPPWEBSERVER_UTILITIES_H

#include "sqlite3.h"

bool PrintFile(const char *filename);
sqlite3 *openMessageBoard();

#endif // CPPWEBSERVER_UTILITIES_H
