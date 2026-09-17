#ifndef __parse_h__
#define __parse_h__

#include <stdbool.h>

#include "command.h"

bool parse_commands(char **line, CommandArray *commands);

#endif