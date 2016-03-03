# neuPlayer
<i>This software is available in French and english. It'll automatically adjust for you when executing it </i>
 
##Downloads

A pre-compiled binary for <b>Windows</b> is available <b>[here](http://goo.gl/QiW88m) </b>(build 290515)

There will be more links available for other OS soon.


##Presentation 

![Screenshot] (http://puu.sh/ntHdY/6dd8a5f0d2.png) 

neuPlayer is a project I started as a challenge to further enhance my skills in C++ and Qt.
It is a fully portable lightweight music player (it runs on a USB drive smoothly !) which can be used as a quick replacement to players like Windows Media Player, iTunes or VLC.

It's entirely built with the Qt classes, so it doesn't support codecs on its own; It's limited to codecs that you have installed already, but there's a plan to bring FLAC support alongside the player package.
Its main goals are : 

  - <b>Speed</b>
  - <b>Portability</b>
  - <b>Simplicity</b>
  - <b>Customization</b>

With those in mind, I'm trying to do a player which can possibly be the daily-driver of most people who just start the player and let the music play while they work.

##Features
 As of 10/03/15, I decided to make this project public on GitHub. Right now it features : 
 - <b>Fast-loading from a base music folder</b>
 
 - Playlist basic functions (<b>Random play</b>, shuffle playlist, loop, <b>favorites</b>...)
 
 - <b>Always on top toggle </b>
 
 - Setup for quick config at first run
 
 - Album art via Tag Viewer or thumbnail on Playlist
 
 - 1.0 x, 0.5x, 2.0x playback
 
 - <b>Skinning !</b>
 
 - Frameless Window capabilities

 - Smooth animations
 
 - Low memory footprint
 
 - Updater built-in to keep the player updated


## How to compile

If you want to compile neuPlayer manually you just need [Qt](https://www.qt.io/) (version 5.2.0 or above is required, 5.3.2 is the one used for release builds).

To compile it, run `qmake`and build
 
## License
**This software is licensed under GNU GPL version 3.**
You can find the full text of the license [here](LICENSE.txt).
