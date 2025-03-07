/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Basic_EQ_RobertAudioProcessor::Basic_EQ_RobertAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Basic_EQ_RobertAudioProcessor::~Basic_EQ_RobertAudioProcessor()
{
}

//==============================================================================
const juce::String Basic_EQ_RobertAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Basic_EQ_RobertAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Basic_EQ_RobertAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Basic_EQ_RobertAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Basic_EQ_RobertAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Basic_EQ_RobertAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Basic_EQ_RobertAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Basic_EQ_RobertAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Basic_EQ_RobertAudioProcessor::getProgramName (int index)
{
    return {};
}

void Basic_EQ_RobertAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Basic_EQ_RobertAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void Basic_EQ_RobertAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Basic_EQ_RobertAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Basic_EQ_RobertAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    //(deleted some stuff that came with the framework to input tutorial junk)

    juce::dsp::AudioBlock<float> block(buffer); //"wrap" our buffer in a "block"

    // extract the 2 channels from the block into 2 sperate blocks
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

}

//==============================================================================
bool Basic_EQ_RobertAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Basic_EQ_RobertAudioProcessor::createEditor()
{
    //return new Basic_EQ_RobertAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Basic_EQ_RobertAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Basic_EQ_RobertAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Basic_EQ_RobertAudioProcessor();
}


// We need to get a NormalizableRange to specify parameters for how our knob will be positioned and move
// We will use our NormalizableRange to create an AudioParameterFloat, by giving it an ID, a name, our Normalizable range our and a default value
// We will make a unique pointer to this float, and pass that to our layout.add()
// layout is a ParameterLayout Object
// this fumction returns a parameter layout, we will 
juce::AudioProcessorValueTreeState::ParameterLayout 
Basic_EQ_RobertAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Layout.add() needs a pointer to an AudioParameterFloat 
    // We use "make_unique" to give a pointer to the parameter we want to add to our layout
    layout.add(
        std::make_unique<juce::AudioParameterFloat> // AudioParameterFloat() constructor takes: ParameterID &parameterID, String &parameterName, <float>normalisableRange normalisableRange , float defaultValue
        ("LowCut Freq", "LowCut Freq",
            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), // NormalizableRange takes the values: ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false
            20.f));

    layout.add(
        std::make_unique<juce::AudioParameterFloat> // AudioParameterFloat() constructor takes: ParameterID &parameterID, String &parameterName, <float>normalisableRange normalisableRange , float defaultValue
        ("HighCut Freq", "HighcutCut Freq",
            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), // NormalizableRange takes the values: ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false
            20.f));

    layout.add(
        std::make_unique<juce::AudioParameterFloat> // AudioParameterFloat() constructor takes: ParameterID &parameterID, String &parameterName, <float>normalisableRange normalisableRange , float defaultValue
        ("Peak Freq", "Peak Freq",
            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), // NormalizableRange takes the values: ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false
            750.f));

    layout.add(
        std::make_unique<juce::AudioParameterFloat> // AudioParameterFloat() constructor takes: ParameterID &parameterID, String &parameterName, <float>normalisableRange normalisableRange , float defaultValue
        ("Peak Gain", "Peak Gain",
            juce::NormalisableRange<float>(-24.f, 24.f, .05f, 1.f), // NormalizableRange takes the values: ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false
            0.f));

    layout.add(
        std::make_unique<juce::AudioParameterFloat> // AudioParameterFloat() constructor takes: ParameterID &parameterID, String &parameterName, <float>normalisableRange normalisableRange , float defaultValue
        ("Peak Quality", "Peak Quality",
            juce::NormalisableRange<float>(-24.f, 24.f, .05f, 1.f), // NormalizableRange takes the values: ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false
            0.f));

    juce::StringArray stringArray;
    for (int i = 0; i < 4; i++)
    {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    return layout;
}