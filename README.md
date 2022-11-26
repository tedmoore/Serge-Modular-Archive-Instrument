# Serge Modular Archive Instrument

(it's a working title)

The _Serge Modular Archive Instrument_ is created by [Jean Brazeau](https://221a.ca/contributors/jean-brazeau/) and [Ted Moore](https://www.tedmooremusic.com/). It is a sample-based computer emulation of selected patches on the vintage Serge Modular instrument that is housed at Simon Frasier University in Vancouver, Canada. The project is conceived of as both an instrument for sonic exploration and an archive of the sound worlds made by this 50+ year old instrument, including (or highlighting) all of the idiosyncrasies it has accumulated over the years.

![Full SMAI](./documentation/figures/serge-full.jpg)

## How to Use This Application:

1. You'll need to download the [latest release](https://github.com/tedmoore/serge-archive-oF/releases/latest) from this repo. 
2. Download a patch (essentially a folder of files) from [this Google Drive folder](https://drive.google.com/drive/folders/1LRe2oOUYgVRhTToPmIJWTU4CTBDvpJhz?usp=sharing). It will contain:
    - `wav` audio files containing the synth sounds
    - a `csv` file containing the data of the audio analyses
    - a `json` file containing some additional data needed for the software to load the data
3. Open the application downloaded in step 1. 
    - The first thing that will happen is the software will ask you to choose a folder: select the folder downloaded in step 2.
4. When the application is done loading the folder it will open and display a menu for selecting your MIDI device. Choose a MIDI device by pressing the number on your QWERTY keyboard next to the MIDI device you would like to use. Your selection should turn red. **To close this menu and begin using the application to make sound, hit the letter `m`**
5. Make sound. Click on the plot, click on the knobs, etc. Refer to the [User Manual](https://github.com/tedmoore/serge-archive-oF/blob/main/documentation/SMAI-User-Manual.pdf) (in-progress) found in this repo for more instructions.

## How Does This Work?

### Recording & Analysis

For each patch included we recorded tens of thousands of one-second samples of different parameter settings by systematically stepping through parameter combinations (sent to the Serge using the [Expert Sleepers ES-3](https://www.expert-sleepers.co.uk/es3.html). These samples were then analyzed using the [FluCoMa](https://www.flucoma.org/) Toolkit for analyses such as [pitch](https://learn.flucoma.org/reference/pitch/), [spectral descriptors](https://learn.flucoma.org/reference/spectralshape/), [timbral descriptors](https://learn.flucoma.org/reference/mfcc/), [loudness](https://learn.flucoma.org/reference/loudness/), and more. The data set created is then reduced to two dimensions using [PCA](https://learn.flucoma.org/reference/pca/) and then [UMAP](https://learn.flucoma.org/reference/umap/).

### Control 

These audio samples can then be accessed by navigating a two-dimensional plot (plus color), which can show the sound samples according to pitch, pitch confidence, loudness, spectral centroid, spectral flatness, or either of the two UMAP dimensions. 

![2D Plot](./documentation/figures/serge-plot.jpg)

Additionally, a skeuomorph of the Serge Modular highlights which knobs control the synthesis parameters used, allowing the user to imitate controlling the vintage instrument. Beneath the two-dimensional plot there is a step sequencer for storing chosen samples to be returned to in sequence.

![Skeuomorph](./documentation/figures/serge-skeuomorph.jpg)

All of the control parameters (including _x_ and _y_ plot navigation, skeuomorph knobs, and step sequencer) are controllable via MIDI and OSC allowing user defined control of the instrument.

## License

The license for the code is BSD-3 Clause and the license for the audio files and accompanying data is licensed under Creative Commons [Attribution 4.0 International](http://creativecommons.org/licenses/by/4.0/).

## Building

Currently, this project exists in XCode. The repo does contain a `Makefile` which does work for building. If you want to try building this on your local computer you'll need the dependencies below.

### Dependencies for Building

* [openFrameworks](https://openframeworks.cc/) (and this repo needs to be in the appropriate `myApps` folder in the openFrameworks folder)
* openFrameworks addon: [ofxAudioFile](https://github.com/npisanti/ofxAudioFile)
* openFrameworks addon: [ofxKDTree](https://github.com/genekogan/ofxKDTree)
* openFrameworks addon: **ofxMidi**: comes with openFrameworks
* openFrameworks addon: **ofxOsc**: comes with openFrameworks

**This repo also uses the [nlohmann json c++ library](https://github.com/nlohmann/json) which is already included in the `thirdparty` folder of `src`**.  
Lohmann, N. (2022). JSON for Modern C++ (Version 3.11.2) [Computer software]. https://github.com/nlohmann

### Dependencies for generating some material used when building:

Some media and data used in the build is already in the repo and doesn't need to be remade, these media were created using the dependencies below, but hopefully these dependencies can be replaced with Python soon.

* [Processing](https://processing.org/) (only for analyzing the skeuomorph graphics when they are updated--there are plans to get rid of this dependency, replacing it with a Python script)
* [SuperCollider](https://supercollider.github.io/) (same as above, only for analyzing the skeuomorph graphics when they are updated--there are plans to get rid of this dependency, replacing it with a Python script--the same Python script as mentioned above)
