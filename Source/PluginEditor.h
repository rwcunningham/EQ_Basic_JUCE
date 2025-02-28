/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Basic_EQ_RobertAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Basic_EQ_RobertAudioProcessorEditor (Basic_EQ_RobertAudioProcessor&);
    ~Basic_EQ_RobertAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Basic_EQ_RobertAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Basic_EQ_RobertAudioProcessorEditor)
};
