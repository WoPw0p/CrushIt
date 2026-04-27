#include "PluginProcessor.h"
#include "PluginEditor.h"

// ═══════════════════════════════════════════════════
// Parameter Layout (35 parameters)
// ═══════════════════════════════════════════════════
juce::AudioProcessorValueTreeState::ParameterLayout
CrushItAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto addParam = [&](const juce::String& id, const juce::String& name,
                        float min, float max, float def,
                        bool isBool = false) {
        if (isBool) {
            layout.add(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID{id, 1}, name, def > 0.5f));
        } else {
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{id, 1}, name,
                juce::NormalisableRange<float>(min, max), def));
        }
    };

    // Global
    addParam(ParamIDs::bypass, "Bypass", 0, 1, 0, true);
    addParam(ParamIDs::output, "Output", -24, 12, 0);

    // M1: Temporal Warp
    addParam(ParamIDs::tw_enable,      "TW Enable",      0, 1, 0, true);
    addParam(ParamIDs::tw_buffer_ms,   "TW Buffer",     50, 2000, 500);
    addParam(ParamIDs::tw_detect_mode, "TW Detect Mode", 0, 2, 0);
    addParam(ParamIDs::tw_sensitivity, "TW Sensitivity", 0, 1, 0.6f);
    addParam(ParamIDs::tw_jump_ms,     "TW Jump",        0.5f, 100, 10);
    addParam(ParamIDs::tw_loop_ms,     "TW Loop",        0.5f, 50, 5);
    addParam(ParamIDs::tw_feedback,    "TW Feedback",    0, 0.95f, 0.7f);
    addParam(ParamIDs::tw_direction,   "TW Direction",   0, 1, 0);
    addParam(ParamIDs::tw_mix,         "TW Mix",         0, 1, 0.5f);

    // M2: Bit Crusher
    addParam(ParamIDs::bc_enable, "BC Enable", 0, 1, 0, true);
    addParam(ParamIDs::bc_bits,   "BC Bits",   1, 32, 12);
    addParam(ParamIDs::bc_mix,    "BC Mix",    0, 1, 0.5f);

    // M3: Sample Rate Decimator
    addParam(ParamIDs::sr_enable,  "SR Enable",  0, 1, 0, true);
    addParam(ParamIDs::sr_divisor, "SR Divisor", 1, 256, 4);
    addParam(ParamIDs::sr_mode,    "SR Mode",    0, 1, 0);
    addParam(ParamIDs::sr_mix,     "SR Mix",     0, 1, 0.5f);

    // M4: Waveform Clipper
    addParam(ParamIDs::wc_enable,    "WC Enable",    0, 1, 0, true);
    addParam(ParamIDs::wc_drive,     "WC Drive",    -12, 36, 0);
    addParam(ParamIDs::wc_threshold, "WC Threshold", -36, 0, -6);
    addParam(ParamIDs::wc_mode,      "WC Mode",      0, 2, 0);
    addParam(ParamIDs::wc_mix,       "WC Mix",       0, 1, 0.5f);

    // M5: Digital Debris
    addParam(ParamIDs::dn_enable,    "DN Enable",    0, 1, 0, true);
    addParam(ParamIDs::dn_type,      "DN Type",      0, 2, 0);
    addParam(ParamIDs::dn_level,     "DN Level",     0, 1, 0.3f);
    addParam(ParamIDs::dn_trigger,   "DN Trigger",   0, 1, 0);
    addParam(ParamIDs::dn_sync_rate, "DN Sync Rate", 0.0625f, 4, 0.25f);
    addParam(ParamIDs::dn_free_rate, "DN Free Rate", 0.5f, 100, 8);
    addParam(ParamIDs::dn_decay_ms,  "DN Decay",     1, 200, 20);
    addParam(ParamIDs::dn_mix,       "DN Mix",       0, 1, 0.3f);

    // M6: Bit Flipping
    addParam(ParamIDs::bf_enable,        "BF Enable",        0, 1, 0, true);
    addParam(ParamIDs::bf_mode,          "BF Mode",          0, 2, 0);
    addParam(ParamIDs::bf_target_bit,    "BF Target Bit",    0, 31, 22);
    addParam(ParamIDs::bf_probability,   "BF Probability",   0, 1, 0.05f);
    addParam(ParamIDs::bf_multiplier,    "BF Multiplier",    1, 1000, 100);
    addParam(ParamIDs::bf_clip_db,       "BF Clip dB",      -24, 0, -3);
    addParam(ParamIDs::bf_bits_affected, "BF Bits Affected", 1, 8, 1);
    addParam(ParamIDs::bf_mix,           "BF Mix",           0, 1, 0.5f);

    // Global: Randomization rate (0=off, 1-11=beat divisions)
    addParam(ParamIDs::rand_rate, "Rand Rate", 0, 11, 0);

    return layout;
}

// ═══════════════════════════════════════════════════
CrushItAudioProcessor::CrushItAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "STATE", createParameterLayout())
{
}

CrushItAudioProcessor::~CrushItAudioProcessor() = default;

// ═══════════════════════════════════════════════════
const juce::String CrushItAudioProcessor::getName() const { return "Crush It"; }
bool CrushItAudioProcessor::acceptsMidi() const { return false; }
bool CrushItAudioProcessor::producesMidi() const { return false; }
bool CrushItAudioProcessor::isMidiEffect() const { return false; }
double CrushItAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int CrushItAudioProcessor::getNumPrograms() { return 1; }
int CrushItAudioProcessor::getCurrentProgram() { return 0; }
void CrushItAudioProcessor::setCurrentProgram(int) {}
const juce::String CrushItAudioProcessor::getProgramName(int) { return "Default"; }
void CrushItAudioProcessor::changeProgramName(int, const juce::String&) {}

// ═══════════════════════════════════════════════════
void CrushItAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;

    // Init smoothing
    const double smoothTime = 0.01; // 10ms
    smTwMix.reset(sr, smoothTime); smBcMix.reset(sr, smoothTime);
    smSrMix.reset(sr, smoothTime); smWcMix.reset(sr, smoothTime);
    smDnMix.reset(sr, smoothTime); smBfMix.reset(sr, smoothTime);
    smOutputGain.reset(sr, smoothTime);
    smTwMix.setCurrentAndTargetValue(0.5f); smBcMix.setCurrentAndTargetValue(0.5f);
    smSrMix.setCurrentAndTargetValue(0.5f); smWcMix.setCurrentAndTargetValue(0.5f);
    smDnMix.setCurrentAndTargetValue(0.5f); smBfMix.setCurrentAndTargetValue(0.5f);
    smOutputGain.setCurrentAndTargetValue(1.0f);

    // M1: Allocate warp buffer (max 2000ms = 2s stereo)
    warpBufferSize = static_cast<int>(sr * 2.0);
    warpBuffer.setSize(2, warpBufferSize);
    warpBuffer.clear();
    warpWritePos = 0;
    twStateL = TWState::NORMAL; twStateR = TWState::NORMAL;
    twReadPosL = 0; twReadPosR = 0;
    twEnvL = 0.0f; twEnvR = 0.0f;
    twPeakHoldL = 0.0f; twPeakHoldR = 0.0f;
    twCooldownL = 0; twCooldownR = 0;
    twZcrCountL = 0; twZcrCountR = 0;
    twZcrRefL = 0; twZcrRefR = 0;
    twZcrBlockCounter = 0;
    twReleaseTimerL = 0; twReleaseTimerR = 0;

    // M3: Reset decimator state
    srLastSampleL = 0.0f; srLastSampleR = 0.0f;
    srCounter = 0;

    // M5: Seed RNG
    debrisRng = juce::Random(0x43525553484954); // "CRUSHIT"
}

void CrushItAudioProcessor::releaseResources()
{
}

bool CrushItAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

// ═══════════════════════════════════════════════════
// SIGNAL CHAIN:
// Input → [M1:TemporalWarp] → [M2:BitCrusher] → [M3:SRDecimator]
//       → [M4:Clipper] → [M5:DigitalDebris] → [M6:BitFlipping] → Output
// ═══════════════════════════════════════════════════
void CrushItAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Measure input peak
    {
        float maxIn = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
            auto r = buffer.findMinMax(ch, 0, buffer.getNumSamples());
            maxIn = std::max({maxIn, std::abs(r.getStart()), std::abs(r.getEnd())});
        }
        inputPeakLevel.store(maxIn);
    }

    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0) return;

    const int numChannels = buffer.getNumChannels();
    const auto totalOutChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = numChannels; i < totalOutChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Read all parameters
    const bool bypassOn = apvts.getRawParameterValue(ParamIDs::bypass)->load() > 0.5f;
    const float outputDb = apvts.getRawParameterValue(ParamIDs::output)->load();

    const bool twOn = apvts.getRawParameterValue(ParamIDs::tw_enable)->load() > 0.5f;
    const float twBufferMs = apvts.getRawParameterValue(ParamIDs::tw_buffer_ms)->load();
    const int twDetectMode = static_cast<int>(apvts.getRawParameterValue(ParamIDs::tw_detect_mode)->load());
    const float twSensitivity = apvts.getRawParameterValue(ParamIDs::tw_sensitivity)->load();
    const float twJumpMs = apvts.getRawParameterValue(ParamIDs::tw_jump_ms)->load();
    const float twLoopMs = apvts.getRawParameterValue(ParamIDs::tw_loop_ms)->load();
    const float twFeedback = apvts.getRawParameterValue(ParamIDs::tw_feedback)->load();
    const int twDirection = static_cast<int>(apvts.getRawParameterValue(ParamIDs::tw_direction)->load());
    const float twMix = apvts.getRawParameterValue(ParamIDs::tw_mix)->load();

    const bool bcOn = apvts.getRawParameterValue(ParamIDs::bc_enable)->load() > 0.5f;
    const float bcBits = apvts.getRawParameterValue(ParamIDs::bc_bits)->load();
    const float bcMix = apvts.getRawParameterValue(ParamIDs::bc_mix)->load();

    const bool srOn = apvts.getRawParameterValue(ParamIDs::sr_enable)->load() > 0.5f;
    const float srDivisor = apvts.getRawParameterValue(ParamIDs::sr_divisor)->load();
    const int srMode = static_cast<int>(apvts.getRawParameterValue(ParamIDs::sr_mode)->load());
    const float srMix = apvts.getRawParameterValue(ParamIDs::sr_mix)->load();

    const bool wcOn = apvts.getRawParameterValue(ParamIDs::wc_enable)->load() > 0.5f;
    const float wcDrive = apvts.getRawParameterValue(ParamIDs::wc_drive)->load();
    const float wcThreshold = apvts.getRawParameterValue(ParamIDs::wc_threshold)->load();
    const int wcMode = static_cast<int>(apvts.getRawParameterValue(ParamIDs::wc_mode)->load());
    const float wcMix = apvts.getRawParameterValue(ParamIDs::wc_mix)->load();

    const bool dnOn = apvts.getRawParameterValue(ParamIDs::dn_enable)->load() > 0.5f;
    const int dnType = static_cast<int>(apvts.getRawParameterValue(ParamIDs::dn_type)->load());
    const float dnLevel = apvts.getRawParameterValue(ParamIDs::dn_level)->load();
    const int dnTrigger = static_cast<int>(apvts.getRawParameterValue(ParamIDs::dn_trigger)->load());
    const float dnSyncRate = apvts.getRawParameterValue(ParamIDs::dn_sync_rate)->load();
    const float dnFreeRate = apvts.getRawParameterValue(ParamIDs::dn_free_rate)->load();
    const float dnDecayMs = apvts.getRawParameterValue(ParamIDs::dn_decay_ms)->load();
    const float dnMix = apvts.getRawParameterValue(ParamIDs::dn_mix)->load();

    const bool bfOn = apvts.getRawParameterValue(ParamIDs::bf_enable)->load() > 0.5f;
    const int bfMode = static_cast<int>(apvts.getRawParameterValue(ParamIDs::bf_mode)->load());
    const float bfTargetBit = apvts.getRawParameterValue(ParamIDs::bf_target_bit)->load();
    const float bfProbability = apvts.getRawParameterValue(ParamIDs::bf_probability)->load();
    const float bfMultiplier = apvts.getRawParameterValue(ParamIDs::bf_multiplier)->load();
    const float bfClipDb = apvts.getRawParameterValue(ParamIDs::bf_clip_db)->load();
    const int bfBitsAffected = static_cast<int>(apvts.getRawParameterValue(ParamIDs::bf_bits_affected)->load());
    const float bfMix = apvts.getRawParameterValue(ParamIDs::bf_mix)->load();

    // Set smoothing targets
    smTwMix.setTargetValue(twMix); smBcMix.setTargetValue(bcMix);
    smSrMix.setTargetValue(srMix); smWcMix.setTargetValue(wcMix);
    smDnMix.setTargetValue(dnMix); smBfMix.setTargetValue(bfMix);
    smOutputGain.setTargetValue(db2gain(outputDb));

    // Quick pass-through if bypassed
    if (bypassOn) {
        // Apply output gain only
        for (int ch = 0; ch < numChannels; ++ch) {
            auto* data = buffer.getWritePointer(ch);
            for (int s = 0; s < numSamples; ++s) {
                data[s] *= smOutputGain.getNextValue();
            }
        }
        // Meter peaks
        float maxIn = 0.0f, maxOut = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch) {
            auto r = buffer.findMinMax(ch, 0, numSamples);
            maxIn = std::max({maxIn, std::abs(r.getStart()), std::abs(r.getEnd())});
            maxOut = std::max(maxOut, maxIn);
        }
        inputPeakLevel.store(maxIn); outputPeakLevel.store(maxOut);
        return;
    }

    // Process per-sample through the chain
    for (int s = 0; s < numSamples; ++s)
    {
        float curSmTw = smTwMix.getNextValue();
        float curSmBc = smBcMix.getNextValue();
        float curSmSr = smSrMix.getNextValue();
        float curSmWc = smWcMix.getNextValue();
        float curSmDn = smDnMix.getNextValue();
        float curSmBf = smBfMix.getNextValue();
        float curGain = smOutputGain.getNextValue();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float dry = buffer.getSample(ch, s);
            float wet = dry;

            // ── M1: TEMPORAL WARP ──
            if (twOn) {
                float warpDry = wet;

                // Write to ring buffer (shared write pos across channels)
                int wIdx = warpWritePos;
                warpBuffer.setSample(ch, wIdx, wet);

                // Per-channel state references
                auto& twState = (ch == 0) ? twStateL : twStateR;
                auto& twReadPos = (ch == 0) ? twReadPosL : twReadPosR;
                auto& twLoopStart = (ch == 0) ? twLoopStartL : twLoopStartR;
                auto& twLoopPos = (ch == 0) ? twLoopPosL : twLoopPosR;
                auto& twEnv = (ch == 0) ? twEnvL : twEnvR;
                auto& twPeak = (ch == 0) ? twPeakHoldL : twPeakHoldR;
                auto& twCooldown = (ch == 0) ? twCooldownL : twCooldownR;
                auto& twLastSamp = (ch == 0) ? twLastSampleL : twLastSampleR;
                auto& twZcr = (ch == 0) ? twZcrCountL : twZcrCountR;
                auto& twZcrRef = (ch == 0) ? twZcrRefL : twZcrRefR;
                auto& twRelTimer = (ch == 0) ? twReleaseTimerL : twReleaseTimerR;

                // Envelope follower (fast attack, slow release)
                float absIn = std::abs(wet);
                float envAttack = (absIn > twEnv) ? 0.995f : 0.9995f; // ~1ms attack, ~50ms release @ 48kHz
                twEnv = twEnv * envAttack + absIn * (1.0f - envAttack);

                // Peak hold with decay
                if (absIn > twPeak) twPeak = absIn;
                else twPeak *= 0.9998f;

                // ZCR tracking
                if ((wet >= 0.0f) != (twLastSamp >= 0.0f)) twZcr++;
                twLastSamp = wet;
                twZcrBlockCounter++;
                if (twZcrBlockCounter >= twZcrBlockSize) {
                    twZcrBlockCounter = 0;
                    twZcrRef = twZcr;
                    twZcr = 0;
                }

                // Transient/pitch detection
                bool detected = false;
                if (twCooldown > 0) twCooldown--;

                if (twCooldown <= 0) {
                    if (twDetectMode == 0) {
                        // Transient detection: envelope exceeds sensitivity * peak
                        float thresh = twSensitivity * twPeak * 1.5f + 0.01f;
                        if (twEnv > thresh && absIn > thresh) detected = true;
                    } else if (twDetectMode == 1) {
                        // Pitch change: ZCR delta exceeds threshold
                        int zcrDelta = std::abs(twZcr - twZcrRef);
                        if (zcrDelta > static_cast<int>(twSensitivity * 30.0f)) detected = true;
                    } else {
                        // Free mode: random periodic triggers
                        if (debrisRng.nextFloat() < twSensitivity * 0.001f) detected = true;
                    }
                }

                // State machine
                int jumpSamples = static_cast<int>(twJumpMs * sampleRate * 0.001f);
                twLoopLengthSamples = static_cast<int>(twLoopMs * sampleRate * 0.001f);
                if (twLoopLengthSamples < 4) twLoopLengthSamples = 4;

                if (detected && twState == TWState::NORMAL) {
                    twState = TWState::JUMPING;
                    int jumpPos = (wIdx - jumpSamples + warpBufferSize) % warpBufferSize;
                    twReadPos = jumpPos;
                    twLoopStart = twReadPos;
                    twLoopPos = 0;
                    twCooldown = static_cast<int>(sampleRate * 0.01); // 10ms cooldown
                }

                float warpWet = 0.0f;

                if (twState == TWState::NORMAL) {
                    // Smooth follow: read a bit behind write head
                    int followDist = 128; // ~2.7ms @ 48kHz
                    twReadPos = (twReadPos + 1) % warpBufferSize;
                    int targetPos = (wIdx - followDist + warpBufferSize) % warpBufferSize;
                    // Gradual catch-up
                    if (twReadPos < targetPos) twReadPos = std::min(targetPos, twReadPos + 2);
                    else if (twReadPos > targetPos) twReadPos = (twReadPos + 1) % warpBufferSize;
                    warpWet = warpBuffer.getSample(ch, twReadPos);

                } else if (twState == TWState::JUMPING) {
                    // Short jump phase: read from jumped position
                    twReadPos = (twReadPos + 1) % warpBufferSize;
                    warpWet = warpBuffer.getSample(ch, twReadPos);
                    twLoopPos++;
                    // Enter looping after reaching loop start
                    if (twLoopPos >= 4) {
                        twState = TWState::LOOPING;
                        twLoopPos = 0;
                    }

                } else if (twState == TWState::LOOPING) {
                    // Micro-loop: read within loop segment
                    int loopEnd = (twLoopStart + twLoopLengthSamples) % warpBufferSize;
                    int readIdx;
                    if (twDirection == 0 || (twLoopPos / twLoopLengthSamples) % 2 == 0) {
                        // Forward
                        readIdx = (twLoopStart + (twLoopPos % twLoopLengthSamples)) % warpBufferSize;
                    } else {
                        // Alternating (reverse)
                        readIdx = (loopEnd - 1 - (twLoopPos % twLoopLengthSamples) + warpBufferSize) % warpBufferSize;
                    }
                    warpWet = warpBuffer.getSample(ch, readIdx);

                    // Feedback: mix loop output back into buffer at loop start
                    float fbSample = warpBuffer.getSample(ch, readIdx) * twFeedback;
                    int fbIdx = (twLoopStart + (twLoopPos % twLoopLengthSamples)) % warpBufferSize;
                    float existing = warpBuffer.getSample(ch, fbIdx);
                    warpBuffer.setSample(ch, fbIdx, existing + fbSample * 0.5f);

                    twLoopPos++;
                    twReadPos = readIdx;

                    // Release after a few loop cycles (controlled by sensitivity)
                    int maxLoopCycles = 3 + static_cast<int>(twSensitivity * 10.0f);
                    if (twLoopPos >= twLoopLengthSamples * maxLoopCycles) {
                        twState = TWState::RELEASING;
                        twRelTimer = twReleaseSamples;
                    }

                } else if (twState == TWState::RELEASING) {
                    // Crossfade from loop back to normal
                    int readIdx = (twLoopStart + (twLoopPos % twLoopLengthSamples)) % warpBufferSize;
                    float loopWet = warpBuffer.getSample(ch, readIdx);
                    // Normal position catch-up
                    int followDist = 128;
                    int normalPos = (wIdx - followDist + warpBufferSize) % warpBufferSize;
                    float normalWet = warpBuffer.getSample(ch, normalPos);
                    float fade = static_cast<float>(twRelTimer) / twReleaseSamples;
                    warpWet = loopWet * fade + normalWet * (1.0f - fade);
                    twRelTimer--;
                    twLoopPos++;
                    if (twRelTimer <= 0) {
                        twState = TWState::NORMAL;
                        twReadPos = normalPos;
                    }
                }

                wet = warpDry + curSmTw * (warpWet - warpDry);
            } else {
                // Still write to buffer even when bypassed (keep it filled)
                int wIdx = warpWritePos;
                warpBuffer.setSample(ch, wIdx, wet);
            }

            // ── M2: BIT CRUSHER ──
            if (bcOn) {
                float bcDry = wet;
                int bits = juce::jlimit(1, 32, static_cast<int>(bcBits));
                float levels = static_cast<float>(1 << bits);
                float step = 2.0f / (levels - 1.0f);
                float crushed = std::round(wet / step) * step;
                wet = bcDry + curSmBc * (crushed - bcDry);
            }

            // ── M3: SAMPLE RATE DECIMATOR ──
            if (srOn) {
                float srDry = wet;
                int div = juce::jlimit(1, 256, static_cast<int>(srDivisor));
                float& lastSample = (ch == 0) ? srLastSampleL : srLastSampleR;

                if (srCounter % div == 0) {
                    lastSample = wet;
                }

                float decimated;
                if (srMode == 0) {
                    // Zero-Order Hold
                    decimated = lastSample;
                } else {
                    // Linear interpolation
                    float frac = static_cast<float>(srCounter % div) / static_cast<float>(div);
                    float nextSample = wet;
                    decimated = lastSample + frac * (nextSample - lastSample);
                }

                wet = srDry + curSmSr * (decimated - srDry);
            } else {
                if (ch == 0) srLastSampleL = wet;
                else srLastSampleR = wet;
            }

            // ── M4: WAVEFORM CLIPPER ──
            if (wcOn) {
                float wcDry = wet;
                float drive = db2gain(wcDrive);
                float thresh = db2gain(wcThreshold);
                float driven = wet * drive;

                float clipped;
                if (wcMode == 0) {
                    // Hard clip
                    clipped = juce::jlimit(-thresh, thresh, driven);
                } else if (wcMode == 1) {
                    // Soft clip (tanh)
                    clipped = std::tanh(driven / thresh) * thresh;
                } else {
                    // Wavefold
                    float a = std::abs(driven + thresh);
                    float b = std::fmod(a, 4.0f * thresh);
                    clipped = thresh - std::abs(b - 2.0f * thresh);
                }

                wet = wcDry + curSmWc * (clipped - wcDry);
            }

            // ── M5: DIGITAL DEBRIS ──
            if (dnOn) {
                float dnDry = wet;
                float& dnEnv = (ch == 0) ? dnEnvL : dnEnvR;

                // Trigger logic
                bool triggerNow = false;
                if (dnTrigger == 0) {
                    // Sync mode: use internal clock (120 BPM default)
                    float beatHz = dnSyncRate * 2.0f; // 1/4 note = 1Hz @ 120BPM, convert
                    dnTriggerInterval = static_cast<int>(sampleRate / beatHz);
                    if (dnTriggerInterval < 4) dnTriggerInterval = 4;
                    dnTriggerCounter--;
                    if (dnTriggerCounter <= 0) {
                        triggerNow = true;
                        dnTriggerCounter = dnTriggerInterval;
                    }
                } else {
                    // Free mode
                    dnTriggerInterval = static_cast<int>(sampleRate / dnFreeRate);
                    if (dnTriggerInterval < 2) dnTriggerInterval = 2;
                    dnTriggerCounter--;
                    if (dnTriggerCounter <= 0) {
                        triggerNow = true;
                        dnTriggerCounter = dnTriggerInterval;
                    }
                }

                // Envelope
                float decayCoef = std::exp(-1.0f / (sampleRate * dnDecayMs * 0.001f + 0.1f));
                if (triggerNow) dnEnv = 1.0f;
                else dnEnv *= decayCoef;

                // Generate noise based on type
                float noise = 0.0f;
                float levelScale = dnLevel * dnEnv;

                if (dnType == 0) {
                    // White: continuous noise
                    noise = (debrisRng.nextFloat() * 2.0f - 1.0f) * levelScale;
                } else if (dnType == 1) {
                    // Burst: enveloped noise (only during trigger)
                    noise = (debrisRng.nextFloat() * 2.0f - 1.0f) * levelScale;
                } else {
                    // Crackle: sparse impulses
                    if (debrisRng.nextFloat() < levelScale * 0.3f) {
                        noise = (debrisRng.nextFloat() * 2.0f - 1.0f) * levelScale * 2.0f;
                    } else if (triggerNow) {
                        noise = (debrisRng.nextFloat() * 2.0f - 1.0f) * levelScale;
                    }
                }

                wet = dnDry + curSmDn * noise;
            } else {
                dnEnvL = 0.0f; dnEnvR = 0.0f;
                dnTriggerCounter = 0;
            }

            // ── M6: BIT FLIPPING (IEEE 754 error injection) ──
            if (bfOn) {
                float bfDry = wet;

                // Per-sample probability gate
                if (debrisRng.nextFloat() < bfProbability) {
                    union { float f; uint32_t i; } bits;
                    bits.f = wet;

                    int tBit = juce::jlimit(0, 31, static_cast<int>(bfTargetBit));
                    int numBits = juce::jlimit(1, 8, bfBitsAffected);

                    if (bfMode == 0) {
                        // Invert: XOR specific bits
                        uint32_t mask = 0;
                        for (int b = 0; b < numBits; ++b) {
                            int bitPos = (tBit + b) & 31; // Wrap within 32 bits
                            mask |= (1u << bitPos);
                        }
                        bits.i ^= mask;
                    } else if (bfMode == 1) {
                        // Random: XOR with random bit pattern in range
                        uint32_t mask = 0;
                        for (int b = 0; b < numBits; ++b) {
                            int bitPos = (tBit + b) & 31;
                            if (debrisRng.nextBool())
                                mask |= (1u << bitPos);
                        }
                        bits.i ^= mask;
                    } else {
                        // Multiply: huge gain then clip
                        bits.f *= bfMultiplier;
                    }

                    float result = bits.f;

                    // NaN/Inf safety — flush to zero
                    if (!std::isfinite(result)) result = 0.0f;

                    // Post-injection hard clipper
                    float clipCeiling = db2gain(bfClipDb);
                    result = juce::jlimit(-clipCeiling, clipCeiling, result);

                    wet = bfDry + curSmBf * (result - bfDry);
                }
            }

            // Apply output gain
            wet *= curGain;

            // NaN safety
            if (!std::isfinite(wet)) wet = 0.0f;

            buffer.setSample(ch, s, wet);
        }
        // Advance shared write pointer after both channels write this sample
        if (twOn) warpWritePos = (warpWritePos + 1) % warpBufferSize;
    }

    // Update SR counter
    if (srOn) {
        srCounter = (srCounter + numSamples);
    }

    // Meter output peak
    {
        float maxOut = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch) {
            auto r = buffer.findMinMax(ch, 0, numSamples);
            maxOut = std::max({maxOut, std::abs(r.getStart()), std::abs(r.getEnd())});
        }
        outputPeakLevel.store(maxOut);
    }
}

// ═══════════════════════════════════════════════════
// State save/load
// ═══════════════════════════════════════════════════
void CrushItAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CrushItAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// ═══════════════════════════════════════════════════
// Editor
// ═══════════════════════════════════════════════════
juce::AudioProcessorEditor* CrushItAudioProcessor::createEditor()
{
    return new CrushItAudioProcessorEditor(*this);
}

bool CrushItAudioProcessor::hasEditor() const { return true; }

// ═══════════════════════════════════════════════════
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrushItAudioProcessor();
}
