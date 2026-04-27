# Implementation Plan — CrushIt

## Complexity Score: 5/5

## Implementation Strategy: Phased (5-tier)

由于复杂度为 5/5，实现分为 5 个子阶段，按风险递增排序：

---

### Phase 4.1.1: 基础架构 + 低风险模块 (M2, M3, M4)
**目标:** 建立完整信号链路骨架，实现三个最成熟的模块。

- [ ] PluginProcessor 框架（35 参数注册、状态管理）
- [ ] PluginEditor WebView 框架（JUCE WebBrowserComponent 集成）
- [ ] 固定信号链路由（6 模块串联骨架）
- [ ] **M2: Bit Crusher** — 量化器 + DC 校正
- [ ] **M3: SR Decimator** — ZOH/Linear 双模式降采样
- [ ] **M4: Waveform Clipper** — Hard/Soft/Fold 三模式削波 + Drive
- [ ] 全局 Bypass + Output Gain
- [ ] 参数平滑 (ramped parameter changes)

**可测试:** 三个模块独立开关 + 串联工作，生成有效音频输出。

---

### Phase 4.1.2: 时空畸变 (M1)
**目标:** 实现最高风险的模块——这是最复杂的 DSP 组件。

- [ ] 环形缓冲区实现（2000ms 最大延迟）
- [ ] 瞬态检测器（包络跟随 + 阈值 + 滞后）
- [ ] 音高变化检测器（ZCR 估计，轻量级）
- [ ] 读取头状态机 (NORMAL → JUMPING → LOOPING → RELEASE)
- [ ] 微循环引擎（Forward/Alternating 方向 + Feedback）
- [ ] 跳转/循环过渡的交叉淡化（抗爆音）
- [ ] Free 模式（随机/周期跳转，不依赖检测）

**可测试:** 用鼓 loop 测试瞬态触发 → 回跳 → 微循环嗡鸣声。

---

### Phase 4.1.3: 数字残骸 + 位反转 (M5, M6)
**目标:** 实现两个创新模块。

**M5: Digital Debris**
- [ ] 白噪声、Burst、Crackle 三种发生器
- [ ] Sync 模式（宿主 PlayHead 同步）
- [ ] Free 模式（内部时钟）
- [ ] 指数衰减包络

**M6: Bit Flipping**
- [ ] IEEE 754 位操作（union float/int 位反转）
- [ ] Invert / Random / Multiply 三种模式
- [ ] 概率门控（per-sample random check）
- [ ] 后注入硬限幅（clamp to ceiling）
- [ ] NaN/Inf 安全检查 + 刷新为零
- [ ] 多位翻转（Bits Affected 1–8）

**可测试:** M5 节拍同步触发噪音；M6 从微妙颗粒感到完全信号崩溃。

---

### Phase 4.1.4: WebView UI
**目标:** 构建完整用户界面。

- [ ] 6 模块标签页布局（Tab-based module switching）
- [ ] 科技纹理美学（深色主题、冷色 LED 调色板、扫描线 CSS 效果）
- [ ] 每个模块独立控制面板（旋钮/开关/下拉菜单）
- [ ] 信号链可视化（模块顺序示意图）
- [ ] 实时电平表（输入/输出）
- [ ] JUCE ↔ JS 参数双向绑定
- [ ] 预设管理系统

---

### Phase 4.1.5: 集成、测试与优化
**目标:** 最终打磨。

- [ ] 全模块串联测试（6 模块同时启用）
- [ ] CPU 性能分析（每个模块单独 + 全链）
- [ ] 参数自动化测试（宿主自动化包络）
- [ ] 边界条件处理（buffer 空/满、NaN 传播、极端参数组合）
- [ ] DAW 兼容性测试（Ableton Live, Logic Pro, FL Studio, Reaper）
- [ ] 延迟补偿报告（Temporal Warp buffer 引入的延迟）

---

## Required JUCE Modules

```cmake
juce_add_plugin(CrushIt
    FORMATS VST3 AU
    PRODUCT_NAME "CrushIt"
    NEEDS_WEB_BROWSER TRUE)

target_link_libraries(CrushIt PRIVATE
    juce::juce_dsp
    juce::juce_audio_utils
    juce::juce_gui_extra)
```

- `juce_audio_basics` — 音频缓冲、参数
- `juce_audio_processors` — 插件框架、PlayHead
- `juce_dsp` — 包络跟随器、振荡器（噪音用）
- `juce_gui_extra` — WebBrowserComponent
- `juce_audio_utils` — 电平表工具

## Risk Assessment

### High Risk
| # | Risk | Mitigation |
|:---|:---|:---|
| 1 | **Bit Flipping NaN/Inf 传播** — 翻转指数位产生非有限值，导致下游模块/DAW崩溃 | 每样本 NaN/Inf 检测 + flush to zero；后置硬限幅作为第二道防线 |
| 2 | **Temporal Warp 微循环反馈失控** — 反馈 >1.0 时产生 runaway oscillation | 反馈路径内置 soft-clip 限制器；`tw_feedback` 上限 0.95 |
| 3 | **WebView 性能** — 35 参数实时双向同步可能丢帧 | 30Hz timer 批量更新；JS 端 requestAnimationFrame 节流 |

### Medium Risk
| # | Risk | Mitigation |
|:---|:---|:---|
| 4 | **瞬态检测误触发** — 连续快速触发导致读取头乱跳 | 10ms cooldown window + 滞后阈值 |
| 5 | **Sync 模式无 PlayHead** — 部分 DAW 不提供 tempo 信息 | Free 模式作为 fallback；检测 PlayHead nullptr |
| 6 | **参数计数过多** — 35 参数使 DAW 自动化列表臃肿 | 合理分组命名；考虑"简化模式"切换 |

### Low Risk
| # | Risk | Mitigation |
|:---|:---|:---|
| 7 | **SR Decimator ZOH 爆音** — 低频降采样产生直流偏移 | 隔直滤波器（DC blocker）在降采样后 |
| 8 | **模块串联增益 staging** — 6 模块累积增益过高 | 每个模块 Mix 0% = 完全干信号通过；输出增益有 -24dB 下限 |

---

## File Creation Order

```
Phase 4.1.1:
  Source/PluginProcessor.h    — 参数枚举 + 声明
  Source/PluginProcessor.cpp  — 参数注册 + M2/M3/M4 DSP + 信号链
  Source/PluginEditor.h       — WebView 框架
  Source/PluginEditor.cpp     — WebView 集成 + 参数绑定

Phase 4.1.2:
  [修改] PluginProcessor.cpp  — 添加 M1 DSP

Phase 4.1.3:
  [修改] PluginProcessor.cpp  — 添加 M5 + M6 DSP

Phase 4.1.4:
  Design/index.html           — 完整 WebView UI

Phase 4.1.5:
  [修改] 所有文件             — 优化 + 修复
```
