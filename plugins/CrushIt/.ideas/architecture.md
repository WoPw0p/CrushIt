# DSP Architecture — CrushIt

## Signal Chain (Fixed Order)
```
Input → [M1: Temporal Warp] → [M2: Bit Crusher] → [M3: SR Decimator] → [M4: Clipper] → [M5: Digital Debris] → [M6: Bit Flipping] → Output Gain → Output
```
注：Phase 4.1 实现固定顺序。模块重排功能可在后续版本评估。

---

## M1: Temporal Warp (时空畸变)

### Core Components
- **RingBuffer** (`juce::AudioBuffer<float>`, single channel max 2000ms)
  - Write head: appends every incoming sample
  - Read head: output position, controlled by detection state machine
  - Size: `tw_buffer_ms` × sampleRate / 1000
- **Transient Detector**
  - Envelope follower (peak decay, attack=1ms, release=50ms)
  - Threshold comparator: `envelope > tw_sensitivity × peak_hold`
  - Hysteresis to prevent re-triggering during cooldown window (10ms min)
- **Pitch Change Detector** (Free/Pitch mode)
  - Zero-crossing rate estimator (lightweight, per-block)
  - Delta-ZCR comparator: trigger when ZCR change exceeds threshold
- **Read Head State Machine**
  - `NORMAL`: read head = write head − buffer_delay (smooth following)
  - `JUMPING`: on detect → read head snaps back `tw_jump_ms` samples
  - `LOOPING`: micro-loop window [`read_pos`, `read_pos + loop_len`] repeats
    - Direction: Forward (→) or Alternating (↔ ping-pong)
    - Feedback: loop output × `tw_feedback` fed back into loop start
  - `RELEASE`: fade out of loop back to NORMAL over 5ms crossfade
- **Mix**: `out = dry × (1 − mix) + wet × mix`

### Key DSP Details
- Jump transition: 1ms linear crossfade to avoid clicks
- Micro-loop: fractional read with linear interpolation
- Feedback saturation: soft-clip feedback path to prevent runaway
- Cooldown: min 10ms between triggers to avoid rapid-fire glitching

---

## M2: Bit Crusher (比特压缩)

### Core Components
- **Quantizer**
  - Step size: `Δ = 2.0 / (2^bits − 1)`
  - Operation: `out = round(in / Δ) × Δ`
  - DC offset correction after quantization
- **Mix**: linear crossfade dry/wet

### Key DSP Details
- bits=1: 2-level quantization (hard square wave)
- bits=32: effectively bypass
- No anti-aliasing (the aliasing IS the effect)

---

## M3: Sample Rate Decimator (采样率腰斩)

### Core Components
- **Downsampler**
  - Hold counter: increments per sample, resets at `divisor`
  - ZOH mode: `out = last_held_value`
  - Linear mode: `out = last_held + (current − last_held) × (counter / divisor)`
- **Mix**: linear crossfade

### Key DSP Details
- Divisor=1: pass through
- Divisor=256 @ 48kHz → effective 187.5 Hz sample rate
- No anti-aliasing filter (aliasing mirrors are the desired effect)

---

## M4: Waveform Clipper (波形裁剪)

### Core Components
- **Drive Stage**: `in × db2mag(wc_drive)`
- **Clipper Stage** (threshold = `db2mag(wc_threshold)`):
  - **Hard**: `clamp(out, −threshold, +threshold)`
  - **Soft**: `tanh(out × 2.0 / threshold) × threshold`
  - **Fold**: `threshold − abs(abs(out + threshold) % (4 × threshold) − 2 × threshold)`
- **Mix**: linear crossfade

### Key DSP Details
- Wavefold uses triangle-wave reflection formula (Buchla-style)
- Soft clip gain compensation: tanh drive normalized to unity at threshold

---

## M5: Digital Debris (数字噪点)

### Core Components
- **Noise Generators**
  - **White**: `juce::Random::nextFloat() × 2 − 1`
  - **Burst**: white noise × exponential decay envelope (τ = `dn_decay_ms`)
  - **Crackle**: sparse random impulses (Poisson-like, probability 0.01/sample) × decay
- **Trigger Engine**
  - **Sync mode**: uses host playhead position (PPQ-based), fires at `dn_sync_rate` beat divisions
  - **Free mode**: internal timer, fires at `dn_free_rate` Hz
- **Envelope Generator**: triggered on each clock pulse
  - Instant attack, exponential decay (`dn_decay_ms`)
  - For Burst: noise duration = decay time
  - For Crackle: single-sample impulse with decay tail
- **Mix**: `out = dry + wet × dn_level` (additive, not crossfade; dry always passes through)

### Key DSP Details
- Sync mode requires `juce::AudioPlayHead` for BPM/position info
- Free mode uses sample counter: `samples_per_trigger = sampleRate / dn_free_rate`
- Crackle probability density increases with `dn_level`

---

## M6: Bit Flipping (浮点错误注入)

### Core Components
- **IEEE 754 Bit Manipulator**
  ```cpp
  union { float f; uint32_t i; } bits;
  bits.f = sample;
  // Flip target bit(s)
  uint32_t mask = 0;
  for (int b = 0; b < bf_bits_affected; ++b)
      mask |= (1u << (bf_target_bit + b));
  bits.i ^= mask;  // XOR to flip
  ```
- **Probability Gate**: per-sample random check against `bf_probability`
  - If `random() > bf_probability` → pass through unchanged
- **Three Flip Modes**
  - **Invert**: XOR specific bit(s) — deterministic at chosen position
  - **Random**: XOR with random bit pattern in target range — stochastic
  - **Multiply**: `sample × bf_multiplier` (bypasses bitmask entirely, uses huge gain + clip)
- **Post-Injection Hard Clipper**
  - Ceiling = `db2mag(bf_clip_db)`
  - `clamp(out, −ceiling, +ceiling)`
- **Safety**: check for NaN/Inf after injection, replace with 0.0
- **Mix**: linear crossfade

### IEEE 754 Float32 Layout
| Bit Range | Field | Effect of Flipping |
|:---|:---|:---|
| 0–22 | Mantissa | Subtle granular noise → harsh quantization |
| 23–30 | Exponent | Extreme jumps (×2^127), near-instant clipping |
| 31 | Sign | Polarity inversion (audible as phase flip) |

### Key DSP Details
- Target bit 22 (MSB of mantissa): audible grit without total collapse
- Target bits 23-30 (exponent): WARNING — can produce +Inf. Always clip after injection
- Multiply mode + hard clipper: produces squared-off, fuzz-like distortion
- Bits Affected > 1: wider destruction, approaching white noise at 8 bits
- Denormal handling: flush to zero to avoid CPU spikes on some hardware

---

## Output Stage

- **Gain**: `out × db2mag(output_gain)`
- **Bypass**: smooth fade (5ms) between dry input and wet output when toggling master bypass
- All modules internally bypassed when their `_enable` is OFF (their mix at 0.0 = dry pass-through)

## Complexity Assessment

| Factor | Level |
|:---|:---|
| Novel DSP Algorithms | Very High — Bit Flipping IEEE 754 manipulation, Temporal Warp transient-driven micro-looping are novel |
| Parameter Count | High — 35 parameters across 6 modules |
| Real-time Constraints | High — per-sample bit manipulation, buffer management, multiple noise generators |
| State Management | Moderate — 6 independent enable states, read head position tracking |
| UI Complexity | High — 6 tabbed modules, real-time visual feedback needed |

**Score: 5/5 (Research-grade)**
**Rationale:** The Bit Flipping module's IEEE 754 direct manipulation is unprecedented in audio plugins. Temporal Warp's transient-driven read-head jumping with micro-looping feedback requires careful buffer management and state machine design. Combined with 4 other modules and 35 parameters, this exceeds standard plugin complexity.
