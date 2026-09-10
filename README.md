**KKM Flight Controller
**
**A standalone STM32-based quadrotor flight controller with integrated long-range LoRa telemetry.**

Senior Design Project · Morgan State University, Department of Electrical & Computer Engineering · Fall 2026

**Overview
**
Most hobby-grade flight controllers depend on a nearby phone or a short-range Wi-Fi/Bluetooth link, which limits them to line-of-sight operation in uncongested spectrum. This board takes a different approach: a single STM32F446RET6 handles sensor acquisition, attitude estimation, and stabilization on-chip, while a directly integrated 915 MHz transceiver maintains an independent command and telemetry link.

The design goal is reliable operation where conventional consumer links degrade — at range, through obstructions, and in congested RF environments.

**Why LoRa instead of Wi-Fi:** The transceiver uses Chirp Spread Spectrum, encoding data as continuous frequency sweeps rather than discrete symbols. This spreads energy across the channel, giving strong resistance to multipath and narrowband interference and allowing demodulation at negative signal-to-noise ratios — the SX1278 is specified down to roughly −20 dB SNR at the highest spreading factors. The tradeoff is bandwidth: the link carries telemetry and commands, not video.
