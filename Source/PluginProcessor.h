#pragma once

#include <JuceHeader.h>
#include "PreampDSP.h"

#include "DirtEQDSP.h"
#include "NeveEQDSP.h"
#include "PultecEQDSP.h"
#include "ModernEQDSP.h"

#include "NC76Compressor.h"
#include "LA2ACompressor.h"
#include "FairchildCompressor.h"
#include "ModernCompressorDSP.h"
#include "OutputDSP.h"
#include <juce_dsp/juce_dsp.h>

class AnalogFxAudioProcessor : public juce::AudioProcessor
{
public:
    AnalogFxAudioProcessor();
    ~AnalogFxAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Parameter Cache for extreme performance
    std::atomic<float>* preTypePtr = nullptr;
    std::atomic<float>* preDrivePtr = nullptr;
    std::atomic<float>* preTrimPtr = nullptr;
    std::atomic<float>* preAutoPtr = nullptr;
    
    std::atomic<float>* compTypePtr = nullptr;
    std::atomic<float>* compTrimPtr = nullptr;
    
    // NC76
    std::atomic<float>* nc76ThreshPtr = nullptr;
    std::atomic<float>* nc76RatioPtr = nullptr;
    std::atomic<float>* nc76AttackPtr = nullptr;
    std::atomic<float>* nc76ReleasePtr = nullptr;
    std::atomic<float>* nc76InGainPtr = nullptr;
    std::atomic<float>* nc76OutGainPtr = nullptr;
    std::atomic<float>* nc76MixPtr = nullptr;
    
    // LA2A
    std::atomic<float>* la2aPeakPtr = nullptr;
    std::atomic<float>* la2aGainPtr = nullptr;
    std::atomic<float>* la2aRatioPtr = nullptr;
    
    // Fairchild
    std::atomic<float>* fcLTPtr = nullptr;
    std::atomic<float>* fcRTPtr = nullptr;
    std::atomic<float>* fcLBPtr = nullptr;
    std::atomic<float>* fcRBPtr = nullptr;
    std::atomic<float>* fcLMPtr = nullptr;
    std::atomic<float>* fcRMPtr = nullptr;
    std::atomic<float>* fcAGCPtr = nullptr;
    std::atomic<float>* fcLTiPtr = nullptr;
    std::atomic<float>* fcRTiPtr = nullptr;
    std::atomic<float>* fcLRMSPtr = nullptr;
    std::atomic<float>* fcRRMSPtr = nullptr;
    
    // Modern Comp
    std::atomic<float>* mcThreshPtr = nullptr;
    std::atomic<float>* mcRatioPtr = nullptr;
    std::atomic<float>* mcAttackPtr = nullptr;
    std::atomic<float>* mcReleasePtr = nullptr;
    std::atomic<float>* mcMakeupPtr = nullptr;
    
    std::atomic<float>* eqTypePtr = nullptr;
    std::atomic<float>* eqTrimPtr = nullptr;
    
    // Dirt EQ
    std::atomic<float>* deLPFPtr = nullptr;
    std::atomic<float>* deLPQPtr = nullptr;
    std::atomic<float>* deB1GPtr = nullptr;
    std::atomic<float>* deB1FPtr = nullptr;
    std::atomic<float>* deB2GPtr = nullptr;
    std::atomic<float>* deB2FPtr = nullptr;
    std::atomic<float>* deB2QPtr = nullptr;
    std::atomic<float>* deB3GPtr = nullptr;
    std::atomic<float>* deB3FPtr = nullptr;
    std::atomic<float>* deB3QPtr = nullptr;
    std::atomic<float>* deB4GPtr = nullptr;
    std::atomic<float>* deB4FPtr = nullptr;
    std::atomic<float>* deHPFPtr = nullptr;
    std::atomic<float>* deHPQPtr = nullptr;
    std::atomic<float>* deIC1Ptr = nullptr;
    std::atomic<float>* deIC2Ptr = nullptr;
    std::atomic<float>* deDrivePtr = nullptr;
    std::atomic<float>* deGainPtr = nullptr;
    std::atomic<float>* deOnPtr = nullptr;
    
    // Neve EQ
    std::atomic<float>* nvGainPtr = nullptr;
    std::atomic<float>* nvHFGPtr = nullptr;
    std::atomic<float>* nvMFGPtr = nullptr;
    std::atomic<float>* nvMFFPtr = nullptr;
    std::atomic<float>* nvLFGPtr = nullptr;
    std::atomic<float>* nvLFFPtr = nullptr;
    std::atomic<float>* nvHPFPtr = nullptr;
    std::atomic<float>* nvDrivePtr = nullptr;
    std::atomic<float>* nvPhasePtr = nullptr;
    std::atomic<float>* nvHFOnPtr = nullptr;
    std::atomic<float>* nvMFOnPtr = nullptr;
    std::atomic<float>* nvLFOnPtr = nullptr;
    std::atomic<float>* nvHPFOnPtr = nullptr;
    std::atomic<float>* nvEqOnPtr = nullptr;
    
    // Pultec
    std::atomic<float>* puLSFPtr = nullptr;
    std::atomic<float>* puLBPtr = nullptr;
    std::atomic<float>* puLAPtr = nullptr;
    std::atomic<float>* puPKFPtr = nullptr;
    std::atomic<float>* puPKQPtr = nullptr;
    std::atomic<float>* puPKBPtr = nullptr;
    std::atomic<float>* puHSFPtr = nullptr;
    std::atomic<float>* puHAPtr = nullptr;
    
    // Modern EQ
    std::atomic<float>* mqLFPtr = nullptr;
    std::atomic<float>* mqLGPtr = nullptr;
    std::atomic<float>* mqM1FPtr = nullptr;
    std::atomic<float>* mqM1GPtr = nullptr;
    std::atomic<float>* mqM1QPtr = nullptr;
    std::atomic<float>* mqM2FPtr = nullptr;
    std::atomic<float>* mqM2GPtr = nullptr;
    std::atomic<float>* mqM2QPtr = nullptr;
    std::atomic<float>* mqHFPtr = nullptr;
    std::atomic<float>* mqHGPtr = nullptr;
    
    std::atomic<float>* oversamplingPtr = nullptr;
    std::atomic<float>* outputTypePtr = nullptr;
    std::atomic<float>* outputDrivePtr = nullptr;
    std::atomic<float>* outputSafePtr = nullptr;

    std::atomic<float> preampLevel { 0.0f };
    std::atomic<float> compLevel   { 0.0f };
    std::atomic<float> eqLevel     { 0.0f };
    std::atomic<float> compGR      { 0.0f };

private:
    struct Preset {
        juce::String name;
        std::vector<std::pair<juce::String, float>> parameters;
    };
    std::vector<Preset> presets;

    void initializePresets();

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    PreampDSP preamp;
    
    NC76Compressor nc76;
    LA2ACompressor la2a;
    FairchildCompressor fairchild;
    ModernCompressorDSP modComp;
    
    DirtEQDSP dirtEq;
    NeveEQDSP neveEq;
    PultecEQDSP pultecEq;
    ModernEQDSP modEq;
    OutputDSP outputColor;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    bool lastOversamplingState = true;

    double inputRmsTracker = 0.001;
    double outputRmsTracker = 0.001;
    double autoMatchGain = 1.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogFxAudioProcessor)
};
