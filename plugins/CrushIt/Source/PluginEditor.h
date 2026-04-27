#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class CrushItAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    CrushItAudioProcessorEditor(CrushItAudioProcessor&);
    ~CrushItAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    CrushItAudioProcessor& audioProcessor;

    // ═══════════════════════════════════════
    // 1. PARAMETER RELAYS (destroyed last)
    // ═══════════════════════════════════════
    juce::WebSliderRelay bypassRelay { "bypass" };
    juce::WebSliderRelay outputRelay { "output" };

    // M1: Temporal Warp
    juce::WebSliderRelay twEnableRelay { "tw_enable" };
    juce::WebSliderRelay twBufferMsRelay { "tw_buffer_ms" };
    juce::WebSliderRelay twDetectModeRelay { "tw_detect_mode" };
    juce::WebSliderRelay twSensitivityRelay { "tw_sensitivity" };
    juce::WebSliderRelay twJumpMsRelay { "tw_jump_ms" };
    juce::WebSliderRelay twLoopMsRelay { "tw_loop_ms" };
    juce::WebSliderRelay twFeedbackRelay { "tw_feedback" };
    juce::WebSliderRelay twDirectionRelay { "tw_direction" };
    juce::WebSliderRelay twMixRelay { "tw_mix" };

    // M2: Bit Crusher
    juce::WebSliderRelay bcEnableRelay { "bc_enable" };
    juce::WebSliderRelay bcBitsRelay { "bc_bits" };
    juce::WebSliderRelay bcMixRelay { "bc_mix" };

    // M3: Sample Rate
    juce::WebSliderRelay srEnableRelay { "sr_enable" };
    juce::WebSliderRelay srDivisorRelay { "sr_divisor" };
    juce::WebSliderRelay srModeRelay { "sr_mode" };
    juce::WebSliderRelay srMixRelay { "sr_mix" };

    // M4: Clipper
    juce::WebSliderRelay wcEnableRelay { "wc_enable" };
    juce::WebSliderRelay wcDriveRelay { "wc_drive" };
    juce::WebSliderRelay wcThresholdRelay { "wc_threshold" };
    juce::WebSliderRelay wcModeRelay { "wc_mode" };
    juce::WebSliderRelay wcMixRelay { "wc_mix" };

    // M5: Digital Debris
    juce::WebSliderRelay dnEnableRelay { "dn_enable" };
    juce::WebSliderRelay dnTypeRelay { "dn_type" };
    juce::WebSliderRelay dnLevelRelay { "dn_level" };
    juce::WebSliderRelay dnTriggerRelay { "dn_trigger" };
    juce::WebSliderRelay dnSyncRateRelay { "dn_sync_rate" };
    juce::WebSliderRelay dnFreeRateRelay { "dn_free_rate" };
    juce::WebSliderRelay dnDecayMsRelay { "dn_decay_ms" };
    juce::WebSliderRelay dnMixRelay { "dn_mix" };

    // M6: Bit Flipping
    juce::WebSliderRelay bfEnableRelay { "bf_enable" };
    juce::WebSliderRelay bfModeRelay { "bf_mode" };
    juce::WebSliderRelay bfTargetBitRelay { "bf_target_bit" };
    juce::WebSliderRelay bfProbabilityRelay { "bf_probability" };
    juce::WebSliderRelay bfMultiplierRelay { "bf_multiplier" };
    juce::WebSliderRelay bfClipDbRelay { "bf_clip_db" };
    juce::WebSliderRelay bfBitsAffectedRelay { "bf_bits_affected" };
    juce::WebSliderRelay bfMixRelay { "bf_mix" };
    juce::WebSliderRelay randRateRelay { "rand_rate" };

    // 2. WEBVIEW (destroyed middle)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3. ATTACHMENTS (destroyed first)
    std::unique_ptr<juce::WebSliderParameterAttachment> bypassAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> outputAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twBufferMsAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twDetectModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twSensitivityAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twJumpMsAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twLoopMsAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twFeedbackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twDirectionAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> twMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bcEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bcBitsAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bcMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> srEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> srDivisorAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> srModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> srMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> wcEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> wcDriveAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> wcThresholdAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> wcModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> wcMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnTypeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnLevelAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnTriggerAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnSyncRateAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnFreeRateAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnDecayMsAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> dnMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfEnableAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfModeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfTargetBitAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfProbabilityAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfMultiplierAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfClipDbAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfBitsAffectedAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> bfMixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> randRateAttachment;

    // Resource provider
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrushItAudioProcessorEditor)
};
