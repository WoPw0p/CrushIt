#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <atomic>

// ═══════════════════════════════════════════════════
// Parameter ID constants
// ═══════════════════════════════════════════════════
namespace ParamIDs {
    const juce::String bypass   = "bypass";
    const juce::String output   = "output";
    // M1: Temporal Warp
    const juce::String tw_enable      = "tw_enable";
    const juce::String tw_buffer_ms   = "tw_buffer_ms";
    const juce::String tw_detect_mode = "tw_detect_mode";
    const juce::String tw_sensitivity = "tw_sensitivity";
    const juce::String tw_jump_ms     = "tw_jump_ms";
    const juce::String tw_loop_ms     = "tw_loop_ms";
    const juce::String tw_feedback    = "tw_feedback";
    const juce::String tw_direction   = "tw_direction";
    const juce::String tw_mix         = "tw_mix";
    // M2: Bit Crusher
    const juce::String bc_enable = "bc_enable";
    const juce::String bc_bits   = "bc_bits";
    const juce::String bc_mix    = "bc_mix";
    // M3: Sample Rate Decimator
    const juce::String sr_enable  = "sr_enable";
    const juce::String sr_divisor = "sr_divisor";
    const juce::String sr_mode    = "sr_mode";
    const juce::String sr_mix     = "sr_mix";
    // M4: Waveform Clipper
    const juce::String wc_enable    = "wc_enable";
    const juce::String wc_drive     = "wc_drive";
    const juce::String wc_threshold = "wc_threshold";
    const juce::String wc_mode      = "wc_mode";
    const juce::String wc_mix       = "wc_mix";
    // M5: Digital Debris
    const juce::String dn_enable    = "dn_enable";
    const juce::String dn_type      = "dn_type";
    const juce::String dn_level     = "dn_level";
    const juce::String dn_trigger   = "dn_trigger";
    const juce::String dn_sync_rate = "dn_sync_rate";
    const juce::String dn_free_rate = "dn_free_rate";
    const juce::String dn_decay_ms  = "dn_decay_ms";
    const juce::String dn_mix       = "dn_mix";
    // M6: Bit Flipping
    const juce::String bf_enable        = "bf_enable";
    const juce::String bf_mode          = "bf_mode";
    const juce::String bf_target_bit    = "bf_target_bit";
    const juce::String bf_probability   = "bf_probability";
    const juce::String bf_multiplier    = "bf_multiplier";
    const juce::String bf_clip_db       = "bf_clip_db";
    const juce::String bf_bits_affected = "bf_bits_affected";
    const juce::String bf_mix           = "bf_mix";
    // Global
    const juce::String rand_rate       = "rand_rate";
}

class CrushItAudioProcessor : public juce::AudioProcessor
{
public:
    CrushItAudioProcessor();
    ~CrushItAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
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

    // Metering
    std::atomic<float> inputPeakLevel { 0.0f };
    std::atomic<float> outputPeakLevel { 0.0f };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ═══════════════════════════════════
    // DSP State
    // ═══════════════════════════════════
    double sampleRate = 44100.0;
    int blockSize = 512;

    // M1: Temporal Warp
    enum class TWState { NORMAL, JUMPING, LOOPING, RELEASING };
    TWState twStateL = TWState::NORMAL, twStateR = TWState::NORMAL;
    juce::AudioBuffer<float> warpBuffer;
    int warpWritePos = 0;
    int warpBufferSize = 0;
    int twReadPosL = 0, twReadPosR = 0;        // Read head positions (per channel)
    int twLoopStartL = 0, twLoopStartR = 0;     // Loop start points
    int twLoopPosL = 0, twLoopPosR = 0;         // Position within loop (samples)
    int twLoopLengthSamples = 0;                // Loop length in samples
    int twReleaseTimerL = 0, twReleaseTimerR = 0; // Crossfade release count
    static constexpr int twReleaseSamples = 64;   // 64-sample crossfade
    // Envelope follower for transient detection
    float twEnvL = 0.0f, twEnvR = 0.0f;
    float twPeakHoldL = 0.0f, twPeakHoldR = 0.0f;
    int twCooldownL = 0, twCooldownR = 0;       // Prevent re-trigger cooldown
    // ZCR for pitch detection
    float twLastSampleL = 0.0f, twLastSampleR = 0.0f;
    int twZcrCountL = 0, twZcrCountR = 0;
    int twZcrRefL = 0, twZcrRefR = 0;           // Reference ZCR for delta detection
    int twZcrBlockCounter = 0;
    static constexpr int twZcrBlockSize = 256;   // ZCR measurement window

    // M2: Bit Crusher state
    // (stateless — computed per sample)

    // M3: SR Decimator state
    float srLastSampleL = 0.0f, srLastSampleR = 0.0f;
    int srCounter = 0;

    // M4: Clipper — stateless

    // M5: Digital Debris
    juce::Random debrisRng;
    float dnEnvL = 0.0f, dnEnvR = 0.0f;          // Noise envelope
    int dnTriggerCounter = 0;                      // Sample counter for trigger timing
    int dnTriggerInterval = 44100;                  // Samples between triggers

    // M6: Bit Flipping
    // (stateless — computed per sample via union)

    // Per-module smoothing for mix params
    juce::SmoothedValue<float> smTwMix, smBcMix, smSrMix;
    juce::SmoothedValue<float> smWcMix, smDnMix, smBfMix;
    juce::SmoothedValue<float> smOutputGain;

    // Helper: dB to linear gain
    static float db2gain(float db) { return std::pow(10.0f, db / 20.0f); }
    static float gain2db(float g) { return 20.0f * std::log10(g); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrushItAudioProcessor)
};
