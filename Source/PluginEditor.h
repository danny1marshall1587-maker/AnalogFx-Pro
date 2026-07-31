#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LogicProKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    LogicProKnobLookAndFeel(juce::Colour accent = juce::Colour(0xff00e5ff))
        : accentColor(accent)
    {
    }

    void setAccentColour(juce::Colour newColour) { accentColor = newColour; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(3.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Soft Drop Shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillEllipse(centreX - radius + 2.0f, centreY - radius + 3.0f, radius * 2.0f, radius * 2.0f);

        // 2. Track Background Arc (Dark Metallic Ring)
        float arcRadius = radius - 5.0f;
        juce::Path trackPath;
        trackPath.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff1f242d));
        g.strokePath(trackPath, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 3. Animated Active Parameter Arc (Neon Glow Accent)
        if (sliderPos > 0.001f)
        {
            juce::Path valuePath;
            valuePath.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
            
            // Outer Glow
            g.setColour(accentColor.withAlpha(0.3f));
            g.strokePath(valuePath, juce::PathStrokeType(7.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            // Core Neon Arc
            g.setColour(accentColor);
            g.strokePath(valuePath, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // 4. Center Knob Cap (Dark Anodized Aluminum)
        float capRadius = radius - 11.0f;
        if (capRadius > 4.0f)
        {
            juce::ColourGradient capGrad(juce::Colour(0xff2c323d), centreX, centreY - capRadius,
                                       juce::Colour(0xff14181f), centreX, centreY + capRadius, false);
            g.setGradientFill(capGrad);
            g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

            // Bevel Ring
            g.setColour(juce::Colours::white.withAlpha(0.15f));
            g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);

            // Inner Cap Indent
            float innerCap = capRadius * 0.75f;
            juce::ColourGradient innerGrad(juce::Colour(0xff12151b), centreX, centreY - innerCap,
                                         juce::Colour(0xff20252f), centreX, centreY + innerCap, false);
            g.setGradientFill(innerGrad);
            g.fillEllipse(centreX - innerCap, centreY - innerCap, innerCap * 2.0f, innerCap * 2.0f);

            // 5. White/Neon Pointer Line
            juce::Path pointer;
            float pointerLength = capRadius * 0.85f;
            pointer.addRectangle(-1.5f, -pointerLength, 3.0f, pointerLength * 0.5f);
            pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
            
            g.setColour(slider.isMouseOverOrDragging() ? accentColor : juce::Colours::white);
            g.fillPath(pointer);
        }
    }

private:
    juce::Colour accentColor;
};

class LogicProSwitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
        bool isOn = button.getToggleState();

        // Background Track (Pill Shape)
        auto trackColor = isOn ? juce::Colour(0xff00d2ff).withAlpha(0.25f) : juce::Colour(0xff1a1e26);
        g.setColour(trackColor);
        g.fillRoundedRectangle(bounds, bounds.getHeight() * 0.5f);

        g.setColour(isOn ? juce::Colour(0xff00d2ff).withAlpha(0.6f) : juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, bounds.getHeight() * 0.5f, 1.0f);

        // Sliding Metallic Thumb Knob
        float thumbW = bounds.getHeight() - 4.0f;
        float thumbX = isOn ? (bounds.getRight() - thumbW - 2.0f) : (bounds.getX() + 2.0f);
        auto thumbRect = juce::Rectangle<float>(thumbX, bounds.getY() + 2.0f, thumbW, thumbW);

        juce::ColourGradient thumbGrad(juce::Colour(0xff454e5b), thumbRect.getCentreX(), thumbRect.getY(),
                                      juce::Colour(0xff222730), thumbRect.getCentreX(), thumbRect.getBottom(), false);
        g.setGradientFill(thumbGrad);
        g.fillEllipse(thumbRect);

        // Center Status Dot
        g.setColour(isOn ? juce::Colour(0xff00e5ff) : juce::Colour(0xff666666));
        g.fillEllipse(thumbRect.reduced(thumbW * 0.3f));
    }
};

class ModernButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(2);
        auto baseColor = juce::Colour(0xff1a1f29);
        auto accentColor = juce::Colour(0xff00e5ff);
        
        if (button.getToggleState()) {
            g.setColour(accentColor.withAlpha(0.5f));
            g.fillRoundedRectangle(bounds, 6.0f);
        } else {
            g.setColour(baseColor);
            g.fillRoundedRectangle(bounds, 6.0f);
        }
        
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
        
        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colours::white.withAlpha(0.1f));
            g.fillRoundedRectangle(bounds, 6.0f);
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

class ModelStepperComponent : public juce::Component
{
public:
    ModelStepperComponent()
    {
        leftBtn.setButtonText("<");
        rightBtn.setButtonText(">");
        
        leftBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f242d));
        leftBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        rightBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f242d));
        rightBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        nameBox.setJustificationType(juce::Justification::centred);
        nameBox.setColour(juce::Label::textColourId, juce::Colours::white);
        nameBox.setFont(juce::Font(11.0f, juce::Font::bold));

        addAndMakeVisible(leftBtn);
        addAndMakeVisible(nameBox);
        addAndMakeVisible(rightBtn);

        leftBtn.onClick = [this] {
            if (options.isEmpty()) return;
            currentIndex = (currentIndex - 1 + options.size()) % options.size();
            updateLabel();
            if (onSelectionChanged) onSelectionChanged(currentIndex + 1);
        };

        rightBtn.onClick = [this] {
            if (options.isEmpty()) return;
            currentIndex = (currentIndex + 1) % options.size();
            updateLabel();
            if (onSelectionChanged) onSelectionChanged(currentIndex + 1);
        };
    }

    void setOptions(const juce::StringArray& newOptions)
    {
        options = newOptions;
        updateLabel();
    }

    void setSelectedId(int id, juce::NotificationType notification = juce::sendNotification)
    {
        int index = id - 1;
        if (index >= 0 && index < options.size()) {
            currentIndex = index;
            updateLabel();
            if (notification == juce::sendNotification && onSelectionChanged)
                onSelectionChanged(id);
        }
    }

    int getSelectedId() const { return currentIndex + 1; }

    std::function<void(int)> onSelectionChanged;

    void resized() override
    {
        auto bounds = getLocalBounds();
        int btnW = 20;
        leftBtn.setBounds(bounds.removeFromLeft(btnW));
        rightBtn.setBounds(bounds.removeFromRight(btnW));
        nameBox.setBounds(bounds);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff12161f));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

private:
    juce::TextButton leftBtn, rightBtn;
    juce::Label nameBox;
    juce::StringArray options;
    int currentIndex = 0;

    void updateLabel()
    {
        if (currentIndex >= 0 && currentIndex < options.size())
            nameBox.setText(options[currentIndex], juce::dontSendNotification);
    }
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
    
    LogicProKnobLookAndFeel preLaf {juce::Colour(0xff00e5ff)};       // Preamp (Cyan)
    LogicProKnobLookAndFeel compLaf {juce::Colour(0xffffa200)};      // Compressor (Amber/Gold)
    LogicProKnobLookAndFeel eqLaf {juce::Colour(0xff00ff88)};        // EQ (Emerald Green)
    LogicProKnobLookAndFeel outputLaf {juce::Colour(0xffff00e5)};    // Output (Magenta)

    LogicProSwitchLookAndFeel switchLaf;
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

    // Module Stepper Selectors
    ModelStepperComponent preampStepper;
    ModelStepperComponent compStepper;
    ModelStepperComponent eqStepper;
    ModelStepperComponent outputStepper;
    
    juce::ToggleButton oversamplingButton;
    juce::Label oversamplingLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> preampSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> compSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> eqSelectorAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> outputSelectorAtt;
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

    void buildKnob(const juce::String& paramId, const juce::String& name, int section); // 0=Pre, 1=Comp, 2=EQ, 3=Output
    void buildButton(const juce::String& paramId, const juce::String& name, int section);
    void updateVisibility();

    float tapeReelAngle = 0.0f;
    float tubeGlowPhase = 0.0f;
    void drawVacuumTube(juce::Graphics& g, juce::Rectangle<float> bounds, float glowAmount);
    void drawTapeReels(juce::Graphics& g, juce::Rectangle<float> bounds, float rotationAngle);

    juce::OpenGLContext openGLContext;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogFxAudioProcessorEditor)
};
