# UI Specification v2 — CrushIt

## Changes from v1
- **Module names:** Simplified English (TIME WARP, BIT CRUSH, SAMPLE RATE, CLIP, DEBRIS, BIT FLIP)
- **Per-module visualizations:** Each tab now has a live Canvas visualization showing the module's effect on audio
- **Layout:** Two-column — visualization (left, flex) + controls (right, 320px fixed)

## Design Source
- **Inspiration:** Glitchmachines (industrial glitch, HUD aesthetic)
- **Style:** Textured Sci-Fi / Damaged Diagnostic Terminal
- **Framework:** WebView (HTML5 Canvas)

## Window
- **Size:** 800 × 500 px (fixed)

---

## Layout Structure

```
┌──────────────────────────────────────────────────────────────┐
│  HEADER: [CRUSH//IT]                      [BYPASS] [OUTPUT]  │ 44px
├──────────────────────────────────────────────────────────────┤
│  TAB BAR: [TIME WARP] [BIT CRUSH] [SAMPLE RATE] [CLIP] ...   │ 32px
├──────────────────────────────────────────────────────────────┤
│ ┌──────────────────────────┬─────────────────────────────┐   │
│ │                          │  Controls (320px wide)      │   │
│ │   LIVE CANVAS            │  Knobs, dropdowns,          │   │
│ │   VISUALIZATION          │  power buttons              │   │
│ │   (fills remaining       │                             │   │
│ │    width)                │                             │   │
│ └──────────────────────────┴─────────────────────────────┘   │
├──────────────────────────────────────────────────────────────┤
│  SIGNAL CHAIN:  IN → [TW]→[BC]→[SR]→[WC]→[DN]→[BF] → OUT    │ 32px
├──────────────────────────────────────────────────────────────┤
│  FOOTER: [I/O Level Meters L/R]              [CRUSH//IT v...] │ 30px
└──────────────────────────────────────────────────────────────┘
```

---

## Per-Module Visualizations

### M1: TIME WARP — Buffer Timeline
- Circular buffer visualization with write head (red dashed line, right side)
- Read head (cyan marker with glow) that jumps backward on transient detection
- Micro-loop zone: highlighted cyan rectangle when glitch active
- Direction arrows for Forward vs Alternating modes
- Jitter CSS animation on the module panel during glitch events

### M2: BIT CRUSH — Stepped Waveform + Color Banding
- Background: color posterization (gradient quantized to `2^bits` levels, showing banding)
- Right sidebar: vertical gradient bar showing discrete quantization steps
- Foreground: quantized sine wave (staircase steps visible at low bit depths)
- Bit depth and level count readout

### M3: SAMPLE RATE — Sparse Samples + Moiré
- Sparse sample dots (spacing proportional to divisor)
- ZOH mode: stair-step connect; Linear mode: straight lines between points
- Background: Moiré pattern overlay (intensifies with higher divisor)
- Pixel grid overlay at extreme rates
- Effective sample rate readout

### M4: CLIP — Waveform with Ceiling Indicator
- Dim reference waveform (unprocessed sine)
- Bright clipped waveform overlaid
- Threshold lines (red dashed) at clip ceiling
- CLIPPING warning text + red background glow when signal exceeds threshold
- Hard/Soft/Fold visual modes

### M5: DEBRIS — Noise Waveform + Trigger Timeline
- Real-time noise waveform (White: uniform, Burst: enveloped, Crackle: sparse impulses)
- Trigger marker ticks at bottom showing sync/free timing
- Level-dependent amplitude scaling
- Color-coded by noise type (Red/Orange/Yellow)

### M6: BIT FLIP — IEEE 754 Bit Map + Corrupted Waveform
- 32-bit grid at top: S (sign) / E (exponent) / M (mantissa) labeled
- Target bit(s) highlighted in red
- Foreground: corrupted waveform (cyan), background: clean reference (dim)
- Mode and probability readout

---

## Per-Module Control Layouts (right panel, 320px)

### TIME WARP (9 controls)
```
[ON] [BUFFER] [SENS] [DETECT▼]
[JUMP] [LOOP] [FBACK] [DIR▼]
[MIX]
```

### BIT CRUSH (3 controls)
```
[ON]      [BITS]      [MIX]
```

### SAMPLE RATE (4 controls)
```
[ON]    [DIVISOR] [HOLD▼]  [MIX]
```

### CLIP (5 controls)
```
[ON]   [DRIVE] [THRESH] [MODE▼] [MIX]
```

### DEBRIS (8 controls)
```
[ON]   [TYPE▼]   [LEVEL]   [TRIG▼]
[SYNC] [RATE Hz] [DECAY]   [MIX]
```

### BIT FLIP (8 controls)
```
[ON]   [MODE▼]   [TARGET BIT] [PROB]
[MULT] [CLIP dB] [AFFECTED]   [MIX]
```
