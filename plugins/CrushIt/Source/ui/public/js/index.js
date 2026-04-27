import * as Juce from "./juce/index.js";

// Parameter state initialization
const paramDefs = [
  ["bypass", 0, 1, 0],
  ["output", -24, 12, 0],
  ["tw_enable", 0, 1, 1], ["tw_buffer_ms", 50, 2000, 500], ["tw_sensitivity", 0, 1, 0.6],
  ["tw_detect_mode", 0, 2, 0], ["tw_jump_ms", 0.5, 100, 10], ["tw_loop_ms", 0.5, 50, 5],
  ["tw_feedback", 0, 0.95, 0.7], ["tw_direction", 0, 1, 0], ["tw_mix", 0, 1, 0.5],
  ["bc_enable", 0, 1, 0], ["bc_bits", 1, 32, 12], ["bc_mix", 0, 1, 0.5],
  ["sr_enable", 0, 1, 0], ["sr_divisor", 1, 256, 4], ["sr_mode", 0, 1, 0], ["sr_mix", 0, 1, 0.5],
  ["wc_enable", 0, 1, 0], ["wc_drive", -12, 36, 0], ["wc_threshold", -36, 0, -6],
  ["wc_mode", 0, 2, 0], ["wc_mix", 0, 1, 0.5],
  ["dn_enable", 0, 1, 0], ["dn_type", 0, 2, 0], ["dn_level", 0, 1, 0.3],
  ["dn_trigger", 0, 1, 0], ["dn_sync_rate", 0.0625, 4, 0.25],
  ["dn_free_rate", 0.5, 100, 8], ["dn_decay_ms", 1, 200, 20], ["dn_mix", 0, 1, 0.3],
  ["bf_enable", 0, 1, 0], ["bf_mode", 0, 2, 0], ["bf_target_bit", 0, 31, 22],
  ["bf_probability", 0, 1, 0.05], ["bf_multiplier", 1, 1000, 100],
  ["bf_clip_db", -24, 0, -3], ["bf_bits_affected", 1, 8, 1], ["bf_mix", 0, 1, 0.5]
];

const states = {};
const sliderStates = {};
paramDefs.forEach(([id, min, max, def]) => {
  const s = Juce.getSliderState(id);
  s.setRange(min, max, (max - min) / 1000);
  s.setValue(def);
  s.addListener((newVal) => { states[id] = newVal; });
  states[id] = def;
  sliderStates[id] = s;
});

// Expose for module access
window.paramStates = states;
window.juceSliders = sliderStates;
window.paramDefs = paramDefs;

// Meter update loop
setInterval(() => {
  // Placeholder for native meter data
}, 33);
