#ifndef COMPUTATO_H
#define COMPUTATO_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <time.h>

class Variable;
class Method;
class Class;
class Parser;

class Variable
{
public:
	std::string key;
	std::string type;
	std::string value;
	Class * linked_class;
	std::vector<Variable *> arr;
	std::vector<int> dim;

	Variable();
	Variable(std::string k, std::string t);
};

class Method
{
public:
	std::map<std::string, std::string> arguements;
	std::string body;
	std::string key;
	std::string type;
	Class * parent = NULL;

	Method(Class * c, std::string s);
};

class Class
{
public:
	std::string key;
	std::map<std::string, Method *> methods;
	std::map<std::string, Variable *> globals;
	std::string body;

	Class(std::string s);
};

class Parser
{
private:
	std::map<std::string, Class *> source;
	bool is_number(const std::string& s);
	std::vector<std::string> split(std::string s, std::string d = " ");
	Variable parse(std::string s, Class * cl, std::map<std::string, Variable *> &vars);
	Variable parse(Method *m, Class * cl, std::map<std::string, Variable *> args);
	std::vector<std::string> line_split(std::string s);
	Class * Delineate(std::string s);
	Method * ParseMethod(std::string s, Class * slavus);
	void loadClassFile(std::string filename);
public:
	Parser(std::string filename);
	void run();
	~Parser();
};

#endif // !PARSER_H
