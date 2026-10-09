# Opus GDExtension

[![Godot Asset Store](https://img.shields.io/github/v/release/Joy-less/OpusGdextension.svg?label=Godot%20Asset%20Store&logo=godotengine)](https://store.godotengine.org/asset/joyless/opus-gdextension)

This addon lets you import Opus, a modern lossy audio encoding with excellent compression, in Godot 4.

Opus GDExtension adds `AudioStreamOpus`, which is almost identical in features to the built-in `AudioStreamOggVorbis`.

## Features

- Import `.opus` files in the editor.
- Play the `.opus` files in the editor inspector.
- Load `.opus` files at runtime.
- Play `.opus` files at runtime.

## How to Use

To convert a WAV file to Opus at 128 kbits/s, install [FFmpeg](https://ffmpeg.org) and run:
```
ffmpeg -i input.wav -c:a libopus -b:a 128k output.opus
```

## Recommended Bitrates

Opus is transparent (perceptually lossless) at 128 kbits/s for music and 32 kbits/s mono for voice.

If you're looking to save as much space as possible, Opus is very high quality at 96 kbits/s for music.

| Format | Transparent Bitrate (Music) | Transparent Bitrate (Voice) |
|---|---|---|
| Opus | 128 kbits/s | 32 kbits/s mono |
| Ogg Vorbis | 160 kbits/s | 64+ kbits/s mono |
| MP3 | 192 kbits/s | 96+ kbits/s mono |

## Supported Platforms

Opus GDExtension has a baseline of Godot 4.5 and aims to support every platform supported by Godot 4.5.

Note: Every supported platform has binaries for single (but not double) precision and debug/release mode.

| Platform | Supported | SIMD Optimizations |
|---|---| --- |
| Windows x64 | ✅ | ✅ |
| Windows x32 | ✅ | ✅ |
| Windows ARM64 | ✅ | ❌ (1) |
| Linux x64 | ✅ | ✅ |
| Linux x32 | ✅ | ✅ |
| Linux ARM64 | ✅ | ✅ |
| Linux ARM32 | ✅ | ❌ (2) |
| macOS Universal | ✅ | ✅ |
| Android x64 | ✅ | ✅ |
| Android x32 | ✅ | ✅ |
| Android ARM64 | ✅ | ✅ |
| Android ARM32 | ✅ | ❌ (2) |
| iOS ARM64 | ✅ | ✅ |
| Web WASM32 | ✅ | ❌ (3) |

*(1) Windows ARM64 uses setup-mingw, which does not provide `arm_neon.h`.*

*(2) ARM32 is not guaranteed to have NEON, so those optimizations are not included.*

*(3) WASM cannot use processor-specific optimizations.*

## Proposal

There is a [proposal](https://github.com/godotengine/godot-proposals/issues/15584) and [pull request](https://github.com/godotengine/godot/pull/117921) to add Opus into Godot core.

Please give them a thumbs up (👍) so it's more likely to be merged!

## Credits

To use this library, you need to follow the license of the following repositories:
- [Opus GDExtension by Joyless (MIT license)](https://github.com/Joy-less/OpusGdextension)
- [Ogg by Xiph (BSD-3-Clause license)](https://github.com/xiph/ogg)
- [Opus by Xiph (BSD-3-Clause license)](https://github.com/xiph/opus)
- [Opusfile by Xiph (BSD-3-Clause license)](https://github.com/xiph/opusfile)
- [Godot by Godot Engine contributors (MIT license)](https://github.com/godotengine/godot)
