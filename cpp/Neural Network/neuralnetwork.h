/**
==========================================================================================

	CS-17
	Neural Network

	@author Benjamin Park
	@version 1.1 3/25/17

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
to computation and floats can only go down like 7 decimal places or something. Personally
that should give you enough space but what do I know I'm just header file.

	So now that we've explained some things, let's go over a worked example:

	Here is our Neural Network N, let's give it two inputs {a, b}, one output {c}, and no
hidden layers:

	{a, b} -> {c(0.4, 0.6)}

	That's the basic idea. Now, take a look at the output c. I have given it 2 weights to
Match the amount of inputs. So c would be calculated by

	c = sig(0.4a + 0.6b)

	>> sig(x) = 1 / ( 1 + e^( -x / t ) )*
	>> t represents the activation response value (usually 1, the closer to 0 the more it
		approximates the step function)

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

	* This is commonly called the Sigmoid function, which returns a value from 0 to 1, and
	is used in this Neural Network. Some NNs use different activation functions so that the
	NN can output values of any range, or to speed up the calculations by doing less math.
	The Sigmoid is most common so I've implimented it, but if you so desire you can change
	it in the code (Make sure D_SIGMOID is changed too, the derivative of the function is
	important for backpropogation).

==========================================================================================
USAGE:

	Pretty simple, just invoke the NeuralNetwork() constructor, with the values input amount,
output amount, hidden layer amount, and the amount of neurons in the hidden layer:

	NeuralNetwork n(2, 1, 1, 4);

	This creates a NN with the parameters:

	{a, b} -> {ha, hb, hc, hd} -> {c}

	The weights begin randomized but can be easily adjusted. GetWeights() returns a vector
of the weights as floats and PutWeights() sends them to the network using the same format.
So you could to minor random adjustments by iterating through a vector from GetWeights()
and resubmitting it using PutWeights().

	The method Propagate() sends the inputs through the Neural Network, and Backpropagate()
sends the outputs back for training. In order to retrieve the outputs from the Neural
Network you can use GetOutputs() which returns a vector of floats (ranging from 0 to 1,
of course).

	Luckily, training and operating a Neural Network can get a little complicated, so while
you are free to manually operate the Neural Network, the Data class supplies an easier
way to accomplish these methods.

Data Functions:

	struct DATA - The means of storing raw data values: DATA() takes a string or a number,
		and is important when you use the Data class because lots of the arguments use
		a std::vector<DATA>.

	Data(std::string filename) - Sets up a Data object based on the contents of a file,
		optionally you can specify two arguements, one being the deliminator that seper-
		ates the field, and a bool to specify whether the targets should be forced as
		categories or stay as numbers (i.e., if you had 1, 2, 3, 4 etc. and you didn't
		want the Neural Network to guess 1.5 or 2.342524324 etc.)

	CreateLinkedNetwork(NeuralNetwork &network) - Sets up a NeuralNetwork controlled by
		and linked to the Data set, which is important for training and testing the network.
		The Neural Network will be set up with the necessary input and output amounts, and
		calculates the amount of Neurons that should be inside the hidden layers. By default
		the number of hidden layers is 1 but a lesser or higher amount can be specified as
		a second argument.

	Train(int epochs) - Self-explanatory, trains the Neural Network linked to the Data set
		for the number of epochs specified.

	Test() - Tests the linked Neural Network with the Data object by default. A different
		Data object can be entered and that set will be used for testing. An error will
		occur if the Data object that you enter does not match the necessary number of
		inputs/outputs.

	AddData(std::vector<DATA> inputs, std::vector<DATA> outputs) - Adds a set of data
		values to the Data set. Uses the DATA struct.

	Invoke(std::vector<DATA> inputs) - Enters a vector of values into the Neural Network
		and returns a vector of the outputs. Uses and returns the DATA struct.

	Print() - Outputs the entire set of data values.

	Info() - Outputs information about the Data set, usually necessary just for debugging
		problems.

==========================================================================================
*/

#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <bitset>

/* BIAS value for the Neurons, usually set to 1 */
const int BIAS = 1;
/* Activation value for the Sigmoid equation */
const int ACTIVATION = 1;

#define	SIGMOID(a)		1.0f / ( 1.0f + exp(-a / ACTIVATION))
#define	D_SIGMOID(a)	a * ( 1.0f - a )

/*
==========================================================================================
PROTOTYPING:
*/

/* The DATA structure, not to be confused with the Data class */
struct DATA;
/* The Data class, easy Neural Network development */
class Data;
/* The Neuron class, would probably not be used unless for debugging */
class Neuron;
/* The NeuralLayer class, which like the Neuron class would probably not be used but for debugging */
class NeuralLayer;
/* The NeuralNetwork class, builds the Network without being linked to a Data class */
class NeuralNetwork;

/*
==========================================================================================
CLASSES/STRUCTS:
*/

struct DATA
{
	float raw_float;
	std::string raw_string;
	int type;
	DATA(std::string s);
	DATA(float f);
	operator std::string();
	operator float();
	bool isString();
	std::string Print();
};

class Neuron
{
private:
	float output;
	float pre_sum;
	void Create();
	bool hasBias = false;
public:
	Neuron(std::vector<Neuron *> neurons);
	Neuron();
	void PutWeights(std::vector<float> weights);
	void Pass();
	float GetOutput();
	void SetOutput(float a);
	float OutputDerivative();
	std::string Print();
	float GetWeight(Neuron *n);
	float delta;
	std::vector<Neuron *> inputVector;
	std::vector<float> weightVector;
	std::vector<float> GetWeights();
};

class NeuralLayer
{
private:
	NeuralLayer * inputLayer;
	std::vector<Neuron *> neurons;
public:
	NeuralLayer(NeuralLayer * input, int n);
	NeuralLayer(int n);
	~NeuralLayer();
	std::vector<Neuron *> GetNeurons();
	void Pass();
	std::vector<float> GetOutputs();
	std::string Print();
};

class NeuralNetwork
{
private:
	NeuralLayer * inputLayer;
	NeuralLayer * outputLayer;
	std::vector<NeuralLayer *> hiddenLayers;
public:
	NeuralNetwork(int inputs, int ouputs, int hidden=0, int perhidden=0);
	~NeuralNetwork();
	void Propagate(std::vector<float> inputs);
	void Backpropagate(std::vector<float> inputs);
	std::vector<float> GetOutputs();
	std::vector<float> GetWeights();
	void PutWeights(std::vector<float> weights);
	std::string Print();
};

class Data
{
private:
	std::vector<std::vector<DATA>> dataValues;
	std::vector<std::vector<DATA>> targetValues;
	std::vector<float> min_O, max_O;
	std::vector<float> min_I, max_I;
	std::vector<std::vector<std::string>> cat_I;
	std::vector<std::vector<std::string>> cat_O;
	std::vector<int> indices_O;
	std::vector<int> indices_I;
	std::vector<std::map<std::string, std::vector<int>>> binary_I;
	std::vector<std::map<std::string, std::vector<int>>> binary_O;
	int size_O;
	int size_I;
	int binary_to_decimal(int num);
	bool IsFloat(std::string myString);
	NeuralNetwork * linkedNetwork = NULL;
	void Transform();
	float RevTarget(float a, int j)
	{
		if (targetValues[0][j].isString())
			return round(a * (max_O[j] - min_O[j]) + min_O[j]);
		else
			return (a * (max_O[j] - min_O[j]) + min_O[j]);
	}
	std::string GetCatI(int i, int j)
	{
		return cat_I[i][j];
	}
	std::string GetCatO(int i, int j)
	{
		return cat_O[i][j];
	}
public:
	Data(std::string filename, std::string delim="\t", bool FORCE_CAT=false);
	Data();
	~Data();
	void AddData(std::vector<DATA> values, std::vector<DATA> targets);
	std::vector<float> GetTestValues(int i);
	std::vector<float> GetTestTargets(int i);
	int size() { return dataValues.size(); }
	void Print();
	void CreateLinkedNetwork(NeuralNetwork * &network, int layers = 1);
	std::vector<DATA> GetRawDataValues(int i) { return dataValues[i]; }
	std::vector<DATA> GetRawDataTargets(int i) { return targetValues[i]; }
	std::vector<DATA> InterpretOutput(std::vector<float> output);
	std::vector<DATA> Invoke(std::vector<DATA> input);
	int InputSize() { return size_I; }
	int OutputSize() { return size_O; }
	void Info();
	void Train(int epochs);
	void Test(Data * data=NULL);
};

#endif
