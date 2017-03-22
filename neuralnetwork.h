#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

/* 
An Application of the Neural Network for C++
Benjamin Park - 2017

TODO:
Backpropogation, Supervised Learning

==========================================================================================
PRIMER - WHAT IS A NEURAL NETWORK?

	Being the lucky fellow that you are, you've successfully downloaded a patchwork of
terrible and incoherent functions that amount to a simple Neural Network. But maybe you
don't quite know what a Neural Network is exactly. I spent a lot of time trying to figure
out what it was too, thinking it was some mystical computer science terrifying code that
only the masters could make. However, it's fairly simple.

	The brain (of a living creature) operates by firing neurons amounting to concious or
unconcious thoughts. The neurons are connected by these wire-like things called dendrites,
which carry the electrical signal emitted by one neuron to the next. Lots of times, neurons
in the brain have multiple inputs and output to multiple other neurons. When a neuron has
multiple inputs, the neuron adds up all the inputs multiplied by their corresponding weights.
The 'weights' in the brain are the strength of the dendrites that carry the electrical
signal. The stronger the 'weight' the more preference the neuron has towards that signal.
So if you've got one input with a weight of say, 0.5 and another has a weight of 100 (very
realistic scenario) then the former has really no impact with regards to how the neuron
outputs. In the brain, like a Neural Network, neurons can have hundreds of connections, and
although a little slow for a computer, it is completely possible. I think the only difference
between a brain and a Neural Network (at least this one) the simulated Neurons are connected
into layers and cannot interact with each other if they are within the same layer. The
brain doesn't work in layers, which is cool and I wish I could do that but that's later on.

	Anyways, the neuron takes all the inputs and their weights, adding them up and putting
them through a function to determine whether the neuron fires. In the brain, this is called
the all-or-nothing-principle, where the neuron only fires after a certain threshold and
fires completely. The difference here is that the neural network fires from a range between
0 to 1 (computed by the Sigmoid function) and sends that to the next layer. Early NN, to my
knowledge, did fire in binary, but the real brain is much more complex than that anyways so
it was found to be much more effective when the output was within that range. FYI, the brain
approximates to binary, which explains the original development, but the approximation does
not fully allow for brain simulation. Brain is still not understood, yadda yadda ask a
biologist. 

	Eventually the NN leads to the outputs specified which are neurons and therefore give
out between 0 and 1. NNs that try to categorize between a set of things, i.e. {Cat, Dog},
will train to assign ranges of values between 0 and 1 to each choice, i.e. {(0, 0.5), [0.5,1)}.
Because there are an infinite amount of values between 0 and 1, then there can be an infinite
amount of categories. That is theoretically speaking, but we all know that there are limits
to computation and doubles can only go down like 7 decimal places or something. Personally
that should give you enough space but what do I know I'm just header file.

	So now that we've explained some things, let's go over a worked example:

	Here is our Neural Network N, let's give it two inputs {a, b}, one output {c}, and no
hidden layers:

	{a, b} -> {c(0.4, 0.6)}

	That's the basic idea. Now, take a look at the output c. I have given it 2 weights to
Match the amount of inputs. So c would be calculated by
	
	c = sig(0.4a + 0.6b)

		>> sig(x) = 1 / ( 1 + e^( -x / t ) )
		>> t represents the activation response value (usually 1)

	So with an input of 1 for a and 2 for b, c would be equal to sig(1.6). It's the same
principle for each neuron, including those in the hidden layers. We can easily add a hidden
layer:

	{a, b} -> {h1a(0.1, 0.2), h1b(0.3, 0.4), ..., h1n(p, q)} -> {c}

		>> h1a = sig(0.1a + 0.2b)
		>> h1b = sig(0.3a + 0.4b)
		>> ...
		>> h1n = sig(p * a + q * b)

	And the layers then continue on, the next one having inputs equal to the amount of
neurons in the previous.

	So what about weights? In a NN everything is dependent on the weights, which start out
randomized. As the NN improves, the weights are adjusted accordingly. In supervised train-
ing they are adjusted according to an error function during back propogation, while in
unsupervised learning, the NN adjusts weights running multiple instances and seeing which
does best, determined by a 'fitness' value that the implimenter uses. The idea is that
eventually, if the fitness value is well chosen, the NN should approach its ideal form, a
form that maximized the fitness value.

	Now we can see that Neural Networks aren't so complicated, they're just small algorithms
to replicate the functions of a brain.

==========================================================================================
USAGE:

	Pretty simple, just invoke the NeuralNetwork() constructor, with the values input amount, 
output amount, hidden layer amount, and the amount of neurons in the hidden layer:

	NeuralNetwork n(2, 1, 1, 4);

	This creates a NN with the parameters:

	{a, b} -> {ha, hb, hc, hd} -> {c}

	The weights begin randomized but can be easily adjusted. GetWeights() returns a vector
of the weights as doubles and PutWeights() sends them to the network using the same format.
So you could to minor random adjustments by iterating through a vector from GetWeights()
and resubmitting it using PutWeights().
	
	The Invoke() methods is what really does the magic, processing a set of inputs and
returning a vector of the outputs.

==========================================================================================
*/

#include <vector>;
#include <random>;

//------------------------------------------
//CONSTANTS:

const double lowerBoundRandom = -1;
const double upperBoundRandom = 1;
const double dBias = -1;
const double dActivationResponse = 1;

//------------------------------------------
//PROTOTYPING:

struct Neuron; /* Simulates a Neuron */
struct NeuralLayer; /* Simulates a layer of Neurons */
class NeuralNetwork; /* The Neural Network itself, that forms and operates */

//------------------------------------------
//DECLARATIONS:

class Neuron
{
public:
	int numberOfInputs;
	float netOutput;
	std::vector<double> inputWeights;
	double Sigmoid(double netinput, double response) { return (1 / (1 + exp(-netinput / response))); }
	Neuron(int numberOfInputs);
	double Pass(std::vector<double> inputs);
};

class NeuralLayer
{
public:
	int numberOfNeurons;
	std::vector<Neuron> Neurons;
	NeuralLayer(int numberOfNeurons, int numberOfInputsPerNeuron);
	std::vector<double> Pass(std::vector<double> inputs);
};

class NeuralNetwork
{
private:
	int numberOfInputs;
	int numberOfOutputs;
	int numberOfHiddenLayers;
	int neuronsPerHiddenLayer;
	std::vector<NeuralLayer> neuralLayers;
	void CreateNetwork();
public:
	NeuralNetwork(int inputs, int outputs, int hidden, int perhidden);
	NeuralNetwork(int inputs, int outputs);
	std::vector<double> GetWeights();
	void PutWeights(std::vector<double> weights);
	std::vector<double> Invoke(std::vector<double> inputs);
	int getNumberOfInputs() { return numberOfInputs; }
	int getnNumberOfOutputs() { return numberOfOutputs; }
	int getNumberOfLayers() { return numberOfHiddenLayers; }
	int getNumberOfNeurons() { return neuronsPerHiddenLayer; }
};

//------------------------------------------
//CONSTRUCTORS:

Neuron::Neuron(int num)
{
	//There needs to be a bias neuron, ergo +1
	numberOfInputs = num + 1;

	//Random generators in C++ are a pain
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> distribution(lowerBoundRandom, upperBoundRandom);

	for (int i = 0; i<num + 1; ++i)
	{
		//Set up the weights with an initial random value between lower and upper bound
		inputWeights.push_back(distribution(mt));
	}
}

NeuralLayer::NeuralLayer(int numberOfNeurons, int numberOfInputsPerNeuron)
{
	numberOfNeurons = numberOfNeurons;

	for (int i = 0; i < numberOfNeurons; ++i)
	{
		Neurons.push_back(Neuron(numberOfInputsPerNeuron));
	}
}

NeuralNetwork::NeuralNetwork(int inputs, int outputs, int hidden, int perhidden)
{
	numberOfInputs = inputs;
	numberOfOutputs = outputs;
	numberOfHiddenLayers = hidden;
	neuronsPerHiddenLayer = perhidden;

	CreateNetwork();
}

NeuralNetwork::NeuralNetwork(int inputs, int outputs)
{
	NeuralNetwork(inputs, outputs, 0, 0);
}

//------------------------------------------
//METHODS:

double Neuron::Pass(std::vector<double> inputs)
{
	double output = 0;

	for (int i = 0; i < numberOfInputs - 1; i++)
	{
		output += inputWeights[i] * inputs[i];
	}

	output += dBias * inputWeights[numberOfInputs - 1];

	return Sigmoid(output, dActivationResponse);
}

std::vector<double> NeuralLayer::Pass(std::vector<double> inputs)
{
	std::vector<double> outputs;

	for (int i = 0; i < Neurons.size(); i++)
	{
		Neuron n = Neurons[i];

		outputs.push_back(n.Pass(inputs));
	}

	return outputs;
}

void NeuralNetwork::CreateNetwork()
{
	//You don't necessarily need a hidden layer unless you're a special complex little boy
	if (numberOfHiddenLayers > 0)
	{
		//The first layer must be created with the number of inputs equal to the number of inputs to the network itself
		neuralLayers.push_back(NeuralLayer(neuronsPerHiddenLayer, numberOfInputs));

		for (int i = 0; i < numberOfHiddenLayers - 1; ++i)
		{
			neuralLayers.push_back(NeuralLayer(neuronsPerHiddenLayer, neuronsPerHiddenLayer));
		}

		neuralLayers.push_back(NeuralLayer(numberOfOutputs, neuronsPerHiddenLayer));
	}
	else
	{
		neuralLayers.push_back(NeuralLayer(numberOfOutputs, numberOfInputs));
	}
}

std::vector<double> NeuralNetwork::Invoke(std::vector<double> inputs)
{
	std::vector<double> outputs;
	int cWeight = 0;

	if (inputs.size() != numberOfInputs) return outputs;

	for (int i = 0; i < numberOfHiddenLayers + 1; i++)
	{
		outputs.clear();

		outputs = neuralLayers[i].Pass(inputs);

		inputs = outputs;
	}

	return outputs;
}

void NeuralNetwork::PutWeights(std::vector<double> weights)
{
	int cWeight = 0;

	for (int i = 0; i<numberOfHiddenLayers + 1; ++i)
	{
		for (int j = 0; j<neuralLayers[i].numberOfNeurons; ++j)
		{
			for (int k = 0; k<neuralLayers[i].Neurons[j].numberOfInputs; ++k)
			{
				neuralLayers[i].Neurons[j].inputWeights[k] = weights[cWeight++];
			}
		}
	}
}

std::vector<double> NeuralNetwork::GetWeights()
{
	std::vector<double> weights;

	for (int i = 0; i<numberOfHiddenLayers + 1; ++i)
	{
		for (int j = 0; j<neuralLayers[i].numberOfNeurons; ++j)
		{
			for (int k = 0; k<neuralLayers[i].Neurons[j].numberOfInputs; ++k)
			{
				weights.push_back(neuralLayers[i].Neurons[j].inputWeights[k]);
			}
		}
	}

	return weights;
}

#endif
