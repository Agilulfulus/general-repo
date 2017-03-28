#include "neuralnetwork.h"

NeuralLayer::NeuralLayer(NeuralLayer * input, int n)
{
	std::vector<Neuron *> inputNeurons = input->GetNeurons();

	for (int i = 0; i < n; i++)
		neurons.push_back(new Neuron(inputNeurons));
}

NeuralLayer::NeuralLayer(int n)
{
	for (int i = 0; i < n; i++)
		neurons.push_back(new Neuron());
}

NeuralLayer::~NeuralLayer()
{
	for (int i = 0; i < neurons.size(); i++)
	{
		delete neurons[i];
	}
}

void NeuralLayer::Pass()
{
	for (int i = 0; i < neurons.size(); i++)
		neurons[i]->Pass();
}

std::vector<float> NeuralLayer::GetOutputs()
{
	std::vector<float> outputs;

	for (int i = 0; i < neurons.size(); i++)
		outputs.push_back(neurons[i]->GetOutput());

	return outputs;
}

std::vector<Neuron *> NeuralLayer::GetNeurons()
{
	return neurons;
}

std::string NeuralLayer::Print()
{
	std::string s = "{";

	for (int i = 0; i < neurons.size() - 1; i++)
		s = s + neurons[i]->Print() + ", ";

	s = s + neurons[neurons.size() - 1]->Print() + "}";

	return s;
}