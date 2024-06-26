# Utility clone

VST plugin like a Ableton Utility.  
Development with [JUCE](https://github.com/juce-framework/JUCE)

![preview](https://github.com/m1m0zzz/utility-clone/assets/117814895/c0f1913b-e393-4dc6-935b-42fdf28d1640)


## ✨ Features

- phase invert
- channel mode
- stereo width, mid/side
- mono
- bass mono
- gain
- pan
- dc offset

### TODO
**Processing**
- [ ] bass mono : filter phase

## 📂 Download
- [Windows (zip)](https://github.com/m1m0zzz/utility-clone/archive/refs/heads/release/windows.zip)
- [Mac (zip)](https://github.com/m1m0zzz/utility-clone/archive/refs/heads/release/macos.zip)
- [Linux (zip)](https://github.com/m1m0zzz/utility-clone/archive/refs/heads/release/linux.zip)

## 🛠️ Build
Currently, it is possible to build with both Projucer and CMake

### use Projucer
- Edit `*.jucer` file
  - `/path/to/JUCE/modules` to the folder where your JUCE is downloaded
- Open Projucer

### use CMake

#### CMake + VSCode
- Open `*.code-workspace`
- Install extension

## 👷 CI

- GitHub Actions [(here...)](https://github.com/m1m0zzz/utility-clone/blob/main/.github/workflows/cmake-multi-platform.yml)
- CMake
- [ ] release by git tag

## License
GPL v3.0

## Acknowledgment

<a href="https://www.flaticon.com/free-icons/music" title="music icons">Music icons created by Freepik - Flaticon</a>

<a href="https://www.flaticon.com/free-icons/swap" title="swap icons">Swap icons created by bqlqn - Flaticon</a>
