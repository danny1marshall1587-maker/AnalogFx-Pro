cmake_minimum_required(VERSION 3.20)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()

project(AnalogFx VERSION 2.0.0)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
)
FetchContent_MakeAvailable(juce)

# Create the plugin target
juce_add_plugin(AnalogFx
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    COMPANY_NAME "Danny Marshall"
    PLUGIN_MANUFACTURER_CODE DanM
    PLUGIN_CODE Anfx
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AnalogFx"
)

juce_generate_juce_header(AnalogFx)

# Binary Data (GUI Assets)
juce_add_binary_data(AnalogFx_Assets
    HEADER_NAME "BinaryData.h"
    NAMESPACE "BinaryData"
    SOURCES
        Assets/final/telefunken_bg.png
        Assets/final/neve_bg.png
        Assets/final/modern_pre_bg.png
        Assets/final/nc76_bg.png
        Assets/final/la2a_bg.png
        Assets/final/fairchild_bg.png
        Assets/final/modern_comp_bg.png
        Assets/final/dirt_eq_bg.png
        Assets/final/pultec_bg.png
        Assets/final/modern_eq_bg.png
        Assets/final/pmod_pre_knob.png
        Assets/final/neve_knob.png
        Assets/final/modern_pre_knob.png
        Assets/final/nc76_knob.png
        Assets/final/la2a_knob.png
        Assets/final/fairchild_knob.png
        Assets/final/modern_comp_knob.png
        Assets/final/dirt_eq_knob.png
        Assets/final/pultec_knob.png
        Assets/final/modern_eq_knob.png
        Assets/final/knob_highlight.png
        Assets/final/switch_on.png
        Assets/final/switch_off.png
        Assets/final/output_bg.png
        Assets/final/output_knob.png
)

set_target_properties(AnalogFx_Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_sources(AnalogFx
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DirtEQDSP.cpp
        Source/NC76Compressor.cpp
        Source/LA2ACompressor.cpp
        Source/FairchildCompressor.cpp
        Source/PreampDSP.cpp
        Source/NeveEQDSP.cpp
        Source/PultecEQDSP.cpp
)

target_compile_definitions(AnalogFx
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AnalogFx
    PRIVATE
        AnalogFx_Assets
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
