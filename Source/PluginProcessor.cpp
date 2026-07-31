#include "PluginProcessor.h"
#include "PluginEditor.h"

AnalogFxAudioProcessor::AnalogFxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
                     #if !JucePlugin_IsMidiEffect
                      #if !JucePlugin_IsSynth
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                     ),
#else
    :
#endif
    apvts(*this, nullptr, "Parameters", createParameters())
{
    initializePresets();
    
    preTypePtr = apvts.getRawParameterValue("preamp_type");
    preDrivePtr = apvts.getRawParameterValue("preamp_drive");
    preTrimPtr = apvts.getRawParameterValue("preamp_trim");
    preAutoPtr = apvts.getRawParameterValue("preamp_auto");
    
    compTypePtr = apvts.getRawParameterValue("comp_type");
    compTrimPtr = apvts.getRawParameterValue("comp_trim");
    nc76ThreshPtr = apvts.getRawParameterValue("nc76_thresh");
    nc76RatioPtr = apvts.getRawParameterValue("nc76_ratio");
    nc76AttackPtr = apvts.getRawParameterValue("nc76_attack");
    nc76ReleasePtr = apvts.getRawParameterValue("nc76_release");
    nc76InGainPtr = apvts.getRawParameterValue("nc76_in_gain");
    nc76OutGainPtr = apvts.getRawParameterValue("nc76_out_gain");
    nc76MixPtr = apvts.getRawParameterValue("nc76_mix");

    la2aPeakPtr = apvts.getRawParameterValue("la2a_peak");
    la2aGainPtr = apvts.getRawParameterValue("la2a_gain");
    la2aRatioPtr = apvts.getRawParameterValue("la2a_ratio");

    fcLTPtr = apvts.getRawParameterValue("fc_l_thresh");
    fcRTPtr = apvts.getRawParameterValue("fc_r_thresh");
    fcLBPtr = apvts.getRawParameterValue("fc_l_bias");
    fcRBPtr = apvts.getRawParameterValue("fc_r_bias");
    fcLMPtr = apvts.getRawParameterValue("fc_l_makeup");
    fcRMPtr = apvts.getRawParameterValue("fc_r_makeup");
    fcAGCPtr = apvts.getRawParameterValue("fc_agc");
    fcLTiPtr = apvts.getRawParameterValue("fc_l_time");
    fcRTiPtr = apvts.getRawParameterValue("fc_r_time");
    fcLRMSPtr = apvts.getRawParameterValue("fc_l_rms");
    fcRRMSPtr = apvts.getRawParameterValue("fc_r_rms");

    mcThreshPtr = apvts.getRawParameterValue("mc_thresh");
    mcRatioPtr = apvts.getRawParameterValue("mc_ratio");
    mcAttackPtr = apvts.getRawParameterValue("mc_attack");
    mcReleasePtr = apvts.getRawParameterValue("mc_release");
    mcMakeupPtr = apvts.getRawParameterValue("mc_makeup");

    eqTypePtr = apvts.getRawParameterValue("eq_type");
    eqTrimPtr = apvts.getRawParameterValue("eq_trim");

    deLPFPtr = apvts.getRawParameterValue("lowpass_freq");
    deLPQPtr = apvts.getRawParameterValue("lowpass_q");
    deB1GPtr = apvts.getRawParameterValue("band1_gain");
    deB1FPtr = apvts.getRawParameterValue("band1_freq");
    deB2GPtr = apvts.getRawParameterValue("band2_gain");
    deB2FPtr = apvts.getRawParameterValue("band2_freq");
    deB2QPtr = apvts.getRawParameterValue("band2_q");
    deB3GPtr = apvts.getRawParameterValue("band3_gain");
    deB3FPtr = apvts.getRawParameterValue("band3_freq");
    deB3QPtr = apvts.getRawParameterValue("band3_q");
    deB4GPtr = apvts.getRawParameterValue("band4_gain");
    deB4FPtr = apvts.getRawParameterValue("band4_freq");
    deHPFPtr = apvts.getRawParameterValue("highpass_freq");
    deHPQPtr = apvts.getRawParameterValue("highpass_q");
    deIC1Ptr = apvts.getRawParameterValue("ic1_drive");
    deIC2Ptr = apvts.getRawParameterValue("ic2_drive");
    deDrivePtr = apvts.getRawParameterValue("drive_db");
    deGainPtr = apvts.getRawParameterValue("gain_db");
    deOnPtr = apvts.getRawParameterValue("dirt_eq_on");

    nvGainPtr = apvts.getRawParameterValue("nv_gain");
    nvHFGPtr = apvts.getRawParameterValue("nv_hf_gain");
    nvMFGPtr = apvts.getRawParameterValue("nv_mf_gain");
    nvMFFPtr = apvts.getRawParameterValue("nv_mf_freq");
    nvLFGPtr = apvts.getRawParameterValue("nv_lf_gain");
    nvLFFPtr = apvts.getRawParameterValue("nv_lf_freq");
    nvHPFPtr = apvts.getRawParameterValue("nv_hpf_freq");
    nvDrivePtr = apvts.getRawParameterValue("nv_drive");
    nvPhasePtr = apvts.getRawParameterValue("nv_phase");
    nvHFOnPtr = apvts.getRawParameterValue("nv_hf_on");
    nvMFOnPtr = apvts.getRawParameterValue("nv_mf_on");
    nvLFOnPtr = apvts.getRawParameterValue("nv_lf_on");
    nvHPFOnPtr = apvts.getRawParameterValue("nv_hpf_on");
    nvEqOnPtr = apvts.getRawParameterValue("nv_eq_on");

    puLSFPtr = apvts.getRawParameterValue("pu_lsf");
    puLBPtr = apvts.getRawParameterValue("pu_l_boost");
    puLAPtr = apvts.getRawParameterValue("pu_l_atten");
    puPKFPtr = apvts.getRawParameterValue("pu_peak");
    puPKQPtr = apvts.getRawParameterValue("pu_peak_q");
    puPKBPtr = apvts.getRawParameterValue("pu_p_boost");
    puHSFPtr = apvts.getRawParameterValue("pu_hsf");
    puHAPtr = apvts.getRawParameterValue("pu_h_atten");

    mqLFPtr = apvts.getRawParameterValue("mq_l_freq");
    mqLGPtr = apvts.getRawParameterValue("mq_l_gain");
    mqM1FPtr = apvts.getRawParameterValue("mq_m1_freq");
    mqM1GPtr = apvts.getRawParameterValue("mq_m1_gain");
    mqM1QPtr = apvts.getRawParameterValue("mq_m1_q");
    mqM2FPtr = apvts.getRawParameterValue("mq_m2_freq");
    mqM2GPtr = apvts.getRawParameterValue("mq_m2_gain");
    mqM2QPtr = apvts.getRawParameterValue("mq_m2_q");
    mqHFPtr = apvts.getRawParameterValue("mq_h_freq");
    mqHGPtr = apvts.getRawParameterValue("mq_h_gain");

    oversamplingPtr = apvts.getRawParameterValue("oversampling");
    outputTypePtr = apvts.getRawParameterValue("output_type");
    outputDrivePtr = apvts.getRawParameterValue("output_drive");
    outputSafePtr = apvts.getRawParameterValue("output_safe");
}

AnalogFxAudioProcessor::~AnalogFxAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout AnalogFxAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // PREAMP
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("preamp_type", 1), "Preamp Type", juce::StringArray{"Bypass", "Telefunken", "Neve", "Modern"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("preamp_drive", 1), "Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("preamp_trim", 1), "Trim", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("preamp_auto", 1), "Auto Gain", true));
    
    // COMPRESSOR SELECTION
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("comp_type", 1), "Compressor", juce::StringArray{"Bypass", "NC76", "LA-2A", "Fairchild", "Modern VCA"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("comp_trim", 1), "Comp Trim", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    // NC76 Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_thresh", 1), "NC76 Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("nc76_ratio", 1), "NC76 Ratio", juce::StringArray{"4:1", "8:1", "12:1", "20:1", "All In"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_attack", 1), "NC76 Attack", juce::NormalisableRange<float>(20.0f, 800.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_release", 1), "NC76 Release", juce::NormalisableRange<float>(10.0f, 1100.0f, 1.0f), 250.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_in_gain", 1), "NC76 In", juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f), 0.77f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_out_gain", 1), "NC76 Out", juce::NormalisableRange<float>(-60.0f, 17.0f, 0.1f), -10.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nc76_mix", 1), "NC76 Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    // LA2A Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("la2a_peak", 1), "LA2A Peak Red.", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 33.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("la2a_gain", 1), "LA2A Gain", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 33.3f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("la2a_ratio", 1), "LA2A Mode", juce::StringArray{"Compress", "Limit"}, 0));

    // Fairchild Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_l_thresh", 1), "L/Lat Thresh", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_r_thresh", 1), "R/Vert Thresh", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_l_bias", 1), "L/Lat Bias", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 70.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_r_bias", 1), "R/Vert Bias", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 70.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_l_makeup", 1), "L/Lat Gain", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_r_makeup", 1), "R/Vert Gain", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("fc_agc", 1), "Mode", juce::StringArray{"Left/Right", "Lat/Vert"}, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("fc_l_time", 1), "L/Lat Time", juce::StringArray{"1", "2", "3", "4", "5", "6"}, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("fc_r_time", 1), "R/Vert Time", juce::StringArray{"1", "2", "3", "4", "5", "6"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_l_rms", 1), "L/Lat RMS", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fc_r_rms", 1), "R/Vert RMS", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 100.0f));

    // Modern Compressor
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mc_thresh", 1), "Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mc_ratio", 1), "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mc_attack", 1), "Attack", juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.3f), 10.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mc_release", 1), "Release", juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.3f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mc_makeup", 1), "Makeup", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    // EQ SELECTION
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("eq_type", 1), "EQ", juce::StringArray{"Bypass", "Dirt EQ", "Neve 1073", "Pultec EQP-1A", "Modern Surgical"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("eq_trim", 1), "EQ Trim", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    // Dirt EQ Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lowpass_freq", 1), "LP Freq", juce::NormalisableRange<float>(4000.0f, 24000.0f, 1.0f, 0.3f), 24000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lowpass_q", 1), "LP Q", juce::NormalisableRange<float>(0.1f, 1.7f, 0.01f), 0.9f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band1_gain", 1), "High Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band1_freq", 1), "High Freq", juce::NormalisableRange<float>(4000.0f, 16000.0f, 1.0f, 0.3f), 10000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band2_gain", 1), "H-Mid Gain", juce::NormalisableRange<float>(-15.0f, 15.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band2_freq", 1), "H-Mid Freq", juce::NormalisableRange<float>(500.0f, 5500.0f, 1.0f, 0.3f), 3000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band2_q", 1), "H-Mid Q", juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band3_gain", 1), "L-Mid Gain", juce::NormalisableRange<float>(-15.0f, 15.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band3_freq", 1), "L-Mid Freq", juce::NormalisableRange<float>(100.0f, 900.0f, 1.0f, 0.5f), 500.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band3_q", 1), "L-Mid Q", juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band4_gain", 1), "Low Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("band4_freq", 1), "Low Freq", juce::NormalisableRange<float>(10.0f, 150.0f, 1.0f, 0.5f), 80.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("highpass_freq", 1), "HP Freq", juce::NormalisableRange<float>(5.0f, 175.0f, 1.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("highpass_q", 1), "HP Q", juce::NormalisableRange<float>(0.1f, 1.7f, 0.01f), 0.9f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ic1_drive", 1), "IC1 Drive", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ic2_drive", 1), "IC2 Drive", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("drive_db", 1), "Drive Gain", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gain_db", 1), "Out Gain", juce::NormalisableRange<float>(-10.0f, 10.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("dirt_eq_on", 1), "Dirt EQ On", true));

    // Neve 1073
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nv_gain", 1), "Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nv_hf_gain", 1), "HF Gain", juce::NormalisableRange<float>(-16.0f, 16.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nv_mf_gain", 1), "MF Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("nv_mf_freq", 1), "MF Freq", juce::StringArray{"Off", "0.36k", "0.7k", "1.6k", "3.2k", "4.8k", "7.2k"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nv_lf_gain", 1), "LF Gain", juce::NormalisableRange<float>(-16.0f, 16.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("nv_lf_freq", 1), "LF Freq", juce::StringArray{"Off", "35Hz", "60Hz", "110Hz", "220Hz"}, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("nv_hpf_freq", 1), "HPF Freq", juce::StringArray{"Off", "50Hz", "80Hz", "160Hz", "300Hz"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("nv_drive", 1), "Saturation", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_phase", 1), "Phase", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_hf_on", 1), "HF On", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_mf_on", 1), "MF On", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_lf_on", 1), "LF On", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_hpf_on", 1), "HPF On", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("nv_eq_on", 1), "EQ On", true));

    // Pultec EQP-1A
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("pu_lsf", 1), "LF Freq", juce::StringArray{"20Hz", "30Hz", "60Hz", "100Hz"}, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pu_l_boost", 1), "LF Boost", juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pu_l_atten", 1), "LF Atten", juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("pu_peak", 1), "HF Freq", juce::StringArray{"3k", "4k", "5k", "8k", "10k", "12k", "16k"}, 4));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pu_peak_q", 1), "HF Q", juce::NormalisableRange<float>(0.0f, 10.0f, 0.05f), 2.77f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pu_p_boost", 1), "HF Boost", juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("pu_hsf", 1), "HF Atten Freq", juce::StringArray{"5k", "10k", "20k"}, 2));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pu_h_atten", 1), "HF Atten", juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f));

    // Modern EQ
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_l_freq", 1), "LF", juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.3f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_l_gain", 1), "LF Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m1_freq", 1), "LMF", juce::NormalisableRange<float>(200.0f, 2500.0f, 1.0f, 0.3f), 800.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m1_gain", 1), "LMF Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m1_q", 1), "LMF Q", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m2_freq", 1), "HMF", juce::NormalisableRange<float>(1000.0f, 8000.0f, 1.0f, 0.3f), 3000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m2_gain", 1), "HMF Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_m2_q", 1), "HMF Q", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_h_freq", 1), "HF", juce::NormalisableRange<float>(4000.0f, 20000.0f, 1.0f, 0.3f), 10000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("mq_h_gain", 1), "HF Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("output_type", 1), "Output Color", juce::StringArray{"Bypass", "Vintage Tape", "British Iron", "Valve Summing", "Modern Polish"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("output_drive", 1), "Output Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("output_safe", 1), "Safe Level", false));

    // GLOBAL
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("oversampling", 1), "Oversampling", true));

    return layout;
}

const juce::String AnalogFxAudioProcessor::getName() const { return JucePlugin_Name; }
bool AnalogFxAudioProcessor::acceptsMidi() const { return false; }
bool AnalogFxAudioProcessor::producesMidi() const { return false; }
bool AnalogFxAudioProcessor::isMidiEffect() const { return false; }
double AnalogFxAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AnalogFxAudioProcessor::getNumPrograms() { 
    return presets.empty() ? 1 : (int)presets.size(); 
}
int AnalogFxAudioProcessor::getCurrentProgram() { return 0; }
void AnalogFxAudioProcessor::setCurrentProgram(int index) {
    if (index >= 0 && index < (int)presets.size()) {
        for (const auto& param : presets[index].parameters) {
            if (auto* p = apvts.getParameter(param.first)) {
                // p->setValueNotifyingHost takes a normalized 0.0 to 1.0 value
                // Since our maps use the actual float values (e.g. frequency 1000.0),
                // we should convert using the parameter's range
                if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(p)) {
                    floatParam->setValueNotifyingHost(floatParam->convertTo0to1(param.second));
                }
                else if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(p)) {
                    choiceParam->setValueNotifyingHost(choiceParam->convertTo0to1(param.second));
                }
                else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(p)) {
                    boolParam->setValueNotifyingHost(boolParam->convertTo0to1(param.second));
                }
            }
        }
    }
}
const juce::String AnalogFxAudioProcessor::getProgramName(int index) {
    if (index >= 0 && index < (int)presets.size()) {
        return presets[index].name;
    }
    return "Default";
}
void AnalogFxAudioProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

void AnalogFxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // 4x Oversampling (2 stages of 2x)
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true);
    oversampler->initProcessing(samplesPerBlock);
    
    double oversampledRate = sampleRate * 4.0;

    preamp.prepare(oversampledRate);
    nc76.prepare(oversampledRate);
    la2a.prepare(oversampledRate);
    fairchild.prepare(oversampledRate);
    modComp.prepare(oversampledRate);
    dirtEq.prepare(oversampledRate);
    neveEq.prepare(oversampledRate);
    pultecEq.prepare(oversampledRate);
    modEq.prepare(oversampledRate);
    outputColor.prepare(oversampledRate);

    lastOversamplingState = apvts.getRawParameterValue("oversampling")->load();
}

void AnalogFxAudioProcessor::releaseResources()
{
}

void AnalogFxAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // --- 0. OVERSAMPLING SWITCH ---
    bool oversamplingEnabled = oversamplingPtr->load();
    if (oversamplingEnabled != lastOversamplingState)
    {
        lastOversamplingState = oversamplingEnabled;
        double currentRate = getSampleRate() * (oversamplingEnabled ? 4.0 : 1.0);
        preamp.prepare(currentRate);
        nc76.prepare(currentRate);
        la2a.prepare(currentRate);
        fairchild.prepare(currentRate);
        modComp.prepare(currentRate);
        dirtEq.prepare(currentRate);
        neveEq.prepare(currentRate);
        pultecEq.prepare(currentRate);
        modEq.prepare(currentRate);
        outputColor.prepare(currentRate);
        
        setLatencySamples(oversamplingEnabled ? (int)oversampler->getLatencyInSamples() : 0);
    }

    float* leftChannel = nullptr;
    float* rightChannel = nullptr;
    int numSamples = 0;

    juce::dsp::AudioBlock<const float> inputBlock(buffer);
    juce::dsp::AudioBlock<float> oversampledBlock;

    if (oversamplingEnabled)
    {
        oversampledBlock = oversampler->processSamplesUp(inputBlock);
        leftChannel = oversampledBlock.getChannelPointer(0);
        rightChannel = (oversampledBlock.getNumChannels() > 1) ? oversampledBlock.getChannelPointer(1) : leftChannel;
        numSamples = (int)oversampledBlock.getNumSamples();
    }
    else
    {
        leftChannel = buffer.getWritePointer(0);
        rightChannel = (buffer.getNumChannels() > 1) ? buffer.getWritePointer(1) : leftChannel;
        numSamples = buffer.getNumSamples();
    }

    // --- MEASURE INPUT RMS FOR GLOBAL AUTO VOLUME MATCHING ---
    double inSum = 0.0;
    for (int i = 0; i < numSamples; ++i)
    {
        double s0 = leftChannel[i];
        double s1 = rightChannel[i];
        inSum += s0 * s0 + s1 * s1;
    }
    double currentInRms = std::sqrt(inSum / std::max(1, numSamples * 2));
    inputRmsTracker = inputRmsTracker * 0.95 + currentInRms * 0.05;

    // --- 1. PREAMP STAGE ---
    PreampDSP::Parameters preParams;
    preParams.type = preTypePtr->load();
    preParams.drive = preDrivePtr->load();
    preParams.trim = preTrimPtr->load();
    preParams.auto_level = preAutoPtr->load();
    
    preamp.process(leftChannel, rightChannel, numSamples, preParams);
    preampLevel.store(preamp.getCurrentRMS());

    // --- 2. COMPRESSOR STAGE ---
    int comp_type = compTypePtr->load();
    if (comp_type == 1) // NC76
    {
        NC76Compressor::Parameters compParams;
        compParams.threshold = nc76ThreshPtr->load();
        int ratioIdx = nc76RatioPtr->load();
        if (ratioIdx == 0) compParams.ratio_idx = 4;
        else if (ratioIdx == 1) compParams.ratio_idx = 8;
        else if (ratioIdx == 2) compParams.ratio_idx = 12;
        else if (ratioIdx == 3) compParams.ratio_idx = 20;
        else compParams.ratio_idx = 99; // All in
        compParams.attack_us = nc76AttackPtr->load();
        compParams.release_ms = nc76ReleasePtr->load();
        compParams.in_gain = nc76InGainPtr->load();
        compParams.out_gain_db = nc76OutGainPtr->load();
        compParams.mix = nc76MixPtr->load();
        
        nc76.process(leftChannel, rightChannel, numSamples, compParams);
        compLevel.store(nc76.getCurrentRMS());
        compGR.store(nc76.getGainReduction());
    }
    else if (comp_type == 2) // LA-2A
    {
        LA2ACompressor::Parameters compParams;
        compParams.peak_reduction = la2aPeakPtr->load();
        compParams.gain = la2aGainPtr->load();
        compParams.ratio_limit = la2aRatioPtr->load();
        
        la2a.process(leftChannel, rightChannel, numSamples, compParams);
        compLevel.store(la2a.getCurrentRMS());
        compGR.store(la2a.getGainReduction());
    }
    else if (comp_type == 3) // Fairchild
    {
        FairchildCompressor::Parameters compParams;
        compParams.l_thresh = fcLTPtr->load();
        compParams.r_thresh = fcRTPtr->load();
        compParams.l_bias = fcLBPtr->load();
        compParams.r_bias = fcRBPtr->load();
        compParams.l_makeup = fcLMPtr->load();
        compParams.r_makeup = fcRMPtr->load();
        compParams.agc = fcAGCPtr->load();
        compParams.l_time = fcLTiPtr->load() + 1;
        compParams.r_time = fcRTiPtr->load() + 1;
        compParams.l_rms = fcLRMSPtr->load();
        compParams.r_rms = fcRRMSPtr->load();
        
        fairchild.process(leftChannel, rightChannel, numSamples, compParams);
        compLevel.store(fairchild.getCurrentRMS());
        compGR.store(fairchild.getGainReduction());
    }
    else if (comp_type == 4) // Modern
    {
        ModernCompressorDSP::Parameters compParams;
        compParams.threshold = mcThreshPtr->load();
        compParams.ratio = mcRatioPtr->load();
        compParams.attack = mcAttackPtr->load();
        compParams.release = mcReleasePtr->load();
        compParams.makeup = mcMakeupPtr->load();
        compParams.trim = compTrimPtr->load();
        
        modComp.process(leftChannel, rightChannel, numSamples, compParams);
        compLevel.store(modComp.getCurrentRMS());
        compGR.store(modComp.getGainReduction());
    }
    else {
        compLevel.store(0.0f);
        compGR.store(0.0f);
    }

    // --- 3. EQ STAGE ---
    int eq_type = eqTypePtr->load();
    if (eq_type == 1) // Dirt EQ
    {
        DirtEQDSP::Parameters params;
        params.lowpass_freq = deLPFPtr->load();
        params.lowpass_q = deLPQPtr->load();
        params.band1_gain = deB1GPtr->load();
        params.band1_freq = deB1FPtr->load();
        params.band2_gain = deB2GPtr->load();
        params.band2_freq = deB2FPtr->load();
        params.band2_q = deB2QPtr->load();
        params.band3_gain = deB3GPtr->load();
        params.band3_freq = deB3FPtr->load();
        params.band3_q = deB3QPtr->load();
        params.band4_gain = deB4GPtr->load();
        params.band4_freq = deB4FPtr->load();
        params.highpass_freq = deHPFPtr->load();
        params.highpass_q = deHPQPtr->load();
        params.ic1_drive = deIC1Ptr->load();
        params.ic2_drive = deIC2Ptr->load();
        params.drive_db = deDrivePtr->load();
        params.gain_db = deGainPtr->load();
        params.eq_on = deOnPtr->load();
        
        dirtEq.process(leftChannel, rightChannel, numSamples, params);
        eqLevel.store(dirtEq.getCurrentRMS());
    }
    else if (eq_type == 2) // Neve 1073
    {
        NeveEQDSP::Parameters params;
        params.gain = nvGainPtr->load();
        params.hf_gain = nvHFGPtr->load();
        params.mf_gain = nvMFGPtr->load();
        params.mf_freq = nvMFFPtr->load();
        params.lf_gain = nvLFGPtr->load();
        params.lf_freq = nvLFFPtr->load();
        params.hpf_freq = nvHPFPtr->load();
        params.drive = nvDrivePtr->load();
        params.phase = nvPhasePtr->load() > 0.5f;
        params.hf_on = nvHFOnPtr->load() > 0.5f;
        params.mf_on = nvMFOnPtr->load() > 0.5f;
        params.lf_on = nvLFOnPtr->load() > 0.5f;
        params.hpf_on = nvHPFOnPtr->load() > 0.5f;
        params.eq_on = nvEqOnPtr->load() > 0.5f;
        params.trim = eqTrimPtr->load();
        
        neveEq.process(leftChannel, rightChannel, numSamples, params);
        eqLevel.store(neveEq.getCurrentRMS());
    }
    else if (eq_type == 3) // Pultec
    {
        PultecEQDSP::Parameters params;
        params.lsf_freq = puLSFPtr->load();
        params.low_boost = puLBPtr->load();
        params.low_atten = puLAPtr->load();
        params.peak_freq = puPKFPtr->load();
        params.peak_q = puPKQPtr->load();
        params.peak_boost = puPKBPtr->load();
        params.hsf_freq = puHSFPtr->load();
        params.high_atten = puHAPtr->load();
        params.trim = eqTrimPtr->load();
        
        pultecEq.process(leftChannel, rightChannel, numSamples, params);
        eqLevel.store(pultecEq.getCurrentRMS());
    }
    else if (eq_type == 4) // Modern EQ
    {
        ModernEQDSP::Parameters params;
        params.lowFreq = mqLFPtr->load();
        params.lowGain = mqLGPtr->load();
        params.mid1Freq = mqM1FPtr->load();
        params.mid1Gain = mqM1GPtr->load();
        params.mid1Q = mqM1QPtr->load();
        params.mid2Freq = mqM2FPtr->load();
        params.mid2Gain = mqM2GPtr->load();
        params.mid2Q = mqM2QPtr->load();
        params.highFreq = mqHFPtr->load();
        params.highGain = mqHGPtr->load();
        params.trim = eqTrimPtr->load();
        
        modEq.process(leftChannel, rightChannel, numSamples, params);
        eqLevel.store(modEq.getCurrentRMS());
    }
    else {
        eqLevel.store(0.0f); // Bypass
    }

    // --- 4. OUTPUT COLOR STAGE ---
    OutputDSP::Parameters outParams;
    outParams.type = outputTypePtr->load();
    outParams.drive = outputDrivePtr->load();
    outParams.safe_level = outputSafePtr->load();
    outputColor.process(leftChannel, rightChannel, numSamples, outParams);

    // --- 4b. GLOBAL AUTOMATIC VOLUME MATCHING ---
    double outSum = 0.0;
    for (int i = 0; i < numSamples; ++i)
    {
        double s0 = leftChannel[i];
        double s1 = rightChannel[i];
        outSum += s0 * s0 + s1 * s1;
    }
    double currentOutRms = std::sqrt(outSum / std::max(1, numSamples * 2));
    outputRmsTracker = outputRmsTracker * 0.95 + currentOutRms * 0.05;

    if (inputRmsTracker > 0.0001 && outputRmsTracker > 0.0001)
    {
        double targetGain = inputRmsTracker / outputRmsTracker;
        targetGain = std::clamp(targetGain, 0.1, 8.0);
        
        for (int i = 0; i < numSamples; ++i)
        {
            autoMatchGain += (targetGain - autoMatchGain) * 0.001;
            leftChannel[i] = static_cast<float>(leftChannel[i] * autoMatchGain);
            rightChannel[i] = static_cast<float>(rightChannel[i] * autoMatchGain);
        }
    }

    // --- 5. DOWNSAMPLING ---
    if (oversamplingEnabled)
    {
        juce::dsp::AudioBlock<float> outputBlock(buffer);
        oversampler->processSamplesDown(outputBlock);
    }
}

bool AnalogFxAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* AnalogFxAudioProcessor::createEditor()
{
    return new AnalogFxAudioProcessorEditor(*this);
}

void AnalogFxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AnalogFxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnalogFxAudioProcessor();
}

void AnalogFxAudioProcessor::initializePresets()
{
    // 1. The Dr. Pepper 1176 (Vocals)
    presets.push_back({"Vocals: Dr. Pepper 1176", {
        {"preamp_type", 3.0f}, {"preamp_drive", 5.0f},
        {"comp_type", 1.0f}, {"nc76_attack", 100.0f}, {"nc76_release", 200.0f}, {"nc76_ratio", 0.0f}, {"nc76_in_gain", 1.5f},
        {"eq_type", 2.0f}, {"nv_hf_gain", 2.0f}, {"nv_hpf_freq", 80.0f}, {"nv_hpf_on", 1.0f}, {"nv_hf_on", 1.0f}
    }});

    // 2. Silky LA-2A Ballad (Vocals)
    presets.push_back({"Vocals: Silky LA-2A", {
        {"preamp_type", 1.0f}, {"preamp_drive", 3.0f},
        {"comp_type", 2.0f}, {"la2a_peak", 60.0f}, {"la2a_gain", 40.0f}, {"la2a_ratio", 0.0f},
        {"eq_type", 3.0f}, {"pu_peak", 4.0f}, {"pu_p_boost", 4.0f}, {"pu_lsf", 3.0f}, {"pu_l_atten", 2.0f}
    }});

    // 3. Aggressive Rock Vocal
    presets.push_back({"Vocals: Aggressive Rock", {
        {"preamp_type", 2.0f}, {"preamp_drive", 15.0f},
        {"comp_type", 1.0f}, {"nc76_ratio", 4.0f}, {"nc76_attack", 20.0f}, {"nc76_release", 100.0f}, {"nc76_in_gain", 2.0f},
        {"eq_type", 4.0f}, {"mq_m1_freq", 300.0f}, {"mq_m1_gain", -3.0f}, {"mq_m2_freq", 3000.0f}, {"mq_m2_gain", -2.0f}
    }});

    // 4. Pultec Bass Trick
    presets.push_back({"Bass: Pultec Trick", {
        {"preamp_type", 2.0f}, {"preamp_drive", 8.0f},
        {"comp_type", 2.0f}, {"la2a_peak", 40.0f}, {"la2a_gain", 45.0f}, {"la2a_ratio", 0.0f},
        {"eq_type", 3.0f}, {"pu_lsf", 2.0f}, {"pu_l_boost", 5.0f}, {"pu_l_atten", 4.0f}
    }});

    // 5. Punchy Picked Bass
    presets.push_back({"Bass: Punchy Picked", {
        {"preamp_type", 3.0f}, {"preamp_drive", 10.0f},
        {"comp_type", 1.0f}, {"nc76_ratio", 0.0f}, {"nc76_attack", 400.0f}, {"nc76_release", 50.0f},
        {"eq_type", 1.0f}, {"band3_freq", 1200.0f}, {"band3_gain", 3.0f}, {"dirt_eq_on", 1.0f}, {"drive_db", 6.0f}
    }});

    // 6. Acoustic Shimmer
    presets.push_back({"Guitar: Acoustic Shimmer", {
        {"preamp_type", 1.0f}, {"preamp_drive", 2.0f},
        {"comp_type", 3.0f}, {"fc_l_thresh", -2.0f}, {"fc_r_thresh", -2.0f}, {"fc_l_time", 0.0f}, {"fc_r_time", 0.0f},
        {"eq_type", 3.0f}, {"pu_peak", 5.0f}, {"pu_p_boost", 5.0f}, {"pu_lsf", 3.0f}, {"pu_l_atten", 3.0f}
    }});

    // 7. Electric Lead Push
    presets.push_back({"Guitar: Electric Lead", {
        {"preamp_type", 2.0f}, {"preamp_drive", 18.0f},
        {"comp_type", 1.0f}, {"nc76_ratio", 1.0f}, {"nc76_attack", 200.0f}, {"nc76_release", 150.0f},
        {"eq_type", 2.0f}, {"nv_mf_freq", 3200.0f}, {"nv_mf_gain", 4.0f}, {"nv_mf_on", 1.0f}, {"nv_hpf_freq", 160.0f}, {"nv_hpf_on", 1.0f}
    }});

    // 8. Fairchild Drum Bus
    presets.push_back({"Drums: Fairchild Bus", {
        {"preamp_type", 3.0f}, {"preamp_drive", 6.0f},
        {"comp_type", 3.0f}, {"fc_l_thresh", -5.0f}, {"fc_r_thresh", -5.0f}, {"fc_l_time", 1.0f}, {"fc_r_time", 1.0f}, {"fc_agc", 1.0f},
        {"eq_type", 4.0f}, {"mq_l_gain", 2.0f}, {"mq_h_gain", 2.0f}
    }});

    // 9. Smash Room Mic
    presets.push_back({"Drums: Smash Room", {
        {"preamp_type", 2.0f}, {"preamp_drive", 24.0f},
        {"comp_type", 1.0f}, {"nc76_ratio", 4.0f}, {"nc76_attack", 20.0f}, {"nc76_release", 50.0f}, {"nc76_in_gain", 3.6f},
        {"eq_type", 1.0f}, {"dirt_eq_on", 1.0f}, {"drive_db", 12.0f}
    }});

    // 10. Fat Kick Drum
    presets.push_back({"Drums: Fat Kick", {
        {"preamp_type", 1.0f}, {"preamp_drive", 12.0f},
        {"comp_type", 4.0f}, {"mc_thresh", -12.0f}, {"mc_ratio", 4.0f}, {"mc_attack", 15.0f},
        {"eq_type", 3.0f}, {"pu_lsf", 2.0f}, {"pu_l_boost", 6.0f}, {"pu_l_atten", 3.0f}, {"pu_peak", 0.0f}, {"pu_p_boost", 4.0f}
    }});
}

