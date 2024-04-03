# Utility clone

VST plugin like a Ableton Utility.  
Development with [JUCE](https://github.com/juce-framework/JUCE)

![image](https://github.com/m1m0zzz/utility-clone/assets/117814895/d256fde1-0c0f-4889-8f5b-c9f0dea2a480)


## ✨ Features
- gain
- phase invert
- pan
- mono
- bass mono
- stereo width, mid/side

### TODO
**Processing**
- [ ] bass mono : filter phase

**UI**
- [ ] popup menu

## 🛠️ Build
Currently, it is possible to build with both Projucer and CMake

### use Projucer
- Edit `*.jucer` file
  `/path/to/JUCE/modules` to the folder where your JUCE is downloaded
- Open Projucer

### use CMake
TODO

#### CMake + VSCode
- Open `*.code-workspace`
- Install extension

## 👷 CI
TODO

- GitHub Actions
- CMake
- release by git tag

## License
GPL v3.0
