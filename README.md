# Speed Shifter

Speedshifter is a practice tool designed for musicians to slow down the tempo of an audio track without affecting the pitch. This enables comfortable practice at slower speeds, improving accuracy and dexterity.

It is currently available on Linux x86-64, and is currently still in the beta phase of development. The Windows build is currently being worked on.

To try it out on Linux, you can clone the repo and run the install script:
```
# clone the repo
mkdir speedshifter
cd speedshifter
git clone https://github.com/snej55/speedshifter.git .

# build and install
chmod +x ./install.sh
./install.sh
```

NOTE: To compile and run the program, it is required to first install gstreamer and it's plugins (gst-plugins-base, gst-plugins-bad, gst-plugins-good, gst-plugins-ugly) via your distro's package manager. This will soon be added to the install script. 

![screenshot](https://hc-cdn.hel1.your-objectstorage.com/s/v3/dc2dfeb3468823cc64fd2bceddc2b5c2400cf9cf_image.png)

## Technical Info:
Speedshifter was developed using Kirigami2 and Qt6 for the user interface and window management, as well as other libraries from KDE Frameworks. For more information about this, see [https://develop.kde.org/docs/](https://develop.kde.org/docs/). Speedshifter uses gstreamer to contruct the audio pipeline, using the playbin element for playback across different audio formats. To 'shift' the tempo without affecting the pitch, the pitch and scaletempo plugins are used. At first, the audio playback rate was changed by performing a seek to manually change the playback speed, but this was inffective with certain audio formats (notably mp3 files). Hence, the pitch element is used instead to changed the playback rate, which has worked for all of the audio formats tested. The scaletempo element is used to scale the tempo without affecting the pitch (avoiding the chipmunk effect). For more information see: [gstreamer docs](https://gstreamer.freedesktop.org/documentation/?gi-language=c), [scaletempo](https://gstreamer.freedesktop.org/documentation/audiofx/scaletempo.html?gi-language=c).

For more in depth information on how the audio pipeline is contructed, see the source code: [Player::load()](https://github.com/snej55/speedshifter/blob/main/src/player.cpp#L88). 

## TODO:

- [X] Add file dialog
- [X] Add media controls
- [X] Add seeking
- [X] Add buffer for seeking
- [X] Add scaletempo
- [X] Fix mp3s
- [X] Don't allow 0% speed
- [X] Fix slider for mp3s
- [X] Make launchable
- [ ] Icon
- [ ] Settings menu
- [X] Fix desktop entry
- [X] Add screenshots
- [X] Make website + download page
- [X] Make promo video
- [ ] Scale dial properly
- [ ] Add vfx
- [ ] Make crossplatorm ports (Windows -> Mac -> Android -> iOS)
- [ ] Add easy install for linux (scale install.sh)

#### Polish:

 - [ ] Error banner
 - [X] Metadata
