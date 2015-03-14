# neuPlayer
- <i>This software is available in French and english. It'll automatically adjust for you when executing it </i>

##Presentation 

![Screenshot] (http://puu.sh/guRg6/04a820b7ef.jpg)


neuPlayer is a project I started as a challenge to further enhance my skills in C++ and Qt.
It is a fully portable lightweight music player (it runs on a USB drive smoothly !) which can be used as a quick replacement to players like Windows Media Player, iTunes or VLC.

It's entirely build with the Qt classes, so it doesn't support codecs on its own; It's limited to codecs that you have installed already, but there's a plan to bring FLAC support alongside the player package.
Its main goals are : 

  - <b>Speed</b>
  - <b>Portability</b>
  - <b>Simplicity</b>
  - <b>Customization</b>
  
 
 With those in mind, I'm trying to do a player which can possibly be the daily-driver of most people who just start the player and let the music play while they work.

##Features
 As of 10/03/15, I decided to make this project public on GitHub. Right now it features : 
 - <b>Fast-loading from a base music folder</b>
 
 - Playlist basic functions (Random play, shuffle playlist, loop...)
 
 - <b>Always on top toggle </b>
 
 - Setup for quick config at first run
 
 - Album art via Tag Viewer or thumbnail on Playlist
 
 - 1.0 x, 0.5x, 2.0x playback
 
 - <b>Skinning ! </b>
 
 And more coming soon...
 
##Downloads

A pre-compiled binary for windows is available [here](http://goo.gl/QiW88m) (build 140315)
There will be more links available for other OS when I'll be reaching a version which seems stable enough and meeting certain criterias I fixed.

## How to compile

If you want to compile neuPlayer manually you just need [Qt](https://www.qt.io/) (version 5.2.0 or above is required)

To compile it, run `qmake` and `make`.
 
  
