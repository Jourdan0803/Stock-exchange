#ifndef PARSER_HPP
#define PARSER_HPP
#include "xml_parser/pugixml.hpp"
#include "database_util.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <cctype> 
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;
using namespace pugi;
string parse_create(xml_document &doc,connection *c);
string parse_transaction(xml_document &doc,connection *c);
string handleRequest(string req,connection *c);
#endif