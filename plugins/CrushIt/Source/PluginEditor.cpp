#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

CrushItAudioProcessorEditor::CrushItAudioProcessorEditor(CrushItAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    auto options = juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withOptionsFrom(bypassRelay)
        .withOptionsFrom(outputRelay)
        .withOptionsFrom(twEnableRelay)
        .withOptionsFrom(twBufferMsRelay)
        .withOptionsFrom(twDetectModeRelay)
        .withOptionsFrom(twSensitivityRelay)
        .withOptionsFrom(twJumpMsRelay)
        .withOptionsFrom(twLoopMsRelay)
        .withOptionsFrom(twFeedbackRelay)
        .withOptionsFrom(twDirectionRelay)
        .withOptionsFrom(twMixRelay)
        .withOptionsFrom(bcEnableRelay)
        .withOptionsFrom(bcBitsRelay)
        .withOptionsFrom(bcMixRelay)
        .withOptionsFrom(srEnableRelay)
        .withOptionsFrom(srDivisorRelay)
        .withOptionsFrom(srModeRelay)
        .withOptionsFrom(srMixRelay)
        .withOptionsFrom(wcEnableRelay)
        .withOptionsFrom(wcDriveRelay)
        .withOptionsFrom(wcThresholdRelay)
        .withOptionsFrom(wcModeRelay)
        .withOptionsFrom(wcMixRelay)
        .withOptionsFrom(dnEnableRelay)
        .withOptionsFrom(dnTypeRelay)
        .withOptionsFrom(dnLevelRelay)
        .withOptionsFrom(dnTriggerRelay)
        .withOptionsFrom(dnSyncRateRelay)
        .withOptionsFrom(dnFreeRateRelay)
        .withOptionsFrom(dnDecayMsRelay)
        .withOptionsFrom(dnMixRelay)
        .withOptionsFrom(bfEnableRelay)
        .withOptionsFrom(bfModeRelay)
        .withOptionsFrom(bfTargetBitRelay)
        .withOptionsFrom(bfProbabilityRelay)
        .withOptionsFrom(bfMultiplierRelay)
        .withOptionsFrom(bfClipDbRelay)
        .withOptionsFrom(bfBitsAffectedRelay)
        .withOptionsFrom(bfMixRelay)
        .withOptionsFrom(randRateRelay)
        .withResourceProvider([this](const auto& url) {
            return getResource(url);
        });

#if JUCE_WINDOWS
    options = options
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options(
            juce::WebBrowserComponent::Options::WinWebView2{}
                .withUserDataFolder(juce::File::getSpecialLocation(
                    juce::File::SpecialLocationType::tempDirectory)));
#endif

    webView = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(*webView);

    // Attachments with null-safety
    auto& apv = audioProcessor.apvts;
    auto attach = [&](auto& relay, const juce::String& id) {
        auto* param = apv.getParameter(id);
        jassert(param != nullptr);
        return param ? std::make_unique<juce::WebSliderParameterAttachment>(*param, relay) : nullptr;
    };
    bypassAttachment = attach(bypassRelay, "bypass");
    outputAttachment = attach(outputRelay, "output");
    twEnableAttachment = attach(twEnableRelay, "tw_enable");
    twBufferMsAttachment = attach(twBufferMsRelay, "tw_buffer_ms");
    twDetectModeAttachment = attach(twDetectModeRelay, "tw_detect_mode");
    twSensitivityAttachment = attach(twSensitivityRelay, "tw_sensitivity");
    twJumpMsAttachment = attach(twJumpMsRelay, "tw_jump_ms");
    twLoopMsAttachment = attach(twLoopMsRelay, "tw_loop_ms");
    twFeedbackAttachment = attach(twFeedbackRelay, "tw_feedback");
    twDirectionAttachment = attach(twDirectionRelay, "tw_direction");
    twMixAttachment = attach(twMixRelay, "tw_mix");
    bcEnableAttachment = attach(bcEnableRelay, "bc_enable");
    bcBitsAttachment = attach(bcBitsRelay, "bc_bits");
    bcMixAttachment = attach(bcMixRelay, "bc_mix");
    srEnableAttachment = attach(srEnableRelay, "sr_enable");
    srDivisorAttachment = attach(srDivisorRelay, "sr_divisor");
    srModeAttachment = attach(srModeRelay, "sr_mode");
    srMixAttachment = attach(srMixRelay, "sr_mix");
    wcEnableAttachment = attach(wcEnableRelay, "wc_enable");
    wcDriveAttachment = attach(wcDriveRelay, "wc_drive");
    wcThresholdAttachment = attach(wcThresholdRelay, "wc_threshold");
    wcModeAttachment = attach(wcModeRelay, "wc_mode");
    wcMixAttachment = attach(wcMixRelay, "wc_mix");
    dnEnableAttachment = attach(dnEnableRelay, "dn_enable");
    dnTypeAttachment = attach(dnTypeRelay, "dn_type");
    dnLevelAttachment = attach(dnLevelRelay, "dn_level");
    dnTriggerAttachment = attach(dnTriggerRelay, "dn_trigger");
    dnSyncRateAttachment = attach(dnSyncRateRelay, "dn_sync_rate");
    dnFreeRateAttachment = attach(dnFreeRateRelay, "dn_free_rate");
    dnDecayMsAttachment = attach(dnDecayMsRelay, "dn_decay_ms");
    dnMixAttachment = attach(dnMixRelay, "dn_mix");
    bfEnableAttachment = attach(bfEnableRelay, "bf_enable");
    bfModeAttachment = attach(bfModeRelay, "bf_mode");
    bfTargetBitAttachment = attach(bfTargetBitRelay, "bf_target_bit");
    bfProbabilityAttachment = attach(bfProbabilityRelay, "bf_probability");
    bfMultiplierAttachment = attach(bfMultiplierRelay, "bf_multiplier");
    bfClipDbAttachment = attach(bfClipDbRelay, "bf_clip_db");
    bfBitsAffectedAttachment = attach(bfBitsAffectedRelay, "bf_bits_affected");
    bfMixAttachment = attach(bfMixRelay, "bf_mix");
    randRateAttachment = attach(randRateRelay, "rand_rate");

    setSize(800, 500);
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    startTimerHz(30);
}

CrushItAudioProcessorEditor::~CrushItAudioProcessorEditor() = default;

void CrushItAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void CrushItAudioProcessorEditor::resized()
{
    if (webView != nullptr)
        webView->setBounds(getLocalBounds());
}

void CrushItAudioProcessorEditor::timerCallback()
{
    float inPeak = audioProcessor.inputPeakLevel.exchange(0.0f);
    float outPeak = audioProcessor.outputPeakLevel.exchange(0.0f);
    juce::String js = "if(window.updateMeterLevels)window.updateMeterLevels("
        + juce::String(inPeak, 3) + "," + juce::String(outPeak, 3) + ")";
    if (webView) webView->evaluateJavascript(js);
}

// ═══════════════════════════════════════
// Resource Provider
// ═══════════════════════════════════════

static const char* getMimeForExtension(const juce::String& extension)
{
    static const std::unordered_map<juce::String, const char*> mimeMap = {
        {{"htm"}, "text/html"}, {{"html"}, "text/html"}, {{"txt"}, "text/plain"},
        {{"jpg"}, "image/jpeg"}, {{"jpeg"}, "image/jpeg"}, {{"svg"}, "image/svg+xml"},
        {{"ico"}, "image/vnd.microsoft.icon"}, {{"json"}, "application/json"},
        {{"png"}, "image/png"}, {{"css"}, "text/css"}, {{"js"}, "text/javascript"},
        {{"woff2"}, "font/woff2"}
    };
    if (const auto it = mimeMap.find(extension.toLowerCase()); it != mimeMap.end())
        return it->second;
    return "text/plain";
}

static juce::String getExtension(const juce::String& filename)
{
    return filename.fromLastOccurrenceOf(".", false, false);
}

std::optional<juce::WebBrowserComponent::Resource> CrushItAudioProcessorEditor::getResource(const juce::String& url)
{
    const auto urlToRetrieve = url == "/" ? juce::String{"index.html"}
                                          : url.fromFirstOccurrenceOf("/", false, false);

    int dataSize = 0;
    const char* data = nullptr;

    if (urlToRetrieve == "index.html")
        data = BinaryData::getNamedResource("index_html", dataSize);

    if (data != nullptr && dataSize > 0)
    {
        std::vector<std::byte> vec((size_t)dataSize);
        std::memcpy(vec.data(), data, (size_t)dataSize);
        auto mime = getMimeForExtension(getExtension(urlToRetrieve).toLowerCase());
        return juce::WebBrowserComponent::Resource{std::move(vec), juce::String{mime}};
    }

    return std::nullopt;
}
