# Style Guide v1 — CrushIt

## Color Palette

```
  Background:  #0B0B10    ████████  Main body
  Surface:     #13131D    ████████  Card/panel backgrounds
  Surface Alt: #1A1A28    ████████  Tab bar, chain bar
  Primary:     #00E5FF    ████████  Accent cyan (knobs, active states)
  Secondary:   #FF3860    ████████  Warning, bypass indicator, clip
  Text:        #E0E4F0    ████████  Primary text, labels
  Text Dim:    #5A6078    ████████  Secondary text, units
  Border:      #1E1E30    ████████  Panels, dividers
  Glow:        rgba(0,229,255,0.6)  LED glow effects
  Disabled:    #2A2A38    ████████  Disabled modules, off state
```

## Typography

| Element | Font | Size | Weight | Style |
|:---|:---|:---|:---|:---|
| Plugin Name | JetBrains Mono | 18px | 700 | uppercase, letter-spacing 3px |
| Tab Labels | JetBrains Mono | 11px | 500 | uppercase, letter-spacing 1.5px |
| Knob Labels | JetBrains Mono | 9px | 400 | uppercase, letter-spacing 1px |
| Knob Values | JetBrains Mono | 11px | 700 | monospaced digits |
| Module Title | JetBrains Mono | 13px | 600 | uppercase, letter-spacing 2px |
| Footer Text | JetBrains Mono | 8px | 400 | uppercase |

Fallback stack: `"JetBrains Mono", "Consolas", "SF Mono", "Menlo", monospace`

## Control Specifications

### Knob
- **Diameter:** 48px (standard), 56px (primary params)
- **Ring:** 3px stroke, #1E1E30 background, #00E5FF fill (proportional arc)
- **Tick:** 2px × 10px line at 0° (12 o'clock), cyan
- **Interaction:** Click+drag up/down, mousewheel, double-click to reset
- **Glow:** box-shadow 0 0 8px rgba(0,229,255,0.3) when hovered
- **Label:** below knob, 9px uppercase

### Power Button (Enable)
- **Size:** 28×28px
- **OFF:** #2A2A38 fill, #1E1E30 border
- **ON:** #00E5FF fill with glow pulse
- **Interaction:** Click to toggle

### Dropdown (Choice params)
- **Width:** 120px
- **Style:** Dark surface with cyan border, chevron icon
- **Options:** Same monospace font, cyan highlight on hover

### Slider (for mix params)
- **Width:** 80px, height: 4px track
- **Track:** #1E1E30, **Fill:** #00E5FF gradient
- **Thumb:** 10px circle, cyan with glow

## Spacing
- **Panel padding:** 16px
- **Knob gap:** 12px horizontal, 16px vertical
- **Section gap:** 20px
- **Tab height:** 36px
- **Header height:** 48px

## Visual Effects

### Scanlines (CSS)
```css
background: repeating-linear-gradient(
  0deg,
  transparent,
  transparent 2px,
  rgba(0,0,0,0.03) 2px,
  rgba(0,0,0,0.03) 4px
);
```

### Noise Overlay (CSS/SVG)
```css
/* Full-window overlay, pointer-events: none */
opacity: 0.04;
/* feTurbulence SVG filter applied */
```

### LED Glow
```css
box-shadow: 0 0 6px rgba(0, 229, 255, 0.4),
            0 0 12px rgba(0, 229, 255, 0.2);
```

### Module States
- **Enabled:** Full cyan glow, active tab underline
- **Disabled:** Dimmed #2A2A38, no glow, gray tab text
- **Bypassed (global):** Red indicator pulse on bypass button
