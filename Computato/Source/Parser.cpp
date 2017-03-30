#include "Computato.h"

//VARIABLE FUNCTIONS:

Variable::Variable()
{
	key = "_TEMP_";
	linked_class = NULL;
}

Variable::Variable(std::string k, std::string t)
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

Variable Parser::parse(Method *m, Class * cl, std::map<std::string, Variable *> args)
{
	std::vector<std::string> lines = line_split(m->body);

	Variable out;

	for (int i = 0; i < lines.size() - 1; i++)
	{
		////cout << " >> " << lines[i] << endl;

		while (lines[i][0] == ' ')
			lines[i] = lines[i].substr(1);
		while (lines[i][lines[i].length() - 1] == ' ')
			lines[i] = lines[i].substr(0, lines[i].length() - 1);

		out = parse(lines[i], cl, args);

		if (out.value != "")
		{
			//cout << "\t>> RETURNING VALUE: (" << out << ")" << endl;
			return out;
		}
	}

	args.erase(args.begin(), args.end());

	return out;
}

Variable Parser::parse(std::string s, Class * cl, std::map<std::string, Variable *> &vars)
{
	std::vector<std::string> comp;

	for (auto const &v : cl->globals)
	{
		vars[v.first] = v.second;
		//cout << "GLOB: " << v.second->value << endl;
	}

	if (s.compare(0, 2, "si") != 0 && s.compare(0, 3, "dum") != 0 && s.compare(0, 5, "inter") != 0 && s.compare(0, 5, "gamma") != 0)
	{
		comp = split(s);
	}
	else
		comp = { s };

	//cout << "\t>> CLASS: " << cl->key << " >> " << s << ", ARGS: " << comp.size() << endl;

	Variable ob;

	if (s == "fuge")
	{
		ob.type = "#FUGE";
		return ob;
	}

	if (comp[0] == "CONSOLE_OUTPUT")
	{
		std::cout << vars[comp[1]]->value;
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

		ob.value = line;

		return ob;
	}
	else if (comp[0] == "GET_RAW_RANDOM")
	{
		ob.value = std::to_string(rand());
		ob.type = "numerus";

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
			if (p == "<" || p == "~" || p == "|" || p == "!")
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
				//cout << "\t>> GAMMA SIZE:\t" << size << endl;
				//cout << "\t>> GAMMA NAME:\t" << split(s)[1] << endl;
				//cout << "\t>> GAMMA TYPE:\t" << split(s)[2] << endl;

				std::vector<std::string> split_arg = split(s);
				std::vector<std::string> split_var = split(ifstmnt, "&");
				int size = -1;


				Variable * g = new Variable(split_arg[1], "gamma");

				for (int x = 0; x < split_var.size(); x++)
				{
					while (split_var[x][0] == ' ')
						split_var[x] = split_var[x].substr(1);
					while (split_var[x][split_var[x].length() - 1] == ' ')
						split_var[x] = split_var[x].substr(0, split_var[x].length() - 1);

					split_var[x] = parse(split_var[x], cl, vars).value;

					g->dim.push_back(stoi(split_var[x]));

					if (size == -1)
						size = stoi(split_var[x]);
					else
						size *= stoi(split_var[x]);
				}
				for (int i = 0; i < size; i++)
				{
					Variable * nv = new Variable("sub" + i, split_arg[2]);

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

				//cout << "\t>> IF LINES:\t" << iflines << endl;

				if (parse(ifstmnt, cl, vars).value == "$VERA")
				{
					//cout << "\t>> TRUE - RUNNING IF STATEMENT" << endl;

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

							if (ob.value != "")
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

				while (parse(ifstmnt, cl, vars).value == "$VERA" && !broke)
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

							if (ob.value != "" || ob.type == "#FUGE")
							{
								ob.type = "";
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

				bool broke = false;

				while (parse(split_var[1], cl, vars).value == "$VERA" && !broke)
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

							if (ob.value != "" || ob.type == "#FUGE")
							{
								ob.type = "";
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

					split_var[x] = parse(split_var[x], cl, vars).value;

					int shift = stoi(split_var[x]);
					for (int i = x + 1; i < vars[key]->dim.size(); i++)
						shift = shift * vars[key]->dim[i];

					pos += shift;
				}

				ob.type = vars[key]->arr[pos]->type;
				ob.value = vars[key]->arr[pos]->value;
				ob.key = key;

				if (vars[key]->arr[pos]->linked_class != NULL)
				{
					if (remainder != "")
					{
						ob = parse(remainder.substr(1), vars[key]->arr[pos]->linked_class, vars);
					}
					else
						ob.linked_class = vars[key]->arr[pos]->linked_class;
				}

				return ob;
			}
			else
			{

				std::string value;// = split(s)[0].substr(key.length() + 1);
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

					value = value + token;
				}

				remainder = s.substr(key.length() + value.length() + 2);

				//cout << "\t>> " << key << " : " << value << " : " << remainder << endl;

				if (key.length() == 0)
					return parse(value, cl, vars);

				//cout << "\t>> " << key << " : " << value << endl;

				if (cl->methods[key] != NULL)
				{
					Method * m = cl->methods[key];
					std::map<std::string, Variable *> new_vars;
					if (!m->arguements.empty())
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

						for (auto const &v : cl->methods[key]->arguements)
						{
							Variable * nv = new Variable(v.first, v.second);
							nv->value = parse(split_var[k], cl, vars).value;
							//cout << "\t>> NEW VAR: " << nv->key << "=" << nv->value << endl;

							new_vars[nv->key] = nv;

							k++;
						}
					}

					ob = parse(m, cl, new_vars);

					if (remainder != "")
					{
						//ob.linked_class->key;
						ob = parse(remainder.substr(1), ob.linked_class, vars);
					}
				}
				else
					std::cout << "Method Not Found! " << key << std::endl;
			}
		}
		else if (p == "|")
		{
			ob.value = comp[0].substr(1, comp[0].length() - 2);
			ob.type = "verba";
		}
		else if (is_number(comp[0]))
		{
			ob.value = comp[0];
			ob.type = "numerus";
		}
		else if (comp[0][0] == '%')
		{
			ob.value = parse(comp[0].substr(1), cl, vars).value;
			ob.type = "verba";
		}
		else if (comp[0][0] == '!')
		{
			std::string val = parse(comp[0].substr(1), cl, vars).value;
			ob.type = "status";

			if (val == "$FALSUS")
				ob.value = "$VERA";
			else
				ob.value = "$FALSUS";
		}
		else if (comp[0] == "$VERA" || comp[0] == "$FALSUS")
		{
			ob.value = comp[0];
			ob.type = "status";
		}
		else
		{
			ob.value = vars[comp[0]]->value;
			ob.type = vars[comp[0]]->type;

			if (vars[comp[0]]->linked_class != NULL)
				ob.linked_class = vars[comp[0]]->linked_class;
		}
	}
	else
	{
		if (comp[0] == "redi")
		{
			//cout << "\t>> RETURNING VALUE: " << s.substr(comp[0].length() + 1) << endl;
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

			ob.linked_class = Delineate(source[classname]->body);
			parse(comp[1], ob.linked_class, vars);
			return ob;
		}
		else if (comp[0] == "verba")
		{
			Variable * nv = new Variable(comp[1], "verba");

			if (comp.size() > 2)
			{
				nv->value = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars).value;
				vars[comp[1]] = nv;
			}
			else
				nv->value = "";

			vars[comp[1]] = nv;

			//cout << "\t>> NEW VAR: " << nv->key << "=" << nv->value << endl;
		}
		else if (comp[0] == "numerus")
		{
			Variable * nv = new Variable(comp[1], "numerus");
			if (comp.size() > 2)
			{
				nv->value = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars).value;
			}
			else
				nv->value = "0";

			vars[comp[1]] = nv;

			//cout << "\t>> NEW VAR: " << nv->key << "=" << nv->value << endl;
		}
		else if (comp[0] == "status")
		{
			Variable * nv = new Variable(comp[1], "status");
			if (comp.size() > 2)
			{
				nv->value = parse(s.substr(comp[0].length() + comp[1].length() + comp[2].length() + 3), cl, vars).value;
			}
			else
				nv->value = "$FALSUS";

			vars[comp[1]] = nv;

			//cout << "\t>> NEW VAR: " << nv->key << "=" << nv->value << endl;
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

					split_var[x] = parse(split_var[x], cl, vars).value;

					int shift = stoi(split_var[x]);
					for (int i = x + 1; i < vars[key]->dim.size(); i++)
						shift = shift * vars[key]->dim[i];

					pos += shift;
				}

				v = vars[key]->arr[pos];
			}

			Variable n_var = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

			if (n_var.linked_class == NULL)
			{
				v->value = n_var.value;
			}
			else
			{
				Class * lc = n_var.linked_class;
				v->linked_class = lc;
				v->value = n_var.value;
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
				Variable o = parse(comp[0], cl, vars);
				v = new Variable(o.key, o.type);
				v->value = o.value;
				if (o.linked_class != NULL)
					v->linked_class = o.linked_class;
			}
			else
			{
				if (vars.find(comp[0]) == vars.end())
				{
					if (comp[0][0] == '|')
					{
						v = new Variable("#TEMP-V", "verba");
						v->value = parse(comp[0], cl, vars).value;
					}
					else if (comp[0][0] == '%')
					{
						v = new Variable("#TEMP-V", "verba");
						v->value = parse(vars[comp[0].substr(1)]->value, cl, vars).value;
					}
					else if (comp[0][0] == '$')
					{
						v = new Variable("#TEMP-S", "status");
						v->value = parse(vars[comp[0].substr(1)]->value, cl, vars).value;
					}
					else
					{
						v = new Variable("#TEMP-N", "numerus");
						v->value = parse(comp[0], cl, vars).value;
					}
				}
				else
				{
					v = vars[comp[0]];
				}
			}

			if (v->type == "verba")
			{
				ob.type = "verba";

				if (comp[1] == "plus")
				{
					std::string n_value = v->value + parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value;
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}
			}
			else if (v->type == "numerus")
			{
				ob.type = "numerus";

				if (comp[1] == "plus")
				{
					std::string n_value = std::to_string(stoi(v->value) + stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}

				if (comp[1] == "minus")
				{
					std::string n_value = std::to_string(stoi(v->value) - stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}

				if (comp[1] == "mod")
				{
					std::string n_value = std::to_string(stoi(v->value) % stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}

				if (comp[1] == "mula")
				{
					std::string n_value = std::to_string(stoi(v->value) * stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}

				if (comp[1] == "vide")
				{
					std::string n_value = std::to_string(stoi(v->value) / stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					ob.value = n_value;
					return ob;
				}

				if (comp[1] == "adde")
				{
					std::string n_value = std::to_string(stoi(v->value) + stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					v->value = n_value;
				}

				if (comp[1] == "accu")
				{
					std::string n_value = std::to_string(stoi(v->value) * stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					v->value = n_value;
				}

				if (comp[1] == "strahe")
				{
					std::string n_value = std::to_string(stoi(v->value) - stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					v->value = n_value;
				}

				if (comp[1] == "divi")
				{
					std::string n_value = std::to_string(stoi(v->value) / stoi(parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value));
					//cout << "\t>> SETTING " << comp[0] << " TO " << n_value << endl;
					v->value = n_value;
				}
			}
			else if (v->type == "status")
			{
				ob.type = "status";

				if (comp[1] == "et")
				{
					Variable n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if (n_value.value == "$VERA" && v->value == "$VERA")
					{
						ob.value = "$VERA";
					}
					else
						ob.value = "$FALSUS";

					return ob;
				}

				if (comp[1] == "vel")
				{
					Variable n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if (n_value.value == "$VERA" || v->value == "$VERA")
					{
						ob.value = "$VERA";
					}
					else
						ob.value = "$FALSUS";

					return ob;
				}

				if (comp[1] == "aut")
				{
					Variable n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars);

					if ((n_value.value == "$VERA" && v->value == "$FALSUS") || (n_value.value == "$FALSUS" && v->value == "$VERA"))
					{
						ob.value = "$VERA";
					}
					else
						ob.value = "$FALSUS";

					return ob;
				}
			}

			if (comp[1] == "sit")
			{
				std::string n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value;

				ob.type = "status";

				if (n_value == v->value)
					ob.value = "$VERA";
				else
					ob.value = "$FALSUS";

				return ob;
			}
			else if (comp[1] == "non")
			{
				std::string n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value;

				ob.type = "status";

				if (n_value != v->value)
					ob.value = "$VERA";
				else
					ob.value = "$FALSUS";

				return ob;
			}
			else if (comp[1] == "minor")
			{
				std::string n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value;
				ob.type = "status";

				if (stoi(n_value) > stoi(v->value))
					ob.value = "$VERA";
				else
					ob.value = "$FALSUS";

				return ob;
			}
			else if (comp[1] == "maior")
			{
				std::string n_value = parse(s.substr(comp[0].length() + comp[1].length() + 2), cl, vars).value;
				ob.type = "status";

				if (stoi(n_value) < stoi(v->value))
					ob.value = "$VERA";
				else
					ob.value = "$FALSUS";

				return ob;
			}
		}
		else if (source.find(comp[0]) != source.end())
		{
			Variable * nv = new Variable(comp[1], comp[0]);
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

			m->arguements[param[1]] = param[0];
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
			raw[i] = raw[i].substr(3);
			std::vector<std::string> comp = split(raw[i]);
			Variable * v = new Variable(comp[1], comp[0]);

			if (comp.size() > 3)
			{
				std::string vle = raw[i].substr(comp[0].length() + comp[1].length() + comp[2].length() + 3);
				vle = vle.substr(0, vle.length() - 2);
				v->value = parse(vle, slavus, slavus->globals).value;
			}

			slavus->globals[v->key] = v;

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

	Class * c = Delineate(raw);
	source[c->key] = c;
}
