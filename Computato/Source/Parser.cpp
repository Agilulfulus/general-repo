#include "Computato.h"

//VARIABLE FUNCTIONS:

Variable::Variable()
{
	key = "_TEMP_";
	linked_class = NULL;
	type = NULL_VAR;
}

Variable::Variable(std::string k, VAR_TYPE t)
{
	key = k;
	type = t;
}

//METHOD FUNCTIONS:

Method::Method(Class * c, std::string s)
{
	parent = c;
	key = s;
}

//CLASS FUNCTIONS:

Class::Class(std::string s)
{
	key = s;
}

//PARSER FUNCTIONS:

Parser::Parser(std::string filename)
{
	loadClassFile(filename);
}

Parser::~Parser()
{
	source.erase(source.begin(), source.end());
}

void Parser::run()
{
	parse(source["OPERATOR"]->methods["OPERATOR"], source["OPERATOR"], {});
}

bool Parser::is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

Variable * Parser::parse(Method *m, Class * cl, std::map<std::string, Variable *> args)
{
	std::vector<std::string> lines = line_split(m->body);

	Variable * out = new Variable();

	for (int i = 0; i < lines.size() - 1; i++)
	{
		//std::cout << " >> " << lines[i] << std::endl;

		while (lines[i][0] == ' ')
			lines[i] = lines[i].substr(1);
		while (lines[i][lines[i].length() - 1] == ' ')
			lines[i] = lines[i].substr(0, lines[i].length() - 1);

		out = parse(lines[i], cl, args);

		if (out->type != NULL_VAR)
		{
			//cout << "\t>> RETURNING VALUE: (" << out << ")" << endl;
			return out;
		}
	}

	args.erase(args.begin(), args.end());

	return out;
}

Variable * Parser::parse(std::string s, Class * cl, std::map<std::string, Variable *> &vars)
{
	Variable * ob = new Variable();

	std::vector<std::string> comp;

	for (auto const &v : cl->globals)
	{
		vars[v.first] = v.second;
		//cout << "GLOB: " << v.second->value << endl;
	}

	if (s.compare(0, 2, "si") != 0 && s.compare(0, 3, "dum") != 0 && s.compare(0, 5, "inter") != 0 && s.compare(0, 6, "gamma<") != 0 && s[0] != '?')
	{
		comp = split(s);
	}
	else
		comp = { s };

	//std::cout << "\t>> CLASS: " << cl->key << " >> " << s << ", ARGS: " << comp.size() << std::endl;

	if (s == "fuge")
	{
		ob->type = FUGE;
		return ob;
	}

	if (comp[0] == "CONSOLE_OUTPUT")
	{
		std::cout << parse(comp[1], cl, vars)->value_s;
		return ob;
	}
	else if (comp[0] == "CONSOLE_ENDLINE")
	{
		std::cout << std::endl;
		return ob;
	}
	else if (comp[0] == "CONSOLE_INPUT")
	{
		std::string line;
		std::cin >> line;

		ob->value_s = line;

		return ob;
	}
	else if (comp[0] == "GET_RAW_RANDOM")
	{
		ob->value_f = rand();
		ob->type = NUMERUS;

		return ob;
	}
	else if (comp[0] == "SET_RAND_SEED")
	{
		srand(time(0));

		return ob;
	}

	if (comp.size() <= 1)
	{
		int j = 0;
		std::string p;
		while (j < comp[0].length())
		{
			p = std::string(1, comp[0][j]);
			if (p == "<" || p == "~" || p == "|" || p == "!" || p == "%" || p == "@" || p == "=" || p == "-")
				break;
			j++;
		}

		if (p == "~")
		{
			std::string key = comp[0].substr(0, j);
			std::string value = comp[0].substr(key.length() + 1);

			//cout << "\t>> " << key << " : " << value << endl;

			if (vars.find(key) != vars.end())
			{
				//std::cout << vars[key]->linked_class->key;
				ob = parse(value, vars[key]->linked_class, vars);
			}
			else
			{
				ob = parse(value, source[key], vars);
			}

			return ob;

			//cout << "\t>> DONE" << endl;
		}
		else if (p == "<")
		{
			std::string key = comp[0].substr(0, j);

			if (key == "gamma")
			{
				std::string ifstmnt = split(s)[0].substr(6);
				ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);
				//std::cout << "\t>> GAMMA SIZE:\t" << size << std::endl;
				//std::cout << "\t>> GAMMA NAME:\t" << split(s)[1] << std::endl;
				//std::cout << "\t>> GAMMA TYPE:\t" << split(s)[2] << std::endl;

				std::vector<std::string> split_arg = split(s);
				std::vector<std::string> split_var = split(ifstmnt, "&");
				int size = -1;

				Variable * g;
					if (vars[split_arg[1]] == NULL)
						g = new Variable(split_arg[1], GAMMA);
					else
						g = vars[split_arg[1]];

					g->arr.clear();
					g->dim.clear();

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);

					int d = parse(split_var[x], cl, vars)->value_f;

					g->dim.push_back(d);

					if (size == -1)
						size = d;
					else
						size *= d;
				}

				for (int i = 0; i < size; i++)
				{
					Variable * nv = new Variable("sub" + i, parse(split_arg[2], cl, vars)->value_t);
					g->arr.push_back(nv);
				}

				vars[g->key] = g;
				return ob;
			}
			else if (key == "si")
			{
				std::string ifstmnt = split(s, ":")[0].substr(3);
				ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);
				//cout << "\t>> IF STMNT:\t" << ifstmnt << endl;

				std::string iflines = s.substr(split(s, ":")[0].length() + 1);
				iflines = iflines.substr(0, iflines.length() - 1);

				//std::cout << "\t>> IF LINES:\t" << iflines << std::endl;

				std::vector<std::string> runlines = line_split(iflines);

				if (parse(ifstmnt, cl, vars)->value_b == true)
				{
					//cout << "\t>> TRUE - RUNNING IF STATEMENT" << endl;

					for (int l = 0; l < runlines.size(); l++)
					{
						if (runlines[l] != "")
						{
							while (runlines[l][0] == ' ')
								runlines[l] = runlines[l].substr(1);
							while (runlines[l][runlines[l].length() - 1] == ' ')
								runlines[l] = runlines[l].substr(0, runlines[l].length() - 1);

							ob = parse(runlines[l], cl, vars);

							if (ob->type != NULL_VAR)
								return ob;
						}
					}
				}
				return ob;
			}
			else if (key == "dum")
			{
				std::string ifstmnt = split(s, ":")[0].substr(4);
				ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);
				//cout << "\t>> WHILE STMNT:\t" << ifstmnt << endl;

				std::string iflines = s.substr(split(s, ":")[0].length() + 1);
				iflines = iflines.substr(0, iflines.length() - 1);

				//cout << "\t>> WHILE LINES:\t" << iflines << endl;
				bool broke = false;

				while (parse(ifstmnt, cl, vars)->value_b == true && !broke)
				{
					//cout << "\t>> TRUE - RUNNING WHILE LOOP" << endl;

					std::vector<std::string> runlines = line_split(iflines);

					for (int l = 0; l < runlines.size(); l++)
					{
						if (runlines[l] != "")
						{
							while (runlines[l][0] == ' ')
								runlines[l] = runlines[l].substr(1);
							while (runlines[l][runlines[l].length() - 1] == ' ')
								runlines[l] = runlines[l].substr(0, runlines[l].length() - 1);

							ob = parse(runlines[l], cl, vars);

							if (ob->type == FUGE)
							{
								ob->type = NULL_VAR;
								return ob;
							}
							else if (ob->type != NULL_VAR)
							{
								return ob;
							}
						}
					}
				}

				return ob;
			}
			else if (key == "inter")
			{
				std::string ifstmnt = split(s, ":")[0].substr(6);
				ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);
				//cout << "\t>> FOR STMNT:\t" << ifstmnt << endl;

				std::vector<std::string> split_var = split(ifstmnt, "&");

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);
				}

				std::string iflines = s.substr(split(s, ":")[0].length() + 1);
				iflines = iflines.substr(0, iflines.length() - 1);

				//cout << "\t>> FOR LINES:\t" << iflines << endl;

				parse(split_var[0], cl, vars);

				//bool broke = false;

				while (parse(split_var[1], cl, vars)->value_b == true)
				{
					//std::cout << "\t>> TRUE (" << split_var[1] << ") RUNNING WHILE LOOP" << std::endl;

					std::vector<std::string> runlines = line_split(iflines);

					for (int l = 0; l < runlines.size(); l++)
					{
						if (runlines[l] != "")
						{
							while (runlines[l][0] == ' ')
								runlines[l] = runlines[l].substr(1);
							while (runlines[l][runlines[l].length() - 1] == ' ')
								runlines[l] = runlines[l].substr(0, runlines[l].length() - 1);

							ob = parse(runlines[l], cl, vars);

							if (ob->type == FUGE)
							{
								ob->type = NULL_VAR;
								return ob;
							}
							else if (ob->type != NULL_VAR)
							{
								//std::cout << runlines[l] << std::endl;
								//std::cout << "TYPE: " << ob->type << std::endl;
								return ob;
							}
						}
					}

					parse(split_var[2], cl, vars);
				}

				return ob;
			}
			else if (vars.find(key) != vars.end())
			{
				std::string ifstmnt;// = split(s)[0].substr(key.length() + 1);
				std::string remainder;
				//ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);

				int level = 0;
				for (int i = key.length() + 1; i < s.length(); i++)
				{
					std::string token = std::string(1, s[i]);

					if (token == "<")
						level++;
					if (token == ">")
					{
						if (level == 0)
							break;

						level--;
					}

					ifstmnt = ifstmnt + token;
				}

				remainder = s.substr(key.length() + ifstmnt.length() + 2);

				std::vector<std::string> split_var = split(ifstmnt, "&");
				int pos = 0;

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);

					int shift = parse(split_var[x], cl, vars)->value_f;

					for (int i = x + 1; i < vars[key]->dim.size(); i++)
						shift = shift * vars[key]->dim[i];

					pos += shift;
				}

				ob = vars[key]->arr[pos];

				if (vars[key]->arr[pos]->linked_class != NULL)
				{
					if (remainder != "")
					{
						ob = parse(remainder.substr(1), vars[key]->arr[pos]->linked_class, vars);
					}
					//else
						//ob->linked_class = vars[key]->arr[pos]->linked_class;
				}

				return ob;
			}
			else if (templates.find(key.substr(1)) != templates.end())
			{
				key = key.substr(1);
				//std::string classname = key;
				std::string body = templates[key]->body;

				std::string value;

				int level = 0;
				for (int i = key.length() + 2; i < s.length(); i++)
				{
					std::string token = std::string(1, s[i]);

					if (token == "<")
						level++;
					if (token == ">")
					{
						if (level == 0)
							break;

						level--;
					}

					value = value + token;
				}

				std::string var = s.substr(key.length() + value.length() + 4);

				//std::cout << var << std::endl;

				std::vector<std::string> split_var = split(value, "&");

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);

					std::string rep = split_var[x];

					replaceAll(body, templates[key]->types[x], rep);
				}

				Variable * v = new Variable(var, LINKED);
				v->linked_class = Delineate(body);
				vars[v->key] = v;

				return ob;
			}
			else
			{
				std::string value;
				std::string remainder;

				int level = 0;
				for (int i = key.length() + 1; i < s.length(); i++)
				{
					std::string token = std::string(1, s[i]);

					if (token == "<")
						level++;
					if (token == ">")
					{
						if (level == 0)
							break;

						level--;
					}

					value = value + token;
				}

				remainder = s.substr(key.length() + value.length() + 2);

				//cout << "\t>> " << key << " : " << value << " : " << remainder << endl;

				if (key.length() == 0)
					return parse(value, cl, vars);

				//std::cout << "\t>> " << key << " : " << value << std::endl;

				if (cl->methods[key] != NULL)
				{
					Method * m = cl->methods[key];
					std::map<std::string, Variable *> new_vars;
					if (!m->arguement_types.empty())
					{
						std::vector<std::string> split_var = split(value, "&");

						for (int x = 0; x < split_var.size(); x++)
						{
							while (split_var[x][0] == ' ')
								split_var[x] = split_var[x].substr(1);
							while (split_var[x][split_var[x].length() - 1] == ' ')
								split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);
						}

						int k = 0;

						for (int i = 0; i < cl->methods[key]->arguement_types.size(); i++)
						{
							VAR_TYPE val = cl->methods[key]->arguement_types[i];

							//if (cl->globals.find(v.second) != cl->globals.end())
							//	val = cl->globals[v.second]->value_t;

							Variable * oldv = parse(split_var[k], cl, vars);

							if (oldv->type != val && val != QUIS)
								return ob;

							Variable * nv = new Variable(cl->methods[key]->arguement_names[i], oldv->type);
							if (oldv->type == NUMERUS)
								nv->value_f = oldv->value_f;
							else if (oldv->type == STATUS)
								nv->value_b = oldv->value_b;
							else if (oldv->type == VERBA)
								nv->value_s = oldv->value_s;
							else if (oldv->type == GENUS)
								nv->value_t = oldv->value_t;
							else if (oldv->type == LINKED)
								nv->linked_class = oldv->linked_class;

							//std::cout << "\t>> NEW VAR: " << nv->key << "=" << nv->value_f << std::endl;

							new_vars[nv->key] = nv;

							k++;
						}
					}

					ob = parse(m, cl, new_vars);

					if (remainder != "")
					{
						//ob->linked_class->key;
						ob = parse(remainder.substr(1), ob->linked_class, vars);
					}
				}
				else
					std::cout << "Method Not Found! " << key << std::endl;
			}
		}
		else if (p == "|")
		{
			ob->value_s = comp[0].substr(1, comp[0].length() - 2);
			ob->type = VERBA;
			return ob;
		}
		else if (comp[0][0] == '%')
		{
			Variable * oldb = parse(comp[0].substr(1), cl, vars);

			if (oldb->type == LINKED)
			{
				std::string val = parse(oldb->linked_class->methods["daVerba"], oldb->linked_class, {})->value_s;
				ob->value_s = val;
			}
			else if (oldb->type == NUMERUS)
				ob->value_s = std::to_string(oldb->value_f);
			else if (oldb->type == VERBA)
				ob->value_s = oldb->value_s;
			else if (oldb->type == STATUS)
			{
				if (oldb->value_b == true)
					ob->value_s = "$VERA";
				else
					ob->value_s = "$FALSUS";
			}

			ob->type = VERBA;
			return ob;
		}
		else if (comp[0][0] == '+')
		{
			std::string gn = comp[0].substr(1);

			if (gn == "numerus")
				ob->value_t = NUMERUS;
			else if (gn == "verba")
				ob->value_t = VERBA;
			else if (gn == "status")
				ob->value_t = STATUS;
			else if (gn == "gamma")
				ob->value_t = GAMMA;
			else
				ob->value_t = LINKED;

			ob->type = GENUS;
			return ob;
		}
		else if (comp[0][0] == '=')
		{
			ob->value_f = stoi(parse(comp[0].substr(1), cl, vars)->value_s);
			ob->type = NUMERUS;
			return ob;
		}
		else if (comp[0][0] == '-')
		{
			ob->value_f = 0 - parse(comp[0].substr(1), cl, vars)->value_f;
			ob->type = NUMERUS;
			return ob;
		}
		else if (comp[0][0] == '@')
		{
			Variable * oldb = parse(comp[0].substr(1), cl, vars);
			int n_v;
			if (oldb->type == GAMMA)
			{
				n_v = oldb->arr.size();
			}
			else if (oldb->type == VERBA)
			{
				n_v = oldb->value_s.length();
			}
			else if (oldb->type == LINKED)
			{
				n_v = parse(oldb->linked_class->methods["daMag"], oldb->linked_class, {})->value_f;
			}
			else
				n_v = 0;

			ob->value_f = n_v;
			ob->type = NUMERUS;
			return ob;
		}
		else if (comp[0][0] == '!')
		{
			bool val = parse(comp[0].substr(1), cl, vars)->value_b;
			ob->type = STATUS;

			if (val == false)
				ob->value_b = true;
			else
				ob->value_b = false;
		}
		else if (comp[0] == "$VERA" || comp[0] == "$FALSUS")
		{
			ob->value_b = comp[0] == "$VERA";
			ob->type = STATUS;
		}
		else if (vars.find(comp[0]) != vars.end())
		{
			//std::cout << s << " : "<< comp[0] << std::endl;
			return vars[comp[0]];
		}
		else if (is_number(comp[0]))
		{
			//std::cout << s << std::endl;
			ob->value_f = stoi(comp[0]);
			ob->type = NUMERUS;
			return ob;
		}
		else
		{
			ob->type = GENUS;
			if (comp[0] == "numerus")
				ob->value_t = (NUMERUS);
			else if (comp[0] == "verba")
				ob->value_t = (VERBA);
			else if (comp[0] == "status")
				ob->value_t = (STATUS);
			else if (comp[0] == "genus")
				ob->value_t = (GENUS);
			else if (comp[0] == "gamma")
				ob->value_t = (GAMMA);
			else if (comp[0] == "quis")
				ob->value_t = (QUIS);
			else
				ob->value_t = (LINKED);
		}
	}
	else
	{
		if (comp[0] == "redi")
		{
			//std::cout << "\t>> RETURNING VALUE: " << s.substr(comp[0].length() + 1) << std::endl;
			return parse(s.substr(comp[0].length() + 1), cl, vars);
		}
		else if (comp[0] == "novum")
		{
			std::string classname = "";
			for (int j = 0; j < comp[1].size(); j++)
			{
				std::string token = std::string(1, comp[1][j]);

				if (token == "<")
					break;
				else
				{
					classname += token;
				}
			}
			
			ob->type = LINKED;
			ob->linked_class = Delineate(source[classname]->body);
			parse(comp[1], ob->linked_class, vars);
			return ob;
		}
		else if (comp[0] == "verba")
		{
			Variable * nv = new Variable(comp[1], VERBA);

			if (comp.size() > 2)
			{
				nv->value_s = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars)->value_s;
				//vars[comp[1]] = nv;
			}
			else
				nv->value_s = "";

			vars[comp[1]] = nv;

			return ob;
		}
		else if (comp[0] == "numerus")
		{
			Variable * nv;
			if (vars[comp[1]] == NULL)
				nv = new Variable(comp[1], NUMERUS);
			else
				nv = vars[comp[1]];

			if (comp.size() > 2)
			{
				nv->value_f = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars)->value_f;
			}
			else
				nv->value_f = 0;

			vars[comp[1]] = nv;

			//std::cout << comp[1] << " = " << nv->value_f << std::endl;

			return ob;
		}
		else if (comp[0] == "status")
		{
			Variable * nv = new Variable(comp[1], STATUS);
			if (comp.size() > 2)
			{
				nv->value_b = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars)->value_b;
			}
			else
				nv->value_b = false;

			vars[comp[1]] = nv;

			return ob;
		}
		else if (comp[0] == "genus")
		{
			Variable * nv = new Variable(comp[1], GENUS);

			if (comp.size() > 2)
			{
				nv->value_s = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars)->value_s;
				//vars[comp[1]] = nv;
			}
			else
				nv->value_s = "";

			vars[comp[1]] = nv;

			return ob;
		}
		else if (comp[0] == "gamma")
		{
			Variable * nv = new Variable(comp[1], GAMMA);

			nv->dim.push_back(0);

			vars[comp[1]] = nv;

			return ob;
		}
		else if (comp[1] == "est")
		{
			std::string key;
			bool gamma = false;
			for (int i = 0; i < comp[0].length(); i++)
			{
				if (comp[0][i] == '<')
				{
					gamma = true;
					break;
				}
				else
					key += std::string(1, comp[0][i]);
			}

			Variable * v;

			if (!gamma)
			{
				v = vars[comp[0]];
			}
			else
			{
				std::string ifstmnt = split(comp[0])[0].substr(key.length() + 1);
				ifstmnt = ifstmnt.substr(0, ifstmnt.length() - 1);
				std::vector<std::string> split_var = split(ifstmnt, "&");
				int pos = 0;

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);

					int shift = parse(split_var[x], cl, vars)->value_f;

					//int shift = stoi(split_var[x]);
					for (int i = x + 1; i < vars[key]->dim.size(); i++)
						shift = shift * vars[key]->dim[i];

					pos += shift;
				}

				v = vars[key]->arr[pos];
			}

			Variable * n_var = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

			if (n_var->type != v->type) // && v->type != QUIS && n_var->type != QUIS)
			{
				std::cout << s << std::endl;
				std::cout << "Wrong Type! " << n_var->type << " not " << v->type << std::endl;
			}

			if (n_var->type == GAMMA)
			{
				v->arr = n_var->arr;
				v->dim = n_var->dim;
			}
			else
			{
				if (n_var->type == NUMERUS)
					v->value_f = n_var->value_f;
				else if (n_var->type == STATUS)
					v->value_b = n_var->value_b;
				else if (n_var->type == GENUS)
					v->value_t = n_var->value_t;
				else if (n_var->type == VERBA)
					v->value_s = n_var->value_s;
				else if (n_var->type == LINKED)
					v->linked_class = n_var->linked_class;

				//std::cout << "LOL";
			}
		}
		else if (source.find(comp[0]) == source.end())
		{
			Variable * v;

			std::string key;
			bool gamma = false;
			for (int i = 0; i < comp[0].length(); i++)
			{
				if (comp[0][i] == '<')
				{
					gamma = true;
					break;
				}
				else
					key += std::string(1, comp[0][i]);
			}

			if (gamma)
			{
				v = parse(comp[0], cl, vars);
				/*v = new Variable(o.key, o.type);
				v->value_s = o.value_s;
				v->value_f = o.value_f;
				if (o.linked_class != NULL)
					v->linked_class = o.linked_class;*/
			}
			else
			{
				if (vars.find(comp[0]) == vars.end())
				{
					if (comp[0][0] == '|')
					{
						v = new Variable("#TEMP-V", VERBA);
						v->value_s = parse(comp[0], cl, vars)->value_s;
					}
					else if (comp[0][0] == '%')
					{
						v = new Variable("#TEMP-V", VERBA);
						v->value_s = parse(vars[comp[0].substr(1)]->value_s, cl, vars)->value_s;
					}
					else if (comp[0][0] == '$')
					{
						v = new Variable("#TEMP-S", STATUS);
						v->value_b = parse(vars[comp[0].substr(1)]->value_s, cl, vars)->value_b;
					}
					else if (is_number(comp[0]))
					{
						v = new Variable("#TEMP-N", NUMERUS);
						v->value_f = parse(comp[0], cl, vars)->value_f;
					}
					else
					{
						v = parse(comp[0], cl, vars);

						/* = new Variable("#TEMP-Q", old.type);
						v->value_s = old.value_s;
						v->value_f = old.value_f;*/
					}
				}
				else
				{
					v = vars[comp[0]];
				}
			}

			if (v->type == VERBA)
			{
				ob->type = VERBA;

				if (comp[1] == "plus")
				{
					std::string n_value = v->value_s + parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_s;
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					//ob->type = "verba";
					ob->value_s = n_value;
					//std::cout << "RET: " << n_value << std::endl;
					return ob;
				}
			}
			else if (v->type == NUMERUS)
			{
				ob->type = NUMERUS;

				if (comp[1] == "plus")
				{
					int n_value = v->value_f + parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					ob->value_f = n_value;
					return ob;
				}

				if (comp[1] == "minus")
				{
					int n_value = v->value_f - parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					ob->value_f = n_value;
					return ob;
				}

				if (comp[1] == "mod")
				{
					int n_value = (int)v->value_f % (int)parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					ob->value_f = n_value;
					return ob;
				}

				if (comp[1] == "mula")
				{
					int n_value = v->value_f * parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					ob->value_f = n_value;
					return ob;
				}

				if (comp[1] == "vide")
				{
					int n_value = v->value_f / parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					ob->value_f = n_value;
					return ob;
				}

				if (comp[1] == "adde")
				{
					int n_value = v->value_f + parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					v->value_f = n_value;
					ob->type = NULL_VAR;
					return ob;
				}

				if (comp[1] == "accu")
				{
					int n_value = v->value_f * parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					v->value_f = n_value;
					ob->type = NULL_VAR;
					return ob;
				}

				if (comp[1] == "strahe")
				{
					int n_value = v->value_f - parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					v->value_f = n_value;
					ob->type = NULL_VAR;
					return ob;
				}

				if (comp[1] == "divi")
				{
					int n_value = v->value_f / parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
					v->value_f = n_value;
					ob->type = NULL_VAR;
					return ob;
				}
			}
			else if (v->type == STATUS)
			{
				ob->type = STATUS;

				if (comp[1] == "et")
				{
					Variable * n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if (n_value->value_b == true && v->value_b == true)
					{
						ob->value_b = true;
					}
					else
						ob->value_b = false;

					return ob;
				}

				if (comp[1] == "vel")
				{
					Variable * n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if (n_value->value_b == true || v->value_b == true)
					{
						ob->value_b = true;
					}
					else
						ob->value_b = false;

					return ob;
				}

				if (comp[1] == "aut")
				{
					Variable * n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if ((n_value->value_b == true && v->value_b == false) || (n_value->value_b == false && v->value_b == true))
					{
						ob->value_b = true;
					}
					else
						ob->value_b = false;

					return ob;
				}
			}

			if (comp[1] == "sit")
			{
				Variable * n = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

				ob->type = STATUS;
				
				if (n->type == NUMERUS)
				{
					if (n->value_f == v->value_f)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == STATUS)
				{
					if (n->value_b == v->value_b)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == VERBA)
				{
					if (n->value_s == v->value_s)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == GENUS)
				{
					if (n->value_t == v->value_t)
						ob->value_b = true;
					else
						ob->value_b = false;
				}

				return ob;
			}
			else if (comp[1] == "non")
			{
				Variable * n = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

				ob->type = STATUS;

				if (n->type == NUMERUS)
				{
					if (n->value_f != v->value_f)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == STATUS)
				{
					if (n->value_b != v->value_b)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == VERBA)
				{
					if (n->value_s != v->value_s)
						ob->value_b = true;
					else
						ob->value_b = false;
				}
				else if (n->type == GENUS)
				{
					if (n->value_t != v->value_t)
						ob->value_b = true;
					else
						ob->value_b = false;
				}

				return ob;
			}
			else if (comp[1] == "minor")
			{
				int n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
				ob->type = STATUS;

				if (n_value > v->value_f)
					ob->value_b = true;
				else
					ob->value_b = false;

				//std::cout << s << " : "<< n_value << " > " << v->value_f << std::endl;

				return ob;
			}
			else if (comp[1] == "maior")
			{
				int n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars)->value_f;
				ob->type = STATUS;

				if (n_value < v->value_f)
					ob->value_b = true;
				else
					ob->value_b = false;

				return ob;
			}
		}
		else if (source.find(comp[0]) != source.end())
		{
			Variable * nv = new Variable(comp[1], LINKED);
			vars[nv->key] = nv;
		}
	}

	return ob;
}

std::vector<std::string> Parser::line_split(std::string s)
{
	s = s + "*";
	std::vector<std::string> raw;
	std::string delimiter = "*";
	int level = 0;
	std::string cur = "";
	bool in_p = false;

	for (int i = 0; i < s.length(); i++)
	{
		std::string token = std::string(1, s[i]);

		if (token == "|")
			in_p = !in_p;

		if (token == ":")
			level++;

		if (token == "*" && level == 0 && !in_p)
		{
			raw.push_back(cur);
			cur = "";
		}
		else
		{
			cur = cur + token;
		}

		if (token == "^")
		{
			level--;

			if (level == 0 && !in_p)
			{
				raw.push_back(cur);
				cur = "";
			}
		}

	}

	return raw;
}

Method * Parser::ParseMethod(std::string s, Class * slavus)
{
	//cout << "PARSING: " << s << endl;

	std::vector<std::string> comp = split(s);

	//cout << " >> TYPE: " << comp[0] << endl;

	std::string key;
	std::string c;
	int i = 0;
	while (c != "<")
	{
		c = std::string(1, comp[1][i]);
		key = key + c;
		i++;
	}
	key.erase(std::remove(key.begin(), key.end(), '<'), key.end());

	std::string args = comp[1].substr(i);
	args = args.substr(0, args.length() - 1);

	//cout << " >> KEY: " << key << endl;
	//cout << " >> ARGS: " << endl;


	Method * m = new Method(slavus, key);

	if (!args.empty())
	{
		std::vector<std::string> args_s = split(args, "&");

		for (int i = 0; i < args_s.size(); i++)
		{
			while (args_s[i][0] == ' ')
				args_s[i] = args_s[i].substr(1, args_s[i].length());
			while (args_s[i][args_s[i].length() - 1] == ' ')
				args_s[i] = args_s[i].substr(0, args_s[i].length() - 1);

			//cout << "\t<" << args_s[i] << ">" << endl;
			std::vector<std::string> param = split(args_s[i]);

			m->arguement_names.push_back(param[1]);

			if (param[0] == "numerus")
				m->arguement_types.push_back(NUMERUS);
			else if (param[0] == "verba")
				m->arguement_types.push_back(VERBA);
			else if (param[0] == "status")
				m->arguement_types.push_back(STATUS);
			else if (param[0] == "genus")
				m->arguement_types.push_back(GENUS);
			else if (param[0] == "gamma")
				m->arguement_types.push_back(GAMMA);
			else if (param[0] == "quis")
				m->arguement_types.push_back(QUIS);
			else
				m->arguement_types.push_back(LINKED);

			//m->arguements[param[1]] = param[0];
		}
	}
	else
		//cout << "\t<N/A>" << endl;

		m->type = comp[0];

	return m;
}

Class * Parser::Delineate(std::string s)
{
	std::vector<std::string> raw;
	//std::std::string delimiter = " ";

	//cout << "CLASS: ";

	std::string key;
	std::string c;
	int i = 0;
	while (c != ":")
	{
		c = std::string(1, s[i]);
		key = key + c;
		i++;
	}
	key.erase(std::remove(key.begin(), key.end(), ':'), key.end());

	//cout << split(key)[1] << endl;

	int level = 0;
	std::string cur = "";
	bool inMethod = false;
	for (int i = 0; i < s.length(); i++)
	{
		std::string token = std::string(1, s[i]);

		if (token == ":")
		{
			if (level == 1)
			{
				raw.push_back("(M)" + cur);
				cur = "";
				//cur = cur + "\n\t{";
			}
			else if (level > 1)
				cur = cur + ":";
			level++;
		}
		else if (token == "^")
		{
			if (level == 2)
			{
				raw.push_back(cur);
				cur = "";
			}
			else if (level > 1)
				cur = cur + "^";
			level--;
		}
		else if (token == "*")
		{
			if (level == 1)
			{
				raw.push_back("(V)" + cur + "*");
				cur = "";
			}
			else
				cur = cur + "*";
		}
		else if (level >= 1)
		{
			cur = cur + token;
		}
	}

	//for (int i = 0; i < raw.size(); i++) //cout << " >> " << raw[i] << endl;

	Class * slavus = new Class(split(key)[1]);
	slavus->body = s;

	for (int i = 0; i < raw.size(); i++)
	{
		if (raw[i].substr(0, 3) == "(M)")
		{
			Method * m = ParseMethod(raw[i].substr(3), slavus);
			m->body = raw[i + 1];
			slavus->methods[m->key] = m;
		}
		else if (raw[i].substr(0, 3) == "(V)")
		{
			/*raw[i] = raw[i].substr(3);
			std::vector<std::string> comp = split(raw[i]);
			Variable * v = new Variable(comp[1], comp[0]);

			if (comp.size() > 3)
			{
				std::string vle = raw[i].substr(comp[0].length() + comp[1].length() + comp[2].length() + 3);
				vle = vle.substr(0, vle.length() - 2);
				v->value = parse(vle, slavus, slavus->globals).value;
			}*/

			std::map<std::string, Variable *> vargs;

			raw[i] = raw[i].substr(3);
			raw[i] = raw[i].substr(0, raw[i].length() - 2);

			parse(raw[i], slavus, vargs);

			for (auto it = vargs.begin(); it != vargs.end(); ++it) slavus->globals[it->first] = it->second;

			//cout << "VARIABLE: " << v->key << " : " << v->value << endl;
		}
	}

	//cout << cur << endl;

	return slavus;
}

std::vector<std::string> Parser::split(std::string s, std::string d)
{
	s = s + d;
	std::vector<std::string> raw;
	std::string delimiter = d;

	int level = 0;
	std::string cur = "";
	bool in_p = false;
	for (int i = 0; i < s.length(); i++)
	{
		std::string token = std::string(1, s[i]);

		if (token == "|")
			in_p = !in_p;

		if (token == "<")
		{
			cur += token;
			level++;
		}
		else if (token == ">")
		{
			cur += token;
			level--;
		}
		else if (token == d && level == 0 && !in_p)
		{
			raw.push_back(cur);
			cur = "";
		}
		else
		{
			cur = cur + token;
		}
	}

	return raw;
}

void Parser::loadClassFile(std::string filename)
{
	std::string line;
	std::ifstream myfile(filename);
	std::string raw;

	bool inhead = true;
	bool temp = false;

	if (myfile.is_open())
	{
		while (std::getline(myfile, line))
		{
			if (line != "")
			{
				if (inhead)
				{
					std::vector<std::string> sep = split(line);

					if (sep[0] == "utendum")
					{
						if (source.find(sep[1]) == source.end())
							loadClassFile(sep[1] + ".svs");
					}
					else if (sep[0] == "slavus")
					{
						inhead = false;
					}
					else if (sep[0] == "species")
					{
						inhead = false;
						temp = true;
					}
				}

				if (!inhead)
				{
					line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

					std::size_t found = line.find("#");
					if (found != std::string::npos)
						line = line.substr(0, found);

					raw = raw + line;
				}
			}
		}

		myfile.close();
	}

	if (temp)
	{
		std::string head;
		std::string c;
		int i = 0;
		while (c != ":")
		{
			c = std::string(1, raw[i]);
			head = head + c;
			i++;
		}
		head.erase(std::remove(head.begin(), head.end(), ':'), head.end());

		std::string key;
		c = "";
		i = 0;
		while (c != "<")
		{
			c = std::string(1, head[i]);
			key = key + c;
			i++;
		}
		key.erase(std::remove(key.begin(), key.end(), '<'), key.end());

		std::string types_raw = head.substr(key.length() + 1);
		types_raw = types_raw.substr(0, types_raw.length() - 1);
		key = split(key)[1];

		std::vector<std::string> args_s = split(types_raw, "&");

		for (int i = 0; i < args_s.size(); i++)
		{
			while (args_s[i][0] == ' ')
				args_s[i] = args_s[i].substr(1, args_s[i].length());
			while (args_s[i][args_s[i].length() - 1] == ' ')
				args_s[i] = args_s[i].substr(0, args_s[i].length() - 1);
		}

		raw = raw.substr(head.length());
		raw = "slavus " + key + raw;

		Template * t = new Template(key, args_s);
		t->body = raw;

		templates[key] = t;
	}
	else
	{
		Class * c = Delineate(raw);
		source[c->key] = c;
	}
}

void Parser::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
