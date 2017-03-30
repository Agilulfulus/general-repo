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
class Template;
class Parser;

enum VAR_TYPE { NULL_VAR, QUIS, NUMERUS, VERBA, STATUS, GENUS, GAMMA, FUGE, LINKED };

class Variable
{
public:
	union
	{
		std::string value_s;
		int value_f;
		bool value_b;
		VAR_TYPE value_t;
		Class * linked_class;
		std::vector<Variable *> arr;
	};

	std::string key;
	VAR_TYPE type;
	std::vector<int> dim;

	Variable();
	Variable(std::string k, VAR_TYPE t);
	//~Variable();
};

class Method
{
public:
	std::vector<VAR_TYPE> arguement_types;
	std::vector<std::string> arguement_names;
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

class Template
{
public:
	std::string key;
	std::vector<std::string> types;
	std::string body;

	Template(std::string k, std::vector<std::string> t) {
		key = k; types = t;
	}
};

class Parser
{
private:
	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	std::map<std::string, Class *> source;
	std::map<std::string, Template *> templates;
	bool is_number(const std::string& s);
	std::vector<std::string> split(std::string s, std::string d = " ");
	Variable * parse(std::string s, Class * cl, std::map<std::string, Variable *> &vars);
	Variable * parse(Method *m, Class * cl, std::map<std::string, Variable *> args);
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
