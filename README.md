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
Download the zip for your OS (Windows / Mac / Linux) from [Releases](https://github.com/m1m0zzz/utility-clone/releases/latest).

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
- Release by git tag
  1. Update `VERSION` in `CMakeLists.txt`
  2. Push a tag such as `v1.2.0` (it must match `VERSION`)
  3. The binaries for each OS are uploaded to GitHub Releases

## License
AGPL v3.0

JUCE 9 is used under the [AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html).

## Acknowledgment

<a href="https://www.flaticon.com/free-icons/music" title="music icons">Music icons created by Freepik - Flaticon</a>

<a href="https://www.flaticon.com/free-icons/swap" title="swap icons">Swap icons created by bqlqn - Flaticon</a>
