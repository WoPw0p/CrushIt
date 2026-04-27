# CrushIt — Parameter Specification

## Module Order (Signal Chain)
```
Input → [Temporal Warp] → [Bit Crusher] → [SR Decimator] → [Clipper] → [Digital Debris] → [Bit Flipping] → Output
```
注：最终实现可以考虑允许用户拖动模块重排（如计划阶段评估复杂度）。

---

## Global

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `bypass` | Master Bypass | Bool | On/Off | Off | — | 全局旁通 |
| `output` | Output Gain | Float | -24.0 – +12.0 | 0.0 | dB | 输出增益补偿 |

---

## Module 1: Temporal Warp (时空畸变)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `tw_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `tw_buffer_ms` | Buffer Size | Float | 50 – 2000 | 500 | ms | 环形缓冲区长度 |
| `tw_detect_mode` | Detect Mode | Choice | Transient / Pitch / Free | Transient | — | 读取头跳跃触发模式 |
| `tw_sensitivity` | Sensitivity | Float | 0.0 – 1.0 | 0.6 | — | 检测灵敏度（瞬态阈值 / 音高变化敏感度） |
| `tw_jump_ms` | Jump Back | Float | 0.5 – 100 | 10 | ms | 检测触发后读取头回跳距离 |
| `tw_loop_ms` | Micro-loop | Float | 0.5 – 50 | 5 | ms | 微循环长度 |
| `tw_feedback` | Feedback | Float | 0.0 – 0.95 | 0.7 | — | 微循环反馈量 |
| `tw_direction` | Loop Direction | Choice | Forward / Alternating | Forward | — | 微循环读取方向 |
| `tw_mix` | Mix | Float | 0.0 – 1.0 | 0.5 | — | 干湿比 |

---

## Module 2: Bit Crusher (比特压缩)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `bc_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `bc_bits` | Bit Depth | Float | 1 – 32 | 12 | bits | 量化位深 (1=极度破坏, 32=透明) |
| `bc_mix` | Mix | Float | 0.0 – 1.0 | 0.5 | — | 干湿比 |

---

## Module 3: Sample Rate Decimator (采样率腰斩)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `sr_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `sr_divisor` | Rate Divisor | Float | 1 – 256 | 4 | x | 采样率除数 (1=原样, 256=极端降采样) |
| `sr_mode` | Hold Mode | Choice | ZOH / Linear | ZOH | — | 降采样插值模式 (零阶保持/线性) |
| `sr_mix` | Mix | Float | 0.0 – 1.0 | 0.5 | — | 干湿比 |

---

## Module 4: Waveform Clipper (波形裁剪)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `wc_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `wc_drive` | Drive | Float | -12.0 – +36.0 | 0.0 | dB | 前置增益 |
| `wc_threshold` | Threshold | Float | -36.0 – 0.0 | -6.0 | dB | 削波阈值 |
| `wc_mode` | Clip Mode | Choice | Hard / Soft / Fold | Hard | — | 削波类型 |
| `wc_mix` | Mix | Float | 0.0 – 1.0 | 0.5 | — | 干湿比 |

---

## Module 5: Digital Debris (数字噪点)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `dn_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `dn_type` | Noise Type | Choice | White / Burst / Crackle | White | — | 噪点类型 |
| `dn_level` | Level | Float | 0.0 – 1.0 | 0.3 | — | 噪点电平 |
| `dn_trigger` | Trigger Mode | Choice | Sync / Free | Sync | — | 触发模式 |
| `dn_sync_rate` | Sync Rate | Float | 1/16 – 4 | 1/4 | beats | 同步切片速率 (仅 Sync 模式) |
| `dn_free_rate` | Free Rate | Float | 0.5 – 100 | 8.0 | Hz | 自由运行速率 (仅 Free 模式) |
| `dn_decay_ms` | Burst Decay | Float | 1 – 200 | 20 | ms | 爆音衰减时间 |
| `dn_mix` | Mix | Float | 0.0 – 1.0 | 0.3 | — | 干湿比 |

---

## Module 6: Bit Flipping (位反转 — 浮点错误注入)

| ID | Name | Type | Range | Default | Unit | Description |
|:---|:---|:---|:---|:---|:---|:---|
| `bf_enable` | Enable | Bool | On/Off | Off | — | 模块开关 |
| `bf_target_bit` | Target Bit | Int | 0 – 31 | 22 | bit | IEEE 754 位位置 (0=LSB尾数, 22=MSB尾数, 23-30=指数, 31=符号) |
| `bf_mode` | Flip Mode | Choice | Invert / Random / Multiply | Invert | — | 反转模式 |
| `bf_probability` | Probability | Float | 0.0 – 1.0 | 0.05 | — | 每个采样的触发概率 |
| `bf_multiplier` | Error Multiplier | Float | 1.0 – 1000.0 | 100.0 | x | Multiply 模式下的放大倍数 |
| `bf_clip_db` | Clip Ceiling | Float | -24.0 – 0.0 | -3.0 | dB | 注入后的硬限幅上限 |
| `bf_bits_affected` | Bits Affected | Int | 1 – 8 | 1 | bits | 同时影响的位数 (多bit随机翻转) |
| `bf_mix` | Mix | Float | 0.0 – 1.0 | 0.5 | — | 干湿比 |

---

## Summary
- **Total Parameters:** 35
- **6 Processing Modules** with independent enable/mix controls
- **Complexity Score:** 5/5 (Multi-module destruction engine with novel DSP algorithms)
- **UI Framework:** TBD (Plan phase)
