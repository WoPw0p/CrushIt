# UI Specification v1 — CrushIt

## Design Source
- **Inspiration:** Glitchmachines (industrial glitch, HUD aesthetic, noise textures)
- **Style:** Textured Sci-Fi / Damaged Diagnostic Terminal
- **Framework:** WebView (HTML5 Canvas)

## Window
- **Size:** 800 × 500 px
- **Minimum:** 800 × 500 px (fixed)

---

## Layout Structure

```
┌──────────────────────────────────────────────────────────────┐
│  HEADER: [Logo/Name]          [BYPASS] [OUTPUT GAIN knob]   │ 48px
├──────────────────────────────────────────────────────────────┤
│  TAB BAR: [时空畸变] [比特压缩] [采样率腰斩] [裁剪] [噪点] [位反转] │ 36px
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  MODULE PANEL (active tab content)                           │ 340px
│  - Knob grid with LED rings                                  │
│  - Module-specific control layout                            │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│  SIGNAL CHAIN:  In → [M1]→[M2]→[M3]→[M4]→[M5]→[M6] → Out   │ 40px
├──────────────────────────────────────────────────────────────┤
│  FOOTER: [I/O Level Meters]                                  │ 36px
└──────────────────────────────────────────────────────────────┘
```

---

## Per-Module Knob Layouts

### Tab 1: Temporal Warp (9 params)
```
[ENABLE]  [BUFFER]  [SENSITIVITY]  [DETECT MODE]
[JUMP]    [LOOP]    [FEEDBACK]     [DIRECTION]
                                    [MIX]
```
- 3 columns + 1 sidebar
- Detect mode as dropdown, Direction as toggle pair

### Tab 2: Bit Crusher (3 params)
```
        [ENABLE]    [BITS]    [MIX]
```
- Centered horizontal row

### Tab 3: SR Decimator (4 params)
```
    [ENABLE]  [DIVISOR]  [MODE]  [MIX]
```
- Centered horizontal row

### Tab 4: Clipper (5 params)
```
  [ENABLE]  [DRIVE]  [THRESHOLD]  [MODE]  [MIX]
```
- Centered horizontal row

### Tab 5: Digital Debris (8 params)
```
[ENABLE]   [TYPE]    [LEVEL]    [TRIGGER]
[SYNC RATE] [FREE RATE] [DECAY] [MIX]
```
- 4×2 grid

### Tab 6: Bit Flipping (8 params)
```
[ENABLE]   [MODE]    [TARGET BIT] [PROBABILITY]
[MULTIPLIER] [CLIP]  [BITS AFFECTED] [MIX]
```
- 4×2 grid
- Target Bit has large numeric readout (0-31)

---

## Color Palette

| Role | Hex | Usage |
|:---|:---|:---|
| Background | `#0B0B10` | Main body bg |
| Surface | `#13131D` | Module panel, knob plates |
| Surface Alt | `#1A1A28` | Tab bar, chain bar |
| Primary Cyan | `#00E5FF` | Accent, active tab, knob rings |
| Secondary Red | `#FF3860` | Bypass indicator, clip warning |
| Text Primary | `#E0E4F0` | Labels, values |
| Text Dim | `#5A6078` | Secondary labels, units |
| Border | `#1E1E30` | Panel borders, dividers |
| LED Glow | `rgba(0,229,255,0.6)` | Active element glow |

## Typography
- **Primary:** "JetBrains Mono", "Consolas", monospace (terminal/tech vibe)
- **Labels:** 10px, uppercase, letter-spacing 1px
- **Values:** 12px, bold, cyan

## Special Effects
- **Scanlines:** CSS repeating-linear-gradient overlay, 3px spacing, opacity 0.03
- **Noise texture:** SVG feTurbulence overlay, opacity 0.04
- **LED Glow:** box-shadow + text-shadow in cyan for active states
- **Tab indicator:** animated underline bar, cyan glow
- **Signal chain:** connected module blocks with arrow connectors, dim if module disabled
