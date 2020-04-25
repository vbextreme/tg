# Terminal Graphics v0.3
Display image and video on terminal<br/>

## State:
* 0.3 only -i for view tgi
* 0.2 aur
* 0.1   start release
* 0.0.0 begin of crazy

## Bug:

Require Developer version of:
EasyFramework<br/>

## To install it:
### arch
```
$ yay -S terminalgraphics
```
### manual build
```
$ meson build
$ cd build
$ ninja
$ sudo ninja install
```

## Doc:
### conversion
tg can display png,jpeg,bmp,svg,gif and all video on your terminal.<br/>
to do this it needs some information which is also difficult to obtain automatically, the software need of 3 principal arguments<br/>
```
-f --fonts
```
this argument set font use on your terminal, for example I use _Source code Pro_, is very hard to get current font used in automatic mode.<br/>
```
-F --fallback
```
this argument set fallbackfont, the fallback font is secondary font used when first font not have some glyph, for example I use _Symbola_.<br/>
```
-s --size
```
this argument set font size.<br/>
```
-c --convert
```
tell software to convert image/video to output.<br/>
```
-i --input
```
an input file.<br>
```
-o --output
```
output file, if no output file is select display image directly on stdout, video not support stdout and need output file.<br/>
```
-a --aspect 
```
select aspect ratio -1 no scale, 0 auto, 1 width, 2 height.<br/>
```
-r --raw
```
tell a software to save image in only text format, this not works with gif or video, with this format can display image with _cat_,<br/>
```
-p --patterns
```
change pattern glyph
```
-S --seek
```
start video at seconds in double value, only for video
### conversion examples
display image.png on terminal.<br/>
```
$ tg --fonts 'Source Code Pro' --fallback 'Symbola' --size 11 --convert --input image.png
```
save image.png on image.txt and display, this works only with images jpeg,gif,svn,png.<br/>
```
$ tg -f 'Source Code Pro' -F 'Symbola' -s 11 -c -i image.png -r -o image.txt
$ cat image.txt
```
save image.png on image.tgi and display.<br/>
```
$ tg -cfFsio 'Source Code Pro' 'Symbola' 11 image.png image.tgi
$ tg -i image.tgi
```
display image with no unicode character
```
$ tg -cfFspi 'Source Code Pro' 'Symbola' 11 ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVXYZ/.,;:!@#%^$&*()_-+=[]{}|' image.png
```

## To uninstall it:
