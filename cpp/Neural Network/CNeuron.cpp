#include "neuralnetwork.h"
#include <random>

Neuron::Neuron(std::vector<Neuron *> neurons)
{
	this->inputVector = neurons;
	this->hasBias = true;

	Create();
}

Neuron::Neuron()
{ 
	output = -1; 
}

void Neuron::Create()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> distribution(-0.5, 0.5);

	for (int i = 0; i < inputVector.size(); i++)
	{
		weightVector.push_back(distribution(mt));
	}

	if (hasBias)
		weightVector.push_back(distribution(mt));

	output = -1;
}

void Neuron::PutWeights(std::vector<float> weights)
{
	if (weights.size() != inputVector.size())
	{
		std::cout << "Cannot put weights, recieved " << weights.size() << ", needed " << inputVector.size() << std::endl;
	}
	else
	{
		weightVector = weights;
	}
}

void Neuron::Pass()
{
	if (inputVector.size() == 0)
		return;

	float sum = 0;

	for (int i = 0; i < inputVector.size(); i++)
		sum += inputVector[i]->GetOutput() * weightVector[i];

	if (hasBias)
		sum += weightVector[weightVector.size() - 1] * BIAS;

	pre_sum = sum;

	output = SIGMOID(sum);
}

void Neuron::SetOutput(float a)
{
	output = a;
}

float Neuron::GetOutput()
{
	return output;
}

float Neuron::OutputDerivative()
{
	return D_SIGMOID(output);
}

float Neuron::GetWeight(Neuron *n)
{
	if (std::find(inputVector.begin(), inputVector.end(), n) == inputVector.end())
		return 0;

	int index = std::find(inputVector.begin(), inputVector.end(), n) - inputVector.begin();

	return weightVector[index];
}

std::vector<float> Neuron::GetWeights()
{
	return weightVector;
}

std::string Neuron::Print()
{
	std::string s;

	if (weightVector.size() > 0)
	{
		s = "(";

		for (int i = 0; i < weightVector.size() - 1; i++)
		{
			s = s + std::to_string(weightVector[i]) + ", ";
		}

		if (hasBias)
			s = s + "B_";

		s = s + std::to_string(weightVector[weightVector.size() - 1]) + ")";
	}
	else
	{
		s = "i";
	}

	if (output > -1)
	{
		s = s + "=" + std::to_string(output);
	}

	return s;
}