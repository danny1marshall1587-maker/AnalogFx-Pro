#include "PluginProcessor.h"
#include "PluginEditor.h"

GlassmorphismComboBoxLookAndFeel::GlassmorphismComboBoxLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0x884a0080)); // Translucent purple
    setColour(juce::ComboBox::textColourId, juce::Colours::white);
    setColour(juce::ComboBox::arrowColourId, juce::Colours::white.withAlpha(0.7f));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xee2d004d)); // Darker translucent purple
    setColour(juce::PopupMenu::textColourId, juce::Colours::white);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xaa8000ff)); // Bright purple highlight
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void GlassmorphismComboBoxLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH);
    auto bounds = juce::Rectangle<float>(0, 0, width, height).reduced(1.0f);
    float cornerSize = height * 0.5f; // fully rounded

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(bounds, cornerSize);

    // Subtle glowing border
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

    // Draw Arrow
    juce::Path path;
    path.addTriangle(width - 20.0f, height * 0.4f, width - 10.0f, height * 0.4f, width - 15.0f, height * 0.6f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(path);
}

void GlassmorphismComboBoxLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(10, 0, box.getWidth() - 30, box.getHeight());
    label.setFont(juce::Font(14.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
}

void GlassmorphismComboBoxLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    auto bounds = juce::Rectangle<float>(0, 0, width, height);
    g.setColour(findColour(juce::PopupMenu::backgroundColourId));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 8.0f, 1.0f);
}

void GlassmorphismComboBoxLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, const bool isSeparator, const bool isActive, const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* const textColourToUse)
{
    juce::ignoreUnused(isTicked, hasSubMenu, shortcutKeyText, icon);
    if (isSeparator)
    {
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.fillRect(area.withSizeKeepingCentre(area.getWidth() - 10, 1));
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
        g.fillRoundedRectangle(area.toFloat().reduced(2.0f), 4.0f);
    }

    g.setColour(isActive ? findColour(juce::PopupMenu::textColourId) : juce::Colours::grey);
    g.setFont(juce::Font(14.0f, juce::Font::plain));
    g.drawText(text, area.reduced(10, 0), juce::Justification::centredLeft, true);
}

AnalogFxAudioProcessorEditor::AnalogFxAudioProcessorEditor(AnalogFxAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    bgTelefunken = juce::ImageCache::getFromMemory(BinaryData::telefunken_bg_png, BinaryData::telefunken_bg_pngSize);
    bgNevePre = juce::ImageCache::getFromMemory(BinaryData::neve_bg_png, BinaryData::neve_bg_pngSize);
    bgModernPre = juce::ImageCache::getFromMemory(BinaryData::modern_pre_bg_png, BinaryData::modern_pre_bg_pngSize);
    bgPultec = juce::ImageCache::getFromMemory(BinaryData::pultec_bg_png, BinaryData::pultec_bg_pngSize);
    bgModEq = juce::ImageCache::getFromMemory(BinaryData::modern_eq_bg_png, BinaryData::modern_eq_bg_pngSize);
    bgModComp = juce::ImageCache::getFromMemory(BinaryData::modern_comp_bg_png, BinaryData::modern_comp_bg_pngSize);
    bgNC76 = juce::ImageCache::getFromMemory(BinaryData::nc76_bg_png, BinaryData::nc76_bg_pngSize);
    bgLA2A = juce::ImageCache::getFromMemory(BinaryData::la2a_bg_png, BinaryData::la2a_bg_pngSize);
    bgFairchild = juce::ImageCache::getFromMemory(BinaryData::fairchild_bg_png, BinaryData::fairchild_bg_pngSize);
    bgDirtEq = juce::ImageCache::getFromMemory(BinaryData::dirt_eq_bg_png, BinaryData::dirt_eq_bg_pngSize);
    bgOutput = juce::ImageCache::getFromMemory(BinaryData::output_bg_png, BinaryData::output_bg_pngSize);

    // Setup Presets Directory
    presetDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                        .getChildFile("AnalogFx").getChildFile("Presets");
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();

    addAndMakeVisible(presetSelector);
    presetSelector.setLookAndFeel(&presetLaf);
    presetSelector.addListener(this);
    loadUserPresets();

    addAndMakeVisible(preampSelector);
    preampSelector.addItemList({"Bypass", "Telefunken", "Neve", "Modern"}, 1);
    preampSelectorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "preamp_type", preampSelector);
    preampSelector.addListener(this);

    addAndMakeVisible(compSelector);
    compSelector.addItemList({"Bypass", "NC76", "LA-2A", "Fairchild", "Modern VCA"}, 1);
    compSelectorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "comp_type", compSelector);
    compSelector.addListener(this);

    addAndMakeVisible(eqSelector);
    eqSelector.addItemList({"Bypass", "Dirt EQ", "Neve 1073", "Pultec EQP-1A", "Modern Surgical"}, 1);
    eqSelectorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "eq_type", eqSelector);
    eqSelector.addListener(this);

    addAndMakeVisible(scaleSelector);
    scaleSelector.addItemList({"Scale: 50%", "Scale: 75%", "Scale: 100%", "Scale: 150%", "Scale: 200%"}, 1);
    scaleSelector.setSelectedItemIndex(2, juce::dontSendNotification);
    scaleSelector.addListener(this);

    addAndMakeVisible(outputSelector);
    outputSelector.addItemList({"Bypass", "Vintage Tape", "British Iron", "Valve Summing", "Modern Polish"}, 1);
    outputSelectorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "output_type", outputSelector);
    outputSelector.addListener(this);

    addAndMakeVisible(oversamplingButton);
    oversamplingButton.setLookAndFeel(&switchLaf);
    oversamplingAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "oversampling", oversamplingButton);

    addAndMakeVisible(oversamplingLabel);
    oversamplingLabel.setText("OVERSAMPLING", juce::dontSendNotification);
    oversamplingLabel.setFont(10.0f);
    oversamplingLabel.setJustificationType(juce::Justification::centred);
    oversamplingLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));

    // Build Preamp Knobs (0)
    buildKnob("preamp_drive", "Drive", 0);
    buildKnob("preamp_trim", "Trim", 0);

    // Build Comp Knobs (1)
    buildKnob("comp_trim", "Trim", 1);
    buildKnob("nc76_thresh", "Threshold", 1);
    buildKnob("nc76_attack", "Attack", 1);
    buildKnob("nc76_release", "Release", 1);
    buildKnob("nc76_in_gain", "In Gain", 1);
    buildKnob("nc76_out_gain", "Out Gain", 1);
    buildKnob("nc76_mix", "Mix", 1);
    
    buildKnob("la2a_peak", "Peak", 1);
    buildKnob("la2a_gain", "Gain", 1);
    
    buildKnob("fc_l_thresh", "L Thresh", 1);
    buildKnob("fc_r_thresh", "R Thresh", 1);
    buildKnob("fc_l_bias", "L Bias", 1);
    buildKnob("fc_r_bias", "R Bias", 1);
    buildKnob("fc_l_makeup", "L Gain", 1);
    buildKnob("fc_r_makeup", "R Gain", 1);
    
    buildKnob("mc_thresh", "Threshold", 1);
    buildKnob("mc_ratio", "Ratio", 1);
    buildKnob("mc_attack", "Attack", 1);
    buildKnob("mc_release", "Release", 1);
    buildKnob("mc_makeup", "Makeup", 1);

    // Build EQ Knobs (2)
    buildKnob("eq_trim", "Trim", 2);
    
    buildKnob("lowpass_freq", "LPF", 2);
    buildKnob("lowpass_q", "LPF Q", 2);
    buildKnob("band1_gain", "HF Gain", 2);
    buildKnob("band1_freq", "HF Freq", 2);
    buildKnob("band2_gain", "HMF Gain", 2);
    buildKnob("band2_freq", "HMF Freq", 2);
    buildKnob("band2_q", "HMF Q", 2);
    buildKnob("band3_gain", "LMF Gain", 2);
    buildKnob("band3_freq", "LMF Freq", 2);
    buildKnob("band3_q", "LMF Q", 2);
    buildKnob("band4_gain", "LF Gain", 2);
    buildKnob("band4_freq", "LF Freq", 2);
    buildKnob("highpass_freq", "HPF", 2);
    buildKnob("highpass_q", "HPF Q", 2);
    
    buildKnob("nv_gain", "Gain", 2);
    buildKnob("nv_hf_gain", "HF", 2);
    buildKnob("nv_mf_gain", "MF", 2);
    buildKnob("nv_lf_gain", "LF", 2);
    buildKnob("nv_drive", "Drive", 2);
    
    buildKnob("pu_l_boost", "L Boost", 2);
    buildKnob("pu_l_atten", "L Atten", 2);
    buildKnob("pu_peak_q", "Peak Q", 2);
    buildKnob("pu_p_boost", "P Boost", 2);
    buildKnob("pu_h_atten", "H Atten", 2);

    buildButton("nv_phase", "PHASE", 2);
    buildButton("nv_hf_on", "HF ON", 2);
    buildButton("nv_mf_on", "MF ON", 2);
    buildButton("nv_lf_on", "LF ON", 2);
    buildButton("nv_hpf_on", "HPF ON", 2);
    buildButton("nv_eq_on", "MASTER", 2);
    
    buildKnob("ic1_drive", "IC1 DRV", 2);
    buildKnob("ic2_drive", "IC2 DRV", 2);
    buildKnob("drive_db", "DRIVE", 2);
    buildKnob("gain_db", "GAIN", 2);
    buildButton("dirt_eq_on", "EQ ON", 2);
    
    buildKnob("mq_l_freq", "LF Hz", 2);
    buildKnob("mq_l_gain", "LF Gain", 2);
    buildKnob("mq_m1_freq", "LMF Hz", 2);
    buildKnob("mq_m1_gain", "LMF Gain", 2);
    buildKnob("mq_m1_q", "LMF Q", 2);
    buildKnob("mq_m2_freq", "HMF Hz", 2);
    buildKnob("mq_m2_gain", "HMF Gain", 2);
    buildKnob("mq_m2_q", "HMF Q", 2);
    buildKnob("mq_h_freq", "HF Hz", 2);
    buildKnob("mq_h_gain", "HF Gain", 2);

    // Build Output Knobs (3)
    buildKnob("output_drive", "Drive", 3);
    buildButton("output_safe", "SAFE", 3);

    // Missing Dirt EQ Knobs
    buildKnob("lowpass_freq", "LP Hz", 2);
    buildKnob("lowpass_q", "LP Q", 2);
    buildKnob("band1_freq", "H Freq", 2);
    buildKnob("band1_gain", "High Gain", 2);
    buildKnob("band2_freq", "HM Freq", 2);
    buildKnob("band2_gain", "HM Gain", 2);
    buildKnob("band2_q", "HM Q", 2);
    buildKnob("band3_freq", "LM Freq", 2);
    buildKnob("band3_gain", "LM Gain", 2);
    buildKnob("band3_q", "LM Q", 2);
    buildKnob("band4_freq", "L Freq", 2);
    buildKnob("band4_gain", "Low Gain", 2);
    buildKnob("highpass_freq", "HP Hz", 2);
    buildKnob("highpass_q", "HP Q", 2);

    updateVisibility();
    setOpaque(true);
    openGLContext.setComponentPaintingEnabled(true);
    openGLContext.attachTo(*this);
    startTimerHz(30);
}

AnalogFxAudioProcessorEditor::~AnalogFxAudioProcessorEditor()
{
    openGLContext.detach();
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void AnalogFxAudioProcessorEditor::timerCallback()
{
    float preRMS = audioProcessor.preampLevel.load();
    float compRMS = audioProcessor.compLevel.load();
    float eqRMS = audioProcessor.eqLevel.load();
    
    preMeterVal = juce::jmax(preRMS, preMeterVal * 0.8f);
    compMeterVal = juce::jmax(compRMS, compMeterVal * 0.8f);
    eqMeterVal = juce::jmax(eqRMS, eqMeterVal * 0.8f);
    compGRVal = audioProcessor.compGR.load();

    tapeReelAngle += 0.06f;
    if (tapeReelAngle > juce::MathConstants<float>::twoPi) tapeReelAngle -= juce::MathConstants<float>::twoPi;
    
    tubeGlowPhase += 0.04f;
    if (tubeGlowPhase > juce::MathConstants<float>::twoPi) tubeGlowPhase -= juce::MathConstants<float>::twoPi;
    
    repaint();
}

void AnalogFxAudioProcessorEditor::drawVacuumTube(juce::Graphics& g, juce::Rectangle<float> bounds, float glowAmount)
{
    auto w = bounds.getWidth();
    auto h = bounds.getHeight();
    auto cx = bounds.getCentreX();
    auto cy = bounds.getCentreY();

    // Outer Glass Envelope
    juce::Path tubePath;
    tubePath.addRoundedRectangle(cx - w * 0.3f, cy - h * 0.45f, w * 0.6f, h * 0.9f, w * 0.25f);

    // Dark Glass Gradient
    juce::ColourGradient glassGrad(juce::Colour(0x66333333), cx, cy - h * 0.4f, juce::Colour(0xaa111111), cx, cy + h * 0.4f, false);
    g.setGradientFill(glassGrad);
    g.fillPath(tubePath);

    // Glowing Filament Wire
    float pulse = 0.75f + 0.25f * std::sin(tubeGlowPhase);
    juce::Colour filamentCol = juce::Colour(0xffff5500).withAlpha(juce::jlimit(0.2f, 1.0f, glowAmount * pulse));

    // Outer Heat Glow Aura
    g.setColour(filamentCol.withAlpha(0.25f * pulse));
    g.fillEllipse(cx - w * 0.45f, cy - h * 0.25f, w * 0.9f, h * 0.55f);

    // Core Glowing Wire Loops
    juce::Path wire;
    wire.startNewSubPath(cx - w * 0.15f, cy + h * 0.3f);
    wire.lineTo(cx - w * 0.1f, cy - h * 0.1f);
    wire.quadraticTo(cx, cy - h * 0.25f, cx + w * 0.1f, cy - h * 0.1f);
    wire.lineTo(cx + w * 0.15f, cy + h * 0.3f);

    g.setColour(filamentCol);
    g.strokePath(wire, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Core Yellow Heat Line
    g.setColour(juce::Colours::yellow.withAlpha(glowAmount * pulse));
    g.strokePath(wire, juce::PathStrokeType(1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Metallic Base Pin Connector
    g.setColour(juce::Colour(0xff444444));
    g.fillRect(cx - w * 0.25f, cy + h * 0.4f, w * 0.5f, h * 0.1f);

    // Glass Reflection Specular Line
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawLine(cx - w * 0.22f, cy - h * 0.35f, cx - w * 0.22f, cy + h * 0.3f, 1.5f);
}

void AnalogFxAudioProcessorEditor::drawTapeReels(juce::Graphics& g, juce::Rectangle<float> bounds, float rotationAngle)
{
    auto r = bounds.getHeight() * 0.38f;
    auto cy = bounds.getCentreY();
    auto leftX = bounds.getX() + bounds.getWidth() * 0.3f;
    auto rightX = bounds.getX() + bounds.getWidth() * 0.7f;

    auto drawSingleReel = [&](float cx, float angle) {
        // Metallic Reel Flange Base
        g.setColour(juce::Colour(0xff222832));
        g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
        g.setColour(juce::Colour(0xff667080));
        g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

        // Dark Magnetic Tape Pack
        float tapeRadius = r * 0.82f;
        g.setColour(juce::Colour(0xff120a06));
        g.fillEllipse(cx - tapeRadius, cy - tapeRadius, tapeRadius * 2.0f, tapeRadius * 2.0f);

        // 3-Spoke Aluminum Hub Openings
        g.setColour(juce::Colour(0xff454e5b));
        for (int i = 0; i < 3; ++i) {
            float spokeAngle = angle + i * (juce::MathConstants<float>::twoPi / 3.0f);
            float hx = cx + (r * 0.5f) * std::cos(spokeAngle);
            float hy = cy + (r * 0.5f) * std::sin(spokeAngle);
            g.fillEllipse(hx - r * 0.22f, hy - r * 0.22f, r * 0.44f, r * 0.44f);
        }

        // Center Hub Cap
        g.setColour(juce::Colour(0xff8a95a5));
        g.fillEllipse(cx - r * 0.25f, cy - r * 0.25f, r * 0.5f, r * 0.5f);
        g.setColour(juce::Colours::black);
        g.drawEllipse(cx - r * 0.25f, cy - r * 0.25f, r * 0.5f, r * 0.5f, 1.0f);
    };

    drawSingleReel(leftX, rotationAngle);
    drawSingleReel(rightX, -rotationAngle * 1.1f);

    // Drawn Tape Path Line connecting reels
    g.setColour(juce::Colour(0xff2d1e16));
    g.drawLine(leftX, cy + r * 0.7f, rightX, cy + r * 0.7f, 2.5f);
}

void AnalogFxAudioProcessorEditor::buildKnob(const juce::String& paramId, const juce::String& name, int section)
{
    auto slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    slider->setComponentID(paramId);
    
    if (section == 0) slider->setLookAndFeel(&preLaf);
    else if (section == 1) slider->setLookAndFeel(&compLaf);
    else if (section == 2) slider->setLookAndFeel(&eqLaf);
    else if (section == 3) slider->setLookAndFeel(&outputLaf);
    
    if (paramId.containsIgnoreCase("gain") || paramId.containsIgnoreCase("drive") || paramId.containsIgnoreCase("db") || paramId.containsIgnoreCase("atten") || paramId.containsIgnoreCase("thresh"))
        slider->setTextValueSuffix(" dB");
    else if (paramId.containsIgnoreCase("freq") || paramId.containsIgnoreCase("hz") || paramId.containsIgnoreCase("pass"))
        slider->setTextValueSuffix(" Hz");

    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(*slider);

    auto label = std::make_unique<juce::Label>("", name);
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    label->setFont(12.0f);
    addAndMakeVisible(*label);

    if (section == 0) {
        preAtts.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramId, *slider));
        preSliders.push_back(std::move(slider));
        preLabels.push_back(std::move(label));
    } else if (section == 1) {
        compAtts.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramId, *slider));
        compSliders.push_back(std::move(slider));
        compLabels.push_back(std::move(label));
    } else if (section == 2) {
        eqAtts.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramId, *slider));
        eqSliders.push_back(std::move(slider));
        eqLabels.push_back(std::move(label));
    } else if (section == 3) {
        outputAtts.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramId, *slider));
        outputSliders.push_back(std::move(slider));
        outputLabels.push_back(std::move(label));
    }
}

void AnalogFxAudioProcessorEditor::buildButton(const juce::String& paramId, const juce::String& name, int section)
{
    auto btn = std::make_unique<juce::ToggleButton>(name);
    btn->setComponentID(paramId);
    btn->setLookAndFeel(&modernButtonLaf);
    addAndMakeVisible(*btn);
    
    auto att = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, paramId, *btn);
    
    if (section == 2) {
        eqButtonAtts.push_back(std::move(att));
        eqButtons.push_back(std::move(btn));
    } else if (section == 3) {
        outputButtonAtts.push_back(std::move(att));
        outputButtons.push_back(std::move(btn));
    }
}

void AnalogFxAudioProcessorEditor::comboBoxChanged(juce::ComboBox* cb)
{
    if (cb == &presetSelector) {
        juce::String selection = presetSelector.getText();
        if (selection == "Save Current Preset...") {
            presetSelector.setSelectedItemIndex(-1, juce::dontSendNotification);
            presetSelector.setTextWhenNothingSelected("Select Preset...");
            saveUserPreset();
        } else if (selection == "Open Presets Folder...") {
            presetSelector.setSelectedItemIndex(-1, juce::dontSendNotification);
            presetSelector.setTextWhenNothingSelected("Select Preset...");
            presetDirectory.revealToUser();
        } else if (presetSelector.getSelectedId() > 0) {
            int id = presetSelector.getSelectedId();
            int numFactory = audioProcessor.getNumPrograms();
            if (id <= numFactory) {
                // Factory preset selected
                audioProcessor.setCurrentProgram(id - 1);
            } else {
                // User preset selected
                int userIndex = id - numFactory - 1; // -1 because ID counts from 1
                if (userIndex >= 0 && userIndex < userPresets.size()) {
                    juce::File f = presetDirectory.getChildFile(userPresets[userIndex] + ".xml");
                    if (f.existsAsFile()) {
                        std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(f);
                        if (xml != nullptr) {
                            if (xml->hasTagName(audioProcessor.apvts.state.getType())) {
                                audioProcessor.apvts.replaceState(juce::ValueTree::fromXml(*xml));
                            }
                        }
                    }
                }
            }
        }
        return;
    }

    if (cb == &scaleSelector)
    {
        int idx = scaleSelector.getSelectedItemIndex();
        if (idx == 0) currentScale = 0.5f;
        else if (idx == 1) currentScale = 0.75f;
        else if (idx == 2) currentScale = 1.0f;
        else if (idx == 3) currentScale = 1.5f;
        else if (idx == 4) currentScale = 2.0f;
    }
    
    updateVisibility();
}

void AnalogFxAudioProcessorEditor::updateVisibility()
{
    int preType = preampSelector.getSelectedId();
    int compType = compSelector.getSelectedId();
    int eqType = eqSelector.getSelectedId();

    int baseW = 800;
    int baseH = (eqType == 2) ? 1550 : 1250;

    int targetW = (int)(baseW * currentScale);
    int targetH = (int)(baseH * currentScale);

    for (auto& s : preSliders) s->setVisible(false);
    for (auto& l : preLabels) l->setVisible(false);
    for (auto& s : compSliders) s->setVisible(false);
    for (auto& l : compLabels) l->setVisible(false);
    for (auto& s : eqSliders) s->setVisible(false);
    for (auto& l : eqLabels) l->setVisible(false);
    for (auto& b : eqButtons) b->setVisible(false);

    auto updateLaf = [&](const juce::String& param, juce::LookAndFeel* laf) {
        auto check = [&](auto& list) {
            for (auto& s : list) {
                if (s->getComponentID() == param) {
                    if (&s->getLookAndFeel() != laf) s->setLookAndFeel(laf);
                }
            }
        };
        check(preSliders);
        check(compSliders);
        check(eqSliders);
        check(outputSliders);
    };

    // Update Preamp Knob Accent Color based on Model
    if (preType == 2) preLaf.setAccentColour(juce::Colour(0xffff8c00));      // Telefunken (Warm Amber)
    else if (preType == 3) preLaf.setAccentColour(juce::Colour(0xffe53935)); // Neve 1073 (Stepped Red)
    else preLaf.setAccentColour(juce::Colour(0xff00e5ff));                   // Boutique (Cyan)

    // Update Comp Knob Accent Color based on Model
    if (compType == 2) compLaf.setAccentColour(juce::Colour(0xffffc107));      // 1176 (Amber VU)
    else if (compType == 3) compLaf.setAccentColour(juce::Colour(0xff2979ff)); // LA-2A (Optical Blue)
    else if (compType == 4) compLaf.setAccentColour(juce::Colour(0xffffd700)); // Fairchild (Gold)
    else compLaf.setAccentColour(juce::Colour(0xffff9900));                   // Precision VCA (Amber)

    // Update EQ Knob Accent Color based on Model
    if (eqType == 2) eqLaf.setAccentColour(juce::Colour(0xffd4a359));          // Dirt EQ (Copper/Gold)
    else if (eqType == 3) eqLaf.setAccentColour(juce::Colour(0xff00e5ff));     // Neve EQ (Cyan)
    else if (eqType == 4) eqLaf.setAccentColour(juce::Colour(0xff00e5ff));     // Pultec (Seafoam Teal)
    else eqLaf.setAccentColour(juce::Colour(0xff00ff88));                      // Surgical (Emerald)

    // Update Output Stage Knob Accent Color based on Model
    int outType = outputSelector.getSelectedId();
    if (outType == 2) outputLaf.setAccentColour(juce::Colour(0xffff9900));     // Tape Saturation (Mahogany Amber)
    else if (outType == 3) outputLaf.setAccentColour(juce::Colour(0xffff3d00));// Tube Valve (Crimson Red)
    else if (outType == 4) outputLaf.setAccentColour(juce::Colour(0xffffc107));// Console (Brass Gold)
    else outputLaf.setAccentColour(juce::Colour(0xffff00e5));                 // Clean Digital (Magenta)

    updateLaf("preamp_drive", &preLaf);
    updateLaf("preamp_trim", &preLaf);
    updateLaf("comp_trim", &compLaf);
    updateLaf("eq_trim", &eqLaf);
    updateLaf("output_drive", &outputLaf);

    // Preamp Visibility
    if (preType > 1) { // 1 is Bypass
        for (size_t i = 0; i < preSliders.size(); ++i) {
            preSliders[i]->setVisible(true);
            preLabels[i]->setVisible(true);
        }
    }

    // Comp Visibility
    if (compType > 1) {
        juce::String prefix = "";
        if (compType == 2) prefix = "nc76";
        else if (compType == 3) prefix = "la2a";
        else if (compType == 4) prefix = "fc";
        else if (compType == 5) prefix = "mc";

        for (size_t i = 0; i < compSliders.size(); ++i) {
            if (compSliders[i]->getComponentID().startsWith(prefix) || compSliders[i]->getComponentID() == "comp_trim") {
                compSliders[i]->setVisible(true);
                compLabels[i]->setVisible(true);
            }
        }
    }

    // EQ Visibility
    if (eqType > 1) {
        juce::String prefix = "";
        if (eqType == 2) prefix = "band"; // Dirt uses band, highpass, etc.
        else if (eqType == 3) prefix = "nv";
        else if (eqType == 4) prefix = "pu";
        else if (eqType == 5) prefix = "mq";

        for (size_t i = 0; i < eqSliders.size(); ++i) {
            auto id = eqSliders[i]->getComponentID();
            if (id.startsWith(prefix) || id == "eq_trim" || 
               (eqType == 2 && (id.startsWith("lowpass") || id.startsWith("highpass") || id.startsWith("ic") || id == "drive_db" || id == "gain_db"))) {
                eqSliders[i]->setVisible(true);
                eqLabels[i]->setVisible(true);
            }
        }
        
        for (auto& b : eqButtons) {
            auto id = b->getComponentID();
            if (id.startsWith(prefix) || (eqType == 2 && id == "dirt_eq_on")) b->setVisible(true);
        }
    }

    // Output Visibility
    outType = outputSelector.getSelectedId();
    for (auto& s : outputSliders) s->setVisible(false);
    for (auto& l : outputLabels) l->setVisible(false);
    if (outType > 1) {
        for (size_t i = 0; i < outputSliders.size(); ++i) {
            outputSliders[i]->setVisible(true);
            outputLabels[i]->setVisible(true);
        }
        for (auto& b : outputButtons) b->setVisible(true);
    }

    // Now that visibility is set, handle layout and possible resize
    if (getWidth() != targetW || getHeight() != targetH)
    {
        setSize(targetW, targetH);
    }
    else
    {
        resized();
    }
    
    repaint();
    openGLContext.triggerRepaint();
}

void AnalogFxAudioProcessorEditor::paint(juce::Graphics& g)
{
    int w = getWidth();
    int h = getHeight();

    // 1. Deep macOS / Logic Pro Dark Glass Background
    juce::ColourGradient bgGrad(juce::Colour(0xff0e1117), 0, 0, juce::Colour(0xff161b24), 0, (float)h, false);
    g.setGradientFill(bgGrad);
    g.fillRect(0, 0, w, h);

    // Subtle Logic Pro Brushed Metal Grid Lines
    g.setColour(juce::Colours::white.withAlpha(0.015f));
    for (int yLine = 0; yLine < h; yLine += 4)
        g.drawHorizontalLine(yLine, 0.0f, (float)w);

    int preType = preampSelector.getSelectedId();
    int compType = compSelector.getSelectedId();
    int eqType = eqSelector.getSelectedId();
    int outType = outputSelector.getSelectedId();

    int taskBarH = (int)(50 * currentScale);
    int preH = (int)(300 * currentScale);
    int compH = (int)(300 * currentScale);
    int eqH = (eqType == 2) ? (int)(600 * currentScale) : (int)(300 * currentScale);
    int outH = (int)(300 * currentScale);

    int preY = taskBarH;
    int compY = preY + preH;
    int eqY = compY + compH;
    int outY = eqY + eqH;

    // 2. Logic Pro Header Bar
    g.setColour(juce::Colour(0xff12151d));
    g.fillRect(0, 0, w, taskBarH);
    g.setColour(juce::Colour(0xff00e5ff).withAlpha(0.4f));
    g.drawHorizontalLine(taskBarH - 1, 0.0f, (float)w);

    // Title Logo Badge ("ANALOG FX PRO")
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f * currentScale, juce::Font::bold));
    g.drawText("ANALOG FX PRO", (int)(18 * currentScale), 0, (int)(180 * currentScale), taskBarH, juce::Justification::centredLeft);
    
    g.setColour(juce::Colour(0xff00e5ff));
    g.setFont(juce::Font(10.0f * currentScale, juce::Font::plain));
    g.drawText("LOGIC PRO EDITION v2.2.1", (int)(155 * currentScale), (int)(2 * currentScale), (int)(160 * currentScale), taskBarH, juce::Justification::centredLeft);

    // 3. Logic Pro Hardware Section Cards & Banners with Model-Specific Color Themes
    auto drawHardwareCard = [&](int sectionIndex, int y, int cardH, int type) {
        auto cardBounds = juce::Rectangle<float>(10.0f * currentScale, (float)y + 4.0f * currentScale,
                                                (float)w - 20.0f * currentScale, (float)cardH - 8.0f * currentScale);

        juce::Colour panelBg = juce::Colour(0xff181d27);
        juce::Colour headerBg = juce::Colour(0xff12151d);
        juce::Colour accentCol = juce::Colour(0xff00e5ff);
        juce::String titleBadge = "";
        bool active = (type > 1);

        if (sectionIndex == 0) { // PREAMP
            if (type == 2) { // Telefunken V76 Tube
                panelBg = juce::Colour(0xff343b42);
                headerBg = juce::Colour(0xff272d33);
                accentCol = juce::Colour(0xffff8c00);
                titleBadge = "TELEFUNKEN V76 TUBE PREAMP";
            } else if (type == 3) { // Neve 1073 Pre
                panelBg = juce::Colour(0xff1d2630);
                headerBg = juce::Colour(0xff141c24);
                accentCol = juce::Colour(0xffe53935);
                titleBadge = "NEVE 1073 CONSOLE PREAMP";
            } else if (type == 4) { // Boutique Digital
                panelBg = juce::Colour(0xff10151d);
                headerBg = juce::Colour(0xff0a0e14);
                accentCol = juce::Colour(0xff00e5ff);
                titleBadge = "BOUTIQUE DIGITAL PREAMP";
            } else titleBadge = "PREAMP & SATURATION (BYPASSED)";
        } else if (sectionIndex == 1) { // COMPRESSOR
            if (type == 2) { // UREI 1176 LN
                panelBg = juce::Colour(0xff1c1f22);
                headerBg = juce::Colour(0xff121417);
                accentCol = juce::Colour(0xffffc107);
                titleBadge = "UREI 1176 LN LIMITER";
            } else if (type == 3) { // LA-2A
                panelBg = juce::Colour(0xffb5bec6);
                headerBg = juce::Colour(0xff8c96a0);
                accentCol = juce::Colour(0xff2979ff);
                titleBadge = "TELETRONIX LA-2A OPTICAL COMPRESSOR";
            } else if (type == 4) { // Fairchild 670
                panelBg = juce::Colour(0xff242629);
                headerBg = juce::Colour(0xff18191c);
                accentCol = juce::Colour(0xffffd700);
                titleBadge = "FAIRCHILD 670 TUBE LIMITER";
            } else if (type == 5) { // Modern VCA
                panelBg = juce::Colour(0xff141820);
                headerBg = juce::Colour(0xff0c1017);
                accentCol = juce::Colour(0xffff9900);
                titleBadge = "PRECISION VCA COMPRESSOR";
            } else titleBadge = "DYNAMICS COMPRESSOR (BYPASSED)";
        } else if (sectionIndex == 2) { // EQ
            if (type == 2) { // Dirt Inductor EQ
                panelBg = juce::Colour(0xff36281e);
                headerBg = juce::Colour(0xff241912);
                accentCol = juce::Colour(0xffd4a359);
                titleBadge = "CUSTOM DIRT INDUCTOR EQ";
            } else if (type == 3) { // Neve 1073 EQ
                panelBg = juce::Colour(0xff1d2630);
                headerBg = juce::Colour(0xff141c24);
                accentCol = juce::Colour(0xff00e5ff);
                titleBadge = "NEVE 1073 EQUALIZER";
            } else if (type == 4) { // Pultec EQP-1A
                panelBg = juce::Colour(0xff16424c);
                headerBg = juce::Colour(0xff0e2c33);
                accentCol = juce::Colour(0xff00e5ff);
                titleBadge = "PULTEC EQP-1A PROGRAM EQ";
            } else if (type == 5) { // Surgical Parametric EQ
                panelBg = juce::Colour(0xff101620);
                headerBg = juce::Colour(0xff0a0f17);
                accentCol = juce::Colour(0xff00ff88);
                titleBadge = "SURGICAL PARAMETRIC EQ";
            } else titleBadge = "EQUALIZER (BYPASSED)";
        } else if (sectionIndex == 3) { // OUTPUT COLOR
            if (type == 2) { // Tape Saturation
                panelBg = juce::Colour(0xff2a1d17);
                headerBg = juce::Colour(0xff1a100a);
                accentCol = juce::Colour(0xffff9900);
                titleBadge = "ANALOG TAPE SATURATION";
            } else if (type == 3) { // Tube Valve Drive
                panelBg = juce::Colour(0xff341818);
                headerBg = juce::Colour(0xff220c0c);
                accentCol = juce::Colour(0xffff3d00);
                titleBadge = "TUBE VALVE DRIVE";
            } else if (type == 4) { // Console Warmth
                panelBg = juce::Colour(0xff22272e);
                headerBg = juce::Colour(0xff161a20);
                accentCol = juce::Colour(0xffffc107);
                titleBadge = "ANALOG CONSOLE WARMTH";
            } else { // Clean Digital
                panelBg = juce::Colour(0xff141923);
                headerBg = juce::Colour(0xff0d111a);
                accentCol = juce::Colour(0xffff00e5);
                titleBadge = "TRANSPARENT DIGITAL OUTPUT";
            }
        }

        // Card Glass Body
        g.setColour(panelBg.withAlpha(0.92f));
        g.fillRoundedRectangle(cardBounds, 8.0f * currentScale);

        // Header Banner Box
        auto bannerBounds = juce::Rectangle<float>(cardBounds.getX(), cardBounds.getY(), cardBounds.getWidth(), 38.0f * currentScale);
        g.setColour(headerBg.withAlpha(0.95f));
        g.fillRoundedRectangle(bannerBounds, 8.0f * currentScale);
        g.fillRect(bannerBounds.withTrimmedTop(10.0f * currentScale)); // Flatten bottom corners

        // Glowing Header Accent Line
        g.setColour(active ? accentCol : juce::Colours::white.withAlpha(0.2f));
        g.drawHorizontalLine((int)(bannerBounds.getBottom() - 1), bannerBounds.getX(), bannerBounds.getRight());

        // Card Border
        g.setColour(active ? accentCol.withAlpha(0.4f) : juce::Colours::white.withAlpha(0.1f));
        g.drawRoundedRectangle(cardBounds, 8.0f * currentScale, 1.0f);

        // Power Dot Indicator
        float dotX = cardBounds.getX() + 16.0f * currentScale;
        float dotY = bannerBounds.getCentreY();
        g.setColour(active ? accentCol : juce::Colour(0xff444444));
        g.fillEllipse(dotX - 4.0f, dotY - 4.0f, 8.0f * currentScale, 8.0f * currentScale);

        if (active) {
            g.setColour(accentCol.withAlpha(0.35f));
            g.fillEllipse(dotX - 7.0f, dotY - 7.0f, 14.0f * currentScale, 14.0f * currentScale);
        }

        // Section Title Text
        bool isLA2A = (sectionIndex == 1 && type == 3);
        g.setColour((active && !isLA2A) ? juce::Colours::white : juce::Colours::white.withAlpha(0.7f));
        g.setFont(juce::Font(12.0f * currentScale, juce::Font::bold));
        g.drawText(titleBadge, (int)(dotX + 12.0f * currentScale), (int)(bannerBounds.getY()), (int)(240 * currentScale), (int)bannerBounds.getHeight(), juce::Justification::centredLeft);

        // Render Animated Tube Graphic for Tube Models
        if (sectionIndex == 0 && type == 2) { // Telefunken V76 Tube
            auto tubeBounds = juce::Rectangle<float>(cardBounds.getRight() - 130.0f * currentScale, cardBounds.getY() + 45.0f * currentScale, 50.0f * currentScale, 90.0f * currentScale);
            drawVacuumTube(g, tubeBounds, 0.85f);
        } else if (sectionIndex == 3 && type == 3) { // Output Tube Valve Drive
            auto tubeBounds = juce::Rectangle<float>(cardBounds.getRight() - 130.0f * currentScale, cardBounds.getY() + 45.0f * currentScale, 50.0f * currentScale, 90.0f * currentScale);
            drawVacuumTube(g, tubeBounds, 0.95f);
        }

        // Render Animated Tape Reels Graphic for Tape Model
        if (sectionIndex == 3 && type == 2) { // Output Tape Saturation
            auto reelBounds = juce::Rectangle<float>(cardBounds.getRight() - 210.0f * currentScale, cardBounds.getY() + 40.0f * currentScale, 130.0f * currentScale, 100.0f * currentScale);
            drawTapeReels(g, reelBounds, tapeReelAngle);
        }
    };

    drawHardwareCard(0, preY, preH, preType);
    drawHardwareCard(1, compY, compH, compType);
    drawHardwareCard(2, eqY, eqH, eqType);
    if (outType > 1) drawHardwareCard(3, outY, outH, outType);

    // 4. Logic Pro Neon LED Level Meters
    auto drawMeter = [&](int sectionIndex, float meterValue, juce::Colour color, bool isGR = false) {
        int curSecY = (sectionIndex == 0) ? preY : (sectionIndex == 1) ? compY : (sectionIndex == 2) ? eqY : outY;
        int meterW = (int)(isGR ? 8 : 10) * currentScale;
        int meterH = (int)(190 * currentScale);
        int x = w - (int)(isGR ? 65 : 45) * currentScale;
        int y = curSecY + (int)(55 * currentScale);

        g.setColour(juce::Colour(0xff0c0f14));
        g.fillRoundedRectangle((float)x, (float)y, (float)meterW, (float)meterH, 3.0f);
        g.setColour(color.withAlpha(0.3f));
        g.drawRoundedRectangle((float)x, (float)y, (float)meterW, (float)meterH, 3.0f, 1.0f);

        float db = isGR ? meterValue : (20.0f * std::log10(std::max(meterValue, 0.0001f)));
        float minDb = isGR ? 0.0f : -60.0f;
        float maxDb = isGR ? 20.0f : 6.0f;
        float norm = juce::jlimit(0.0f, 1.0f, (db - minDb) / (maxDb - minDb));
        
        int fillH = static_cast<int>(norm * meterH);
        
        juce::Colour finalColor = color;
        if (isGR) {
            if (meterValue > 6.0f) finalColor = juce::Colours::red;
            else finalColor = juce::Colour(0xffffa200);
        } else {
            if (db > -3.0f) finalColor = juce::Colours::red;
            else if (db > -18.0f) finalColor = juce::Colour(0xffffd700);
        }

        g.setColour(finalColor);
        if (isGR) g.fillRoundedRectangle((float)x, (float)y, (float)meterW, (float)fillH, 2.0f);
        else g.fillRoundedRectangle((float)x, (float)(y + meterH - fillH), (float)meterW, (float)fillH, 2.0f);
    };
    
    if (preType > 1) drawMeter(0, preMeterVal, juce::Colour(0xff00e5ff));
    if (compType > 1) {
        drawMeter(1, compMeterVal, juce::Colour(0xffffa200));
        drawMeter(1, std::abs(compGRVal), juce::Colour(0xffffd700), true);
    }
    if (eqType > 1) drawMeter(2, eqMeterVal, juce::Colour(0xff00ff88));
    if (outType > 1) drawMeter(3, eqMeterVal, juce::Colour(0xffff00e5));
}

void AnalogFxAudioProcessorEditor::resized()
{
    int w = getWidth();
    int margin = (int)(20 * currentScale);
    auto selectorW = (int)(150 * currentScale);
    auto selectorH = (int)(24 * currentScale);

    // Variable Section Heights
    int taskBarH = (int)(50 * currentScale);
    int preH = (int)(300 * currentScale);
    int compH = (int)(300 * currentScale);
    int eqH = (eqSelector.getSelectedId() == 2) ? (int)(600 * currentScale) : (int)(300 * currentScale);
    int outH = (int)(300 * currentScale);

    int preY = taskBarH;
    int compY = preY + preH;
    int eqY = compY + compH;
    int outY = eqY + eqH;

    // GPU Optimization: Pre-scale backgrounds to match the window size once
    auto scaleBg = [&](const juce::Image& src, juce::Image& dest, int tw, int th) {
        if (src.isValid()) dest = src.rescaled(tw, th, juce::Graphics::highResamplingQuality);
    };

    scaleBg(bgTelefunken, s_bgTelefunken, w, preH);
    scaleBg(bgNevePre, s_bgNevePre, w, preH);
    scaleBg(bgModernPre, s_bgModernPre, w, preH);
    scaleBg(bgNC76, s_bgNC76, w, compH);
    scaleBg(bgLA2A, s_bgLA2A, w, compH);
    scaleBg(bgFairchild, s_bgFairchild, w, compH);
    scaleBg(bgModComp, s_bgModComp, w, compH);
    scaleBg(bgDirtEq, s_bgDirtEq, w, eqH);
    scaleBg(bgPultec, s_bgPultec, w, eqH);
    scaleBg(bgModEq, s_bgModEq, w, eqH);
    scaleBg(bgOutput, s_bgOutput, w, outH);

    // Place Preset Selector in top task bar
    int presetW = (int)(250 * currentScale);
    int presetH = (int)(28 * currentScale);
    presetSelector.setBounds((w - presetW) / 2, (taskBarH - presetH) / 2, presetW, presetH);

    // Embed Module Selectors cleanly inside Section Header Banners
    preampSelector.setBounds((int)(280 * currentScale), preY + (int)(11 * currentScale), selectorW, selectorH);
    compSelector.setBounds((int)(310 * currentScale), compY + (int)(11 * currentScale), selectorW, selectorH);
    eqSelector.setBounds((int)(280 * currentScale), eqY + (int)(11 * currentScale), selectorW, selectorH);
    outputSelector.setBounds((int)(240 * currentScale), outY + (int)(11 * currentScale), selectorW, selectorH);
    
    scaleSelector.setBounds(w - selectorW - margin, preY + (int)(11 * currentScale), selectorW, selectorH);
    
    // Oversampling Button (Top section, away from meter)
    int osW = (int)(140 * currentScale);
    int osH = (int)(45 * currentScale);
    int osX = w - osW - (int)(150 * currentScale); 
    int osY = preY + (int)(60 * currentScale);
    oversamplingButton.setBounds(osX, osY, osW, osH);
    oversamplingLabel.setBounds(osX, osY + osH, osW, (int)(15 * currentScale));

    // Safe Button Layout (Output section, away from meter)
    int safeW = (int)(120 * currentScale);
    int safeH = (int)(40 * currentScale);
    int safeX = w - safeW - (int)(150 * currentScale);
    int safeY = outY + (int)(60 * currentScale);
    for (auto& b : outputButtons) {
        b->setBounds(safeX, safeY, safeW, safeH);
    }

    auto getKnobSize = [&](int section, const juce::String& id) {
        if (section == 0) return (int)(120 * currentScale); // Preamp
        if (section == 2) { // EQ
            if (id.containsIgnoreCase("freq") || id.containsIgnoreCase("q") || id.endsWithIgnoreCase("hz")) 
                return (int)(55 * currentScale); // Smaller for EQ
            return (int)(75 * currentScale); 
        }
        if (section == 3) return (int)(120 * currentScale); // Output
        return (int)(75 * currentScale); 
    };

    auto layoutSection = [&](std::vector<std::unique_ptr<juce::Slider>>& sliders, std::vector<std::unique_ptr<juce::Label>>& labels, int startY, int sectionIdx) {
        std::vector<int> vis;
        for (size_t i = 0; i < sliders.size(); ++i) {
            if (sliders[i]->isVisible()) vis.push_back((int)i);
        }
        if (vis.empty()) return;

        struct Row { std::vector<int> idxs; int width = 0; int maxH = 0; };
        std::vector<Row> rows;
        Row cur;
        int maxW = w - (int)(120 * currentScale); // Space for meter & side graphics
        int spacingX = (int)(20 * currentScale);

        for (int i : vis) {
            int sz = getKnobSize(sectionIdx, sliders[i]->getComponentID());
            if (!cur.idxs.empty() && cur.width + spacingX + sz > maxW) {
                rows.push_back(cur);
                cur = Row();
            }
            cur.idxs.push_back(i);
            cur.width += (cur.idxs.size() > 1 ? spacingX : 0) + sz;
            cur.maxH = std::max(cur.maxH, sz);
        }
        if (!cur.idxs.empty()) rows.push_back(cur);

        int curY = startY + (int)(65 * currentScale);
        for (auto& row : rows) {
            int x = (w - row.width) / 2;
            for (int i : row.idxs) {
                int sz = getKnobSize(sectionIdx, sliders[i]->getComponentID());
                int yOff = (row.maxH - sz) / 2;
                sliders[i]->setBounds(x, curY + yOff, sz, sz);
                labels[i]->setBounds(x - (int)(10 * currentScale), curY + yOff + sz + (int)(4 * currentScale), sz + (int)(20 * currentScale), (int)(18 * currentScale));
                x += sz + spacingX;
            }
            curY += row.maxH + (int)(45 * currentScale);
        }
    };

    layoutSection(preSliders, preLabels, preY, 0);
    layoutSection(compSliders, compLabels, compY, 1);
    layoutSection(eqSliders, eqLabels, eqY, 2);
    layoutSection(outputSliders, outputLabels, outY, 3);
    
    // Layout EQ Buttons
    int btnX = margin + selectorW + margin;
    int btnY = eqY + (int)(20 * currentScale);
    for (auto& b : eqButtons) {
        if (b->isVisible()) {
            b->setBounds(btnX, btnY, (int)(80 * currentScale), selectorH);
            btnX += (int)(90 * currentScale);
            if (btnX > w - (int)(150 * currentScale)) {
                btnX = margin + selectorW + margin;
                btnY += selectorH + (int)(5 * currentScale);
            }
        }
    }
}

void AnalogFxAudioProcessorEditor::loadUserPresets()
{
    presetSelector.clear();
    presetSelector.setTextWhenNothingSelected("Select Preset...");

    int itemId = 1;
    
    // 1. Factory Presets
    presetSelector.addSectionHeading("Factory Presets");
    int numFactory = audioProcessor.getNumPrograms();
    for (int i = 0; i < numFactory; ++i) {
        presetSelector.addItem(audioProcessor.getProgramName(i), itemId++);
    }
    
    presetSelector.addSeparator();

    // 2. User Presets
    presetSelector.addSectionHeading("User Presets");
    userPresets.clear();
    juce::Array<juce::File> files = presetDirectory.findChildFiles(juce::File::findFiles, false, "*.xml");
    for (auto& f : files) {
        userPresets.add(f.getFileNameWithoutExtension());
        presetSelector.addItem(f.getFileNameWithoutExtension(), itemId++);
    }

    presetSelector.addSeparator();

    // 3. Actions
    presetSelector.addItem("Save Current Preset...", itemId++);
    presetSelector.addItem("Open Presets Folder...", itemId++);
}

void AnalogFxAudioProcessorEditor::saveUserPreset()
{
    juce::AlertWindow* alert = new juce::AlertWindow("Save Preset", "Enter a name for your preset:", juce::AlertWindow::NoIcon);
    alert->addTextEditor("Preset Name", "My Preset");
    alert->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alert->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    alert->enterModalState(true, juce::ModalCallbackFunction::create([this, alert](int result) {
        if (result == 1) {
            auto name = alert->getTextEditorContents("Preset Name");
            if (name.isNotEmpty()) {
                auto file = presetDirectory.getChildFile(name + ".xml");
                auto state = audioProcessor.apvts.copyState();
                std::unique_ptr<juce::XmlElement> xml(state.createXml());
                if (xml != nullptr) {
                    xml->writeTo(file);
                    loadUserPresets();
                }
            }
        }
        // Since we pass true as the first argument, the AlertWindow deletes itself.
    }));
}
