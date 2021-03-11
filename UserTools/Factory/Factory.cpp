#include "Factory.h"

Tool* Factory(std::string tool) {
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="Emergency") ret=new Emergency;
if (tool=="Poll") ret=new Poll;
if (tool=="Recieve") ret=new Recieve;
if (tool=="Set") ret=new Set;
if (tool=="Stream") ret=new Stream;
return ret;
}
