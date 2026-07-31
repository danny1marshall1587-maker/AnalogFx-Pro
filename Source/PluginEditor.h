#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ImageKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ImageKnobLookAndFeel(const char* baseData, int baseSize, const char* highlightData, int highlightSize)
    {
        knobImage = juce::ImageCache::getFromMemory(baseData, baseSize);
        if (highlightData != nullptr)
            highlightImage = juce::ImageCache::getFromMemory(highlightData, highlightSize);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        juce::ignoreUnused(slider);
        if (!knobImage.isValid()) return;

        const double rotation = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        const float radius = juce::jmin(width / 2.0f, height / 2.0f) * 0.94f;
        const float rx = x + width * 0.5f - radius;
        const float ry = y + height * 0.5f - radius;
        const float renderSize = radius * 2.0f;
        
        if (renderSize <= 0) return;

        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);

        // Draw Soft Shadow SMALLER than the knob to prevent darkening the face
        {
            const float shadowOffset = renderSize * 0.08f;
            const float shadowSize = renderSize * 0.85f;
            const float sx = rx + (renderSize - shadowSize) * 0.5f + shadowOffset * 0.3f;
            const float sy = ry + (renderSize - shadowSize) * 0.5f + shadowOffset * 0.6f;
            
            juce::ColourGradient grad(juce::Colours::black.withAlpha(0.6f), sx + shadowSize * 0.5f, sy + shadowSize * 0.5f,
                                      juce::Colours::transparentBlack, sx + shadowSize * 1.1f, sy + shadowSize * 1.1f, true);
            g.setGradientFill(grad);
            g.fillEllipse(sx - shadowSize * 0.1f, sy - shadowSize * 0.1f, shadowSize * 1.2f, shadowSize * 1.2f);
        }
        
        juce::AffineTransform transform;
        transform = transform.rotated((float)rotation, knobImage.getWidth() * 0.5f, knobImage.getHeight() * 0.5f);
        transform = transform.scaled(renderSize / knobImage.getWidth(), renderSize / knobImage.getHeight());
        transform = transform.translated(rx, ry);
        
        g.drawImageTransformed(knobImage, transform, false);

        if (highlightImage.isValid())
        {
            // Reduced opacity to 0.12f to keep the colors vibrant and avoid "graying out"
            g.setOpacity(0.12f);
            g.drawImage(highlightImage, rx, ry, renderSize, renderSize, 0, 0, highlightImage.getWidth(), highlightImage.getHeight(), false);
            g.setOpacity(1.0f);
        }
    }

private:
    juce::Image knobImage;
    juce::Image highlightImage;
};

class SwitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SwitchLookAndFeel()
    {
        onImage = juce::ImageCache::getFromMemory(BinaryData::switch_on_png, BinaryData::switch_on_pngSize);
        offImage = juce::ImageCache::getFromMemory(BinaryData::switch_off_png, BinaryData::switch_off_pngSize);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        if (onImage.isValid() && offImage.isValid())
        {
            auto bounds = button.getLocalBounds().toFloat();
            auto& img = button.getToggleState() ? onImage : offImage;
            g.drawImage(img, bounds, juce::RectanglePlacement::centred);
        }
    }

private:
    juce::Image onImage, offImage;
};

class ModernButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(2);
        auto baseColor = juce::Colour(0xff222222);
        auto accentColor = juce::Colours::cyan;
        
        if (button.getToggleState()) {
            g.setColour(accentColor.withAlpha(0.6f));
            g.fillRoundedRectangle(bounds, 4.0f);
        } else {
            g.setColour(baseColor);
            g.fillRoundedRectangle(bounds, 4.0f);
        }
        
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
        
        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colours::white.withAlpha(0.1f));
            g.fillRoundedRectangle(bounds, 4.0f);
        }
    }
};

class GlassmorphismComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GlassmorphismComboBoxLookAndFeel();
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, const bool isSeparator, const bool isActive, const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
};

class AnalogFxAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::ComboBox::Listener, private juce::Timer
{
public:
    AnalogFxAudioProcessorEditor(AnalogFxAudioProcessor&);
    ~AnalogFxAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void timerCallback() override;

private:
    AnalogFxAudioProcessor& audioProcessor;
    
    ImageKnobLookAndFeel telefunkenLaf {BinaryData::pmod_pre_knob_png, BinaryData::pmod_pre_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel neveLaf {BinaryData::neve_knob_png, BinaryData::neve_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel modPreLaf {BinaryData::modern_pre_knob_png, BinaryData::modern_pre_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel nc76Laf {BinaryData::nc76_knob_png, BinaryData::nc76_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel la2aLaf {BinaryData::la2a_knob_png, BinaryData::la2a_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel fairchildLaf {BinaryData::fairchild_knob_png, BinaryData::fairchild_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel modCompLaf {BinaryData::modern_comp_knob_png, BinaryData::modern_comp_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel dirtEqLaf {BinaryData::dirt_eq_knob_png, BinaryData::dirt_eq_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel pultecLaf {BinaryData::pultec_knob_png, BinaryData::pultec_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel modEqLaf {BinaryData::modern_eq_knob_png, BinaryData::modern_eq_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};
    ImageKnobLookAndFeel outputLaf {BinaryData::output_knob_png, BinaryData::output_knob_pngSize, BinaryData::knob_highlight_png, BinaryData::knob_highlight_pngSize};

    SwitchLookAndFeel switchLaf;
    ModernButtonLookAndFeel modernButtonLaf;
    
    float preMeterVal = 0.0f;
    float compMeterVal = 0.0f;
    float eqMeterVal = 0.0f;
    float compGRVal = 0.0f;

    // Background Images
    juce::Image bgTelefunken, bgNevePre, bgModernPre, bgPultec, bgModEq, bgModComp;
    juce::Image bgNC76, bgLA2A, bgFairchild, bgDirtEq, bgOutput;

    // Cached Scaled Backgrounds for performance
    juce::Image s_bgTelefunken, s_bgNevePre, s_bgModernPre;
    juce::Image s_bgPultec, s_bgModEq, s_bgModComp;
    juce::Image s_bgNC76, s_bgLA2A, s_bgFairchild, s_bgDirtEq, s_bgOutput;

    juce::ComboBox presetSelector;
    GlassmorphismComboBoxLookAndFeel presetLaf;
    juce::File presetDirectory;
    juce::StringArray userPresets;
    void loadUserPresets();
    void saveUserPreset();

    // Module Selectors
    juce::ComboBox preampSelector;
    juce::ComboBox compSelector;
    juce::ComboBox eqSelector;
    juce::ComboBox outputSelector;
    juce::ComboBox scaleSelector;
    
    juce::ToggleButton oversamplingButton;
    juce::Label oversamplingLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> preampSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> compSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> eqSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> outputSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oversamplingAtt;

    float currentScale = 1.0f;

    // Slider Arrays
    std::vector<std::unique_ptr<juce::Slider>> preSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> preAtts;
    std::vector<std::unique_ptr<juce::Label>> preLabels;

    std::vector<std::unique_ptr<juce::Slider>> compSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> compAtts;
    std::vector<std::unique_ptr<juce::Label>> compLabels;

    std::vector<std::unique_ptr<juce::Slider>> eqSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> eqAtts;
    std::vector<std::unique_ptr<juce::Label>> eqLabels;

    std::vector<std::unique_ptr<juce::Slider>> outputSliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> outputAtts;
    std::vector<std::unique_ptr<juce::Label>> outputLabels;

    std::vector<std::unique_ptr<juce::ToggleButton>> eqButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> eqButtonAtts;

    std::vector<std::unique_ptr<juce::ToggleButton>> outputButtons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> outputButtonAtts;

    void buildKnob(const juce::String& paramId, const juce::String& name, int section); // 0=Pre, 1=Comp, 2=EQ, 3=Output
    void buildButton(const juce::String& paramId, const juce::String& name, int section);
    void updateVisibility();

    juce::OpenGLContext openGLContext;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogFxAudioProcessorEditor)
};
