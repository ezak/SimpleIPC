# SimpleIPC

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](#)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey.svg)](#)

### Introduction

`SimpleIPC` is a high-performance, low-latency Inter-Process Communication (IPC) utility optimized for deterministic, zero-copy data transmission. By decoupling serialization from transport logic, the framework utilizes shared memory primitives (`mmap`/POSIX shared memory).

Rather than incurring heavy CPU serialization and allocation overhead, `SimpleIPC` relies directly on FlatBuffers to read message payloads in-place from memory-mapped regions. This architecture guarantees a fully bounded memory footprint, zero dynamic runtime allocations in critical execution paths, and predictable sub-microsecond latency boundaries, making it highly suited for systems engineering, telemetry pipelines, and high-throughput localized streaming.

---

### Build

This project uses a standard, native build toolchain to minimize compilation complexity and enforce predictable optimization vectors.

#### Dependencies
- **FlatBuffers compiler (`flatc`)**: Required to compile the underlying `.fbs` schema files into native layouts.
- **C/C++ Compiler**: `GCC` (>= 10.0) or `Clang` (>= 11.0) with support for modern standards (`C++20`).
- **Build Automation**: `CMake` >= 3.15.

#### Compiling Schemas (TODO)
Before compiling the core source binaries, generate the flat buffer headers from your protocol definitions:
```bash
flatc --cpp --cpp-std c++20 -o include/schemas/ schemas/MessageProtocol.fbs