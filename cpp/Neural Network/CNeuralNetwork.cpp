#include "neuralnetwork.h"

NeuralNetwork::NeuralNetwork(int inputs, int ouputs, int hidden, int perhidden)
{
	inputLayer = new NeuralLayer(inputs);

	if (hidden > 0)
	{
		hiddenLayers.push_back(new NeuralLayer(inputLayer, perhidden));

		for (int i = 0; i < hidden - 1; i++)
			hiddenLayers.push_back(new NeuralLayer(hiddenLayers[i], perhidden));

		outputLayer = new NeuralLayer(hiddenLayers[hiddenLayers.size() - 1], ouputs);
	} else
		outputLayer = new NeuralLayer(inputLayer, ouputs);
}

NeuralNetwork::~NeuralNetwork()
{
	if (inputLayer!=NULL)
		delete inputLayer;

	for (int i = 0; i < hiddenLayers.size(); i++)
	{
		delete hiddenLayers[i];
	}

	if (outputLayer != NULL)
		delete outputLayer;
}

void NeuralNetwork::Propagate(std::vector<float> inputs)
{
	if (inputs.size() != inputLayer->GetNeurons().size())
	{
		std::cout << "Invalid input amount!" << std::endl;
		return;
	}

	for (int i = 0; i < inputLayer->GetNeurons().size(); i++)
		inputLayer->GetNeurons()[i]->SetOutput(inputs[i]);

	for (int i = 0; i < hiddenLayers.size(); i++)
		hiddenLayers[i]->Pass();

	outputLayer->Pass();
}

std::vector<float> NeuralNetwork::GetOutputs()
{
	return outputLayer->GetOutputs();
}

std::string NeuralNetwork::Print()
{
	std::string s;

	s = s + inputLayer->Print() + " -> ";

	for (int i = 0; i < hiddenLayers.size(); i++)
		s = s + hiddenLayers[i]->Print() + " -> ";

	s = s + outputLayer->Print();

	return s;
}

void NeuralNetwork::Backpropagate(std::vector<float> desired)
{
	float LEARNING_RATE = 0.5f;

	//Get Errors
	for (int i = 0; i < outputLayer->GetNeurons().size(); i++)
	{
		outputLayer->GetNeurons()[i]->delta = ((desired[i] - outputLayer->GetNeurons()[i]->GetOutput()) * outputLayer->GetNeurons()[i]->OutputDerivative());
	}

	for (int i = hiddenLayers.size() - 1; i >= 0; i--)
	{
		NeuralLayer * layer = hiddenLayers[i];
		NeuralLayer * layer2;
		if (i < hiddenLayers.size() - 1)
			layer2 = hiddenLayers[i + 1];
		else
			layer2 = outputLayer;

		for (int j = 0; j < layer->GetNeurons().size(); j++)
		{
			Neuron * n = layer->GetNeurons()[j];
			float err = 0;

			for (int k = 0; k < layer2->GetNeurons().size(); k++)
			{
				Neuron * n2 = layer2->GetNeurons()[k];
				float w = n2->GetWeight(n);
				err += w * n2->delta;
			}

			n->delta = err * n->OutputDerivative();
		}
	}

	//Fix Weights
	for (int i = 0; i < outputLayer->GetNeurons().size(); i++)
	{
		Neuron * n = outputLayer->GetNeurons()[i];

		for (int j = 0; j < n->inputVector.size(); j++)
		{
			n->weightVector[j] += LEARNING_RATE * n->delta * n->inputVector[j]->GetOutput();
		}

		n->weightVector[n->weightVector.size() - 1] += LEARNING_RATE * n->delta * BIAS;
	}

	for (int k = 0; k < hiddenLayers.size(); k++)
	{
		for (int i = 0; i < hiddenLayers[k]->GetNeurons().size(); i++)
		{
			Neuron * n = hiddenLayers[k]->GetNeurons()[i];

			for (int j = 0; j < n->inputVector.size(); j++)
			{
				n->weightVector[j] += LEARNING_RATE * n->delta * n->inputVector[j]->GetOutput();
			}

			n->weightVector[n->weightVector.size() - 1] += LEARNING_RATE * n->delta * BIAS;
		}
	}
}

std::vector<float> NeuralNetwork::GetWeights()
{
	std::vector<float> weights;

	for (int i = 0; i < hiddenLayers.size(); i++)
	{
		for (int j = 0; j < hiddenLayers[i]->GetNeurons().size(); j++)
		{
			Neuron * n = hiddenLayers[i]->GetNeurons()[j];

			for (int k = 0; k < n->GetWeights().size(); k++)
			{
				weights.push_back(n->GetWeights()[k]);
			}
		}
	}

	for (int j = 0; j < outputLayer->GetNeurons().size(); j++)
	{
		Neuron * n = outputLayer->GetNeurons()[j];

		for (int k = 0; k < n->GetWeights().size(); k++)
		{
			weights.push_back(n->GetWeights()[k]);
		}
	}

	return weights;
}

void NeuralNetwork::PutWeights(std::vector<float> weights)
{
	int c = 0;

	for (int i = 0; i < hiddenLayers.size(); i++)
	{
		for (int j = 0; j < hiddenLayers[i]->GetNeurons().size(); j++)
		{
			Neuron * n = hiddenLayers[i]->GetNeurons()[j];

			for (int k = 0; k < n->GetWeights().size(); k++)
			{
				n->GetWeights()[k] = weights[c++];
			}
		}
	}

	for (int j = 0; j < outputLayer->GetNeurons().size(); j++)
	{
		Neuron * n = outputLayer->GetNeurons()[j];

		for (int k = 0; k < n->GetWeights().size(); k++)
		{
			n->GetWeights()[k] = weights[c++];
		}
	}
}