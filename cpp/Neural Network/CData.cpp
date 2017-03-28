#include "neuralnetwork.h"


DATA::DATA(std::string s)
{
	type = 1;
	raw_string = s;
}

DATA::DATA(float f)
{
	type = 0;
	raw_float = f;
}

DATA::operator std::string()
{
	type = 1;
	return raw_string;
}

DATA::operator float()
{
	type = 0;
	return raw_float;
}

bool DATA::isString() { return type == 1; }

std::string DATA::Print()
{
	if (isString())
		return raw_string;
	else
		return std::to_string(raw_float);
}

Data::Data(std::string filename, std::string delim, bool FORCE_CAT)
{
	std::ifstream infile(filename);
	std::string line;

	while (std::getline(infile, line))
	{
		line = line + delim;
		std::vector<std::string> raw;
		std::string delimiter = delim;

		size_t pos = 0;
		std::string token;
		while ((pos = line.find(delimiter)) != std::string::npos) {
			token = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());

			raw.push_back(token);
		}

		DATA target(raw[raw.size() - 1]);

		if (IsFloat(raw[raw.size() - 1]) && FORCE_CAT!=true)
			target = DATA(stof(raw[raw.size() - 1]));

		raw.pop_back();

		std::vector<DATA> data_value;
		for (int i = 0; i < raw.size(); i++)
		{
			if (IsFloat(raw[i]))
				data_value.push_back(DATA(stof(raw[i])));
			else
				data_value.push_back(DATA(raw[i]));
		}

		//AddData(data_value, { target });
		dataValues.push_back(data_value);
		targetValues.push_back({target});
	}

	Transform();
}

Data::Data() {}

Data::~Data()
{
	if (linkedNetwork != NULL)
		delete linkedNetwork;
}

void Data::AddData(std::vector<DATA> values, std::vector<DATA> targets)
{
	dataValues.push_back(values);
	targetValues.push_back(targets);
	Transform();
}

void Data::Print()
{
	for (int i = 0; i < dataValues.size(); i++)
	{
		for (int j = 0; j < dataValues[i].size(); j++) std::cout << dataValues[i][j].Print() << "\t";

		std::cout << "->\t";

		for (int j = 0; j < targetValues[i].size(); j++) std::cout << targetValues[i][j].Print() << "\t";

		std::cout << std::endl;
	}
}

bool Data::IsFloat(std::string myString) {
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
							   // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

void Data::Transform()
{
	min_I.clear();
	max_I.clear();
	min_O.clear();
	max_O.clear();
	cat_I.clear();
	cat_O.clear();
	indices_I.clear();
	indices_O.clear();

	for (int i = 0; i < dataValues[0].size(); i++)
	{
		min_I.push_back(FLT_MAX);
		max_I.push_back(FLT_MIN);
		cat_I.push_back({});
	}

	for (int i = 0; i < dataValues.size(); i++)
	{
		for (int j = 0; j < dataValues[i].size(); j++)
		{
			if (dataValues[i][j].isString())
			{
				if (std::find(cat_I[j].begin(), cat_I[j].end(), (std::string)dataValues[i][j]) == cat_I[j].end())
				{
					cat_I[j].push_back((std::string)dataValues[i][j]);
					min_I[j] = 0;
					max_I[j] = cat_I[j].size() - 1;
				}
			}
			else
			{
				if ((float)dataValues[i][j] < min_I[j])
				{
					min_I[j] = (float)dataValues[i][j];
				}

				if ((float)dataValues[i][j] > max_I[j])
				{
					max_I[j] = (float)dataValues[i][j];
				}
			}
		}
	}

	for (int i = 0; i < targetValues[0].size(); i++)
	{
		min_O.push_back(FLT_MAX);
		max_O.push_back(FLT_MIN);
		cat_O.push_back({});
	}

	for (int i = 0; i < targetValues.size(); i++)
	{
		for (int j = 0; j < targetValues[i].size(); j++)
		{
			if (targetValues[i][j].isString())
			{
				if (std::find(cat_O[j].begin(), cat_O[j].end(), (std::string)targetValues[i][j]) == cat_O[j].end())
				{
					cat_O[j].push_back((std::string)targetValues[i][j]);
					min_O[j] = 0;
					max_O[j] = cat_O[j].size() - 1;
				}
			}
			else
			{
				if ((float)targetValues[i][j] < min_O[j])
				{
					min_O[j] = (float)targetValues[i][j];
				}

				if ((float)targetValues[i][j] > max_O[j])
				{
					max_O[j] = (float)targetValues[i][j];
				}
			}
		}
	}

	//INPUT BINARY
	binary_I.clear();
	size_I = 0;

	for (int k = 0; k < cat_I.size(); k++)
	{
		std::map<std::string, std::vector<int>> datum;

		if (cat_I[k].size() > 0)
		{
			indices_I.push_back(std::to_string(std::stoi(std::bitset<8>(cat_I[k].size() - 1).to_string())).length());
			size_I += indices_I[k];

			for (int i = 0; i < cat_I[k].size(); i++)
			{
				std::string bin = std::to_string(stoi(std::bitset<8>(i).to_string()));
				std::vector<int> values;

				while (bin.length() < indices_I[k])
					bin = "0" + bin;

				for (int j = 0; j < bin.length(); j++)
				{
					values.push_back(stoi(std::string(1, bin.at(j))));
				}

				datum[cat_I[k][i]] = values;
			}
		}
		else
		{
			indices_I.push_back(1);
			size_I++;
		}

		binary_I.push_back(datum);
	}

	//OUTPIT BINARY
	binary_O.clear();
	size_O = 0;

	for (int k = 0; k < cat_O.size(); k++)
	{
		std::map<std::string, std::vector<int>> datum;

		if (cat_O[k].size() > 0)
		{
			indices_O.push_back(std::to_string(std::stoi(std::bitset<8>(cat_O[k].size() - 1).to_string())).length());
			size_O += indices_O[k];

			for (int i = 0; i < cat_O[k].size(); i++)
			{
				std::string bin = std::to_string(stoi(std::bitset<8>(i).to_string()));
				std::vector<int> values;

				while (bin.length() < indices_O[k])
					bin = "0" + bin;

				for (int j = 0; j < bin.length(); j++)
				{
					values.push_back(stoi(std::string(1, bin.at(j))));
				}

				datum[cat_O[k][i]] = values;
			}
		}
		else
		{
			indices_O.push_back(1);
			size_O++;
		}

		binary_O.push_back(datum);
	}
}

void Data::Info()
{
	std::cout << "INPUT PARAMETERS:" << std::endl;

	std::cout << "MIN VALUES:\t";
	for (int i = 0; i < min_I.size(); i++)
		std::cout << min_I[i] << "\t";
	std::cout << std::endl;

	std::cout << "MAX VALUES:\t";
	for (int i = 0; i < max_I.size(); i++)
		std::cout << max_I[i] << "\t";
	std::cout << std::endl;

	std::cout << "CATEGORIES:" << std::endl;
	for (int i = 0; i < cat_I.size(); i++)
	{
		if (!cat_I[i].empty())
		{
			std::cout << i << ":\t";
			for (int j = 0; j < cat_I[i].size(); j++)
				std::cout << cat_I[i][j] << "\t";

			std::cout << std::endl;
		}
	}
	std::cout << "REAL INPUT:\t" << size_I << std::endl;

	std::cout << std::endl << "OUTPUT PARAMETERS:" << std::endl;

	std::cout << "MIN VALUES:\t";
	for (int i = 0; i < min_O.size(); i++)
		std::cout << min_O[i] << "\t";
	std::cout << std::endl;

	std::cout << "MAX VALUES:\t";
	for (int i = 0; i < max_O.size(); i++)
		std::cout << max_O[i] << "\t";
	std::cout << std::endl;

	std::cout << "CATEGORIES:" << std::endl;
	for (int i = 0; i < cat_O.size(); i++)
	{
		if (!cat_O[i].empty())
		{
			std::cout << i << ":\t";
			for (int j = 0; j < cat_O[i].size(); j++)
				std::cout << cat_O[i][j] << "\t";

			std::cout << std::endl;
		}
	}

	std::cout << "REAL OUTPUT:\t" << size_O << std::endl;
}

std::vector<float> Data::GetTestValues(int i)
{
	std::vector<float> values;

	for (int j = 0; j < dataValues[i].size(); j++)
	{
		if (dataValues[i][j].isString())
		{
			std::vector<int> bin = binary_I[j][(std::string)dataValues[i][j]];

			for (int k = 0; k < bin.size(); k++)
				values.push_back(bin[k]);
		}
		else
		{
			float index = (float)dataValues[i][j];
			values.push_back((index - min_I[j]) / (max_I[j] - min_I[j]));
		}
	}

	return values;
}

std::vector<float> Data::GetTestTargets(int i)
{
	std::vector<float> values;

	for (int j = 0; j < targetValues[i].size(); j++)
	{
		if (targetValues[i][j].isString())
		{
			std::vector<int> bin = binary_O[j][(std::string)targetValues[i][j]];

			for (int k = 0; k < bin.size(); k++)
				values.push_back(bin[k]);
		}
		else
		{
			float index = (float)targetValues[i][j];
			values.push_back((index - min_O[j]) / (max_O[j] - min_O[j]));
		}
	}

	return values;
}

std::vector<DATA> Data::InterpretOutput(std::vector<float> output)
{
	std::vector<DATA> translation;
	int c = 0;

	for (int i = 0; i < indices_O.size(); i++)
	{
		if (indices_O[i] == 1)
		{
			if (targetValues[0][i].isString())
				translation.push_back(DATA(cat_O[i][round(output[c++])]));
			else
				translation.push_back(DATA(round(RevTarget(output[c++], i))));
		}
		else
		{
			std::string val = "";

			for (int j = 0; j < indices_O[i]; j++)
			{
				val = val + std::to_string((int)round(output[c++]));
			}

			translation.push_back(DATA(cat_O[i][binary_to_decimal(stoi(val))]));
		}
	}

	return translation;
}

void Data::CreateLinkedNetwork(NeuralNetwork * &network, int layers)
{
	network = new NeuralNetwork(size_I, size_O, layers, (int)ceil((size_I + size_O) / 2.0));
	this->linkedNetwork = network;
}

int Data::binary_to_decimal(int num)
{
	unsigned res = 0;

	for (int i = 0; num > 0; ++i)
	{
		if ((num % 10) == 1)
			res += (1 << i);

		num /= 10;
	}

	return res;
}

std::vector<DATA> Data::Invoke(std::vector<DATA> input)
{
	std::vector<DATA> ret;

	if (linkedNetwork == NULL)
		return ret;

	std::vector<float> values;

	for (int j = 0; j < input.size(); j++)
	{
		if (input[j].isString())
		{
			std::vector<int> bin = binary_I[j][(std::string)input[j]];

			for (int k = 0; k < bin.size(); k++)
				values.push_back(bin[k]);
		}
		else
		{
			float index = (float)input[j];
			values.push_back((index - min_I[j]) / (max_I[j] - min_I[j]));
		}
	}

	linkedNetwork->Propagate(values);

	ret = InterpretOutput(linkedNetwork->GetOutputs());

	return ret;
}

void Data::Train(int epochs)
{
	std::cout << "Training Network..." << std::endl;

	int space = epochs / 10;
	for (int i = 1; i <= epochs; i++)
	{
		float err = 0;

		for (int j = 0; j < size(); j++)
		{
			linkedNetwork->Propagate(GetTestValues(j));
			linkedNetwork->Backpropagate(GetTestTargets(j));
			for (int z = 0; z < linkedNetwork->GetOutputs().size(); z++) err += pow(GetTestTargets(j)[z] - linkedNetwork->GetOutputs()[z], 2);
		}

		if (epochs > 1)
		{
			if (i % space == 0)
				std::cout << "ERROR: " << err << ", EPOCH: " << i << std::endl;
		}
		else
			std::cout << "ERROR: " << err << ", EPOCH: " << i << std::endl;
	}

	std::cout << "Done" << std::endl;
}

void Data::Test(Data * data)
{
	float win = 0;

	if (data == NULL)
		data = this;

	std::cout << "-----------------------------------------" << std::endl;
	for (int i = 0; i < data->size(); i++)
	{
		linkedNetwork->Propagate(data->GetTestValues(i));

		bool success = true;

		std::cout << "OUTPUTS: " << std::endl;
		for (int j = 0; j < linkedNetwork->GetOutputs().size(); j++)
		{
			std::cout << (j + 1) << ":\t" << "Desired: ";
			std::cout << data->GetTestTargets(i)[j] << "\t";
			std::cout << "Returned: ";
			std::cout << linkedNetwork->GetOutputs()[j] << std::endl;
		}

		std::cout << "INTERPRETATION:\t";
		std::vector<DATA> inter = data->InterpretOutput(linkedNetwork->GetOutputs());
		std::cout << "{ ";
		for (int j = 0; j < inter.size(); j++)
		{
			if (inter[j].isString())
				std::cout << (std::string)inter[j] << " ";
			else
				std::cout << (float)inter[j] << " ";
		}
		std::cout << "}" << std::endl;

		std::cout << "EXPECTED:\t";
		std::vector<DATA> exp = data->InterpretOutput(data->GetTestTargets(i));
		std::cout << "{ ";
		for (int j = 0; j < exp.size(); j++)
		{
			if (exp[j].isString())
				std::cout << (std::string)exp[j] << " ";
			else
				std::cout << (float)exp[j] << " ";
		}
		std::cout << "}" << std::endl;

		std::cout << std::endl;

		for (int j = 0; j < inter.size(); j++)
		{
			if (exp[j].isString())
			{
				if ((std::string)exp[j] != (std::string)inter[j]) success = false;
			}
			else
			{
				if ((float)exp[j] != (float)inter[j]) success = false;
			}
		}

		if (success) win++;
	}

	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "Successes: " << win << "/" << data->size() << " - " << ((win / data->size())*100.0) << "%" << std::endl;
}
