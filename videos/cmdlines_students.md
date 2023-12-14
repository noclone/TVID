
# Testing playback of interlaced files with mpv (and sometimes, mplayer)

# useful mpv keyboard shortcuts

|key| action |
|--|--|
|.|next frame| 
|,|prev frame| 
|[|slow down |
|]|speed up|



**Note: please use those shortcuts to get the grasp of what's going on** :) 

# Some simple playback tests on synthetic (non-natural) contents
## with file: bw_numbers
Synthetic interlaced file that shows a picture with a white counter on black background / black counter on white backround. 
The color inverts on each field. The counter increases on each field

bw_numbers NO deinterlacing:
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=no bw_numbers.m2v  # show both top white and bottom black fields as one interlaced frame

# in software:
ffplay -loop 0 -i bw_numbers.m2v
cvlc --loop --vout-filter=deinterlace --deinterlace-mode=weave bw_numbers.m2v
```
bw_numbers show FIRST FIELD only (skip mode):
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=first-field bw_numbers.m2v # show first field only (skip mode)

# in software:
cvlc --loop --vout-filter=d=deinterlace --deinterlace-mode=discard pendulum.m2v

```
bw_numbers show SECOND FIELD only (skip mode):

VDPAU ONLY :( 
```
# in hardware with vdpau (nvidia only):
mplayer -loop 0  -vo vdpau:deint=2 bw_numbers.m2v  # show second field

```
# **<span style="color:red"> EPILEPSY WARNING: THE FOLLOWING COMMAND LINE PLAYS EACH FIELD OF bw_numbers  IN BOB MODE, THE VISUAL RESULT GIVES BLACK AND WHITE FLASHES AS EXPECTED. BUT IT IS VERY BLINKY!  </span>**
bw_numbers BOB WARNING BLINK BLINK:
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=bob bw_numbers.m2v #bob deinterlacing, blinks A LOT!

# in software:
cvlc --loop  --vout-filter=deinterlace --deinterlace-mode=bob bw_numbers.m2v

```

## With file: jaggies
This synthetic file is intendedly made for stressing deinterlacers with a rotaing bar. With bad deinterlacers, staircase effect happens when the bar is almost horizontal. This is because bad deinterlacers cannot compensate well the loss of horizontal resolution per field caused by interlacing. Also, they don't have any memory of what they deinterlace, no motion estimation, etc.

jaggies NO deinterlacing
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=no Jaggies1.m2v # no deinterlacing

# in software:
ffplay -loop 0 -i Jaggies1.m2v
# or:  
cvlc --vout-filter=deinterlace --deinterlace-mode=weave Jaggies1.m2v
```

## With file: pendulum
This synthetic file is intendedly made for stressing deinterlacers, it shows a swinging pendulum and two letters: O and K, each in its field. 
When deinterlaced, the pendulum should be solid, with smooth edges.
The letters O and K MUST be BOTH visible at EACH deinterlaced frame.
 
pendulum deint WEAVE:
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=no pendulum.m2v # no deinterlacing

# in software:
ffplay -loop 0 -i pendulum.m2v
# or:  
cvlc --vout-filter=deinterlace --deinterlace-mode=weave pendulum.m2v
```

pendulum deint FIRST FIELD:
```
# in hardware with vdpau (nvidia only): 
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=first-field pendulum.m2v # show first field only ("O")

# in software:
cvlc --loop --vout-filter=deinterlace --deinterlace-mode=discard pendulum.m2v
```
pendulum deint SECOND FIELD:
VDPAU ONLY :(
```
# in hardware with vdpau (nvidia only):
mplayer -loop 0  -vo vdpau:deint=2 pendulum.m2v  # shows second field  only ("K")        
```


# Deinterlacing quality comparisons 

**Reminder**: Explore the following command lines with pause and step one frame back/forward.
##  BOB failures

With synthetic contents, BOB is particularly bad because it is the simplest deinterlacer there is. But it is very economical.

Jaggies deint BOB:
``` 
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=bob Jaggies1.m2v # When the bar gets horizontal, a lot of staircase effect happens. Also, line tweeting on intro screen. Fails the test.

# in software:
cvlc --loop  --vout-filter=deinterlace --deinterlace-mode=bob Jaggies1.m2v
```

pendulum deint BOB:
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=bob pendulum.m2v  # Some staircase effect on pendulum edges, and the worst part: O and K are blinking at each field. Fails the test. 

# in software:
cvlc --loop  --vout-filter=deinterlace --deinterlace-mode=bob pendulum.m2v
```


## Better adaptive deinterlacing (sometimes): Yadif

This is a well-known FFMPEG software scaler, known for its high quality and high cpu usage.
 
Jaggies deint YADIFX2:
```
# in software:
mpv --vf=yadif=mode=send_field:parity=auto:deint=all Jaggies1.m2v #much better in every aspect. see how the rotating bar edges are smoothed even at very low horizontal angles. of course, this is computationally more expensive 

# or
ffplay -loop 0 -vf yadif=3 Jaggies1.m2v
``` 

pendulum deint W3FDIF BAD:
```
# in software:
ffplay -loop 0 -vf w3fdif pendulum.m2v
```
# in software:
pendulum deint bwdif BAD SEE OK BLINKING:
```
# in software:
ffplay -loop 0 -vf bwdif pendulum.m2v
```

pendulum deint YADIFX2 GOOD:
```
# in software: 
mpv --vf=yadif=mode=send_field:parity=auto:deint=all pendulum.m2v #much better but computationnaly expensive. YET, IT FAILS THE O K TEST!!
# or:
ffplay -loop 0 -vf yadif=3 pendulum.m2v
```

## Disadvantages of adaptive deinterlacing

Adaptive deinterlacing doesn't work too well on synthetic contents because they account for "smooth" changes between fields. When those changes are too harsh (unnatural), they cannot do much more.

For example:
bw_numbers deint YADIFX2 or hardware temporal or hardware temporal+spatial BAD:
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=temporal bw_numbers.m2v #see the mess on numbers by pausing video and moving frame by frame!

# in software: 
ffplay -loop 0 -vf yadif=3 bw_numbers.m2v
```

```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=temporal-spatial bw_numbers.m2v  #see the mess on numbers by pausing video and moving frame by frame!
```



# Deinterlacing on natural stream: lci

This is a raw capture from French channel LCI over digital terrestrial TV. It is a 720i50 video stream.  

Check the results from the following command lines. Pay  close attention to text quality in the scrolling news ticker, also to the static graphics edges that are in contact with moving people.
You might see some interesting artifacts.

**Reminder**: Explore with pause and step one frame back/forward.

lci deint BOB
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=bob lci.m2v #bob

# in software:
cvlc --loop  --vout-filter=deinterlace --deinterlace-mode=bob lci.m2v
```
lci deint VDPAU TEMPORAL (NVIDIA ONLY)
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=temporal lci.m2v # adaptive in hardware
```

lci deint W3FDIF AVERAGE (a bit bobbing but text ok) :
```
# in software:
ffplay -loop 0 -vf w3fdif pendulum.m2v
```

lci deint YADIFX2 AVERAGE (text a bit smeared!! interesting)
```
# in hardware with vdpau (nvidia only):
mpv --vf=yadif=mode=send_field:parity=auto:deint=all lci.m2v  #top of the pop deinterlacer, in software.

# in software:
ffplay -loop 0 -vf yadif=3 lci.m2v
```
lci deint BWDIF TOP OF THE POP IN THIS CASE ( text ok) :
```
# in software:
ffplay -loop 0 -vf bwdif lci.m2v
```



## Hardware Adaptive deinterlacing : temporal
**note**: mpv calls it "temporal" but this is a comparable implementation of the spatial deinterlacing algorithm I explained in class, which looks at differences/errors for each zones between fields {T0, T1} and  {B0 and B1}, and finally decides to bob-deinterlace or weave **T1** zones.
Jaggies deint VDPAU TEMPORAL
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=temporal Jaggies1.m2v #better than bob, but still far from perfect on horizontal edges
```
pendulum deint VDPAU TEMPORAL
```
# in hardware with vdpau (nvidia only):
mpv --gpu-context=x11 -loop --vf=vdpaupp=deint=yes:deint-mode=temporal pendulum.m2v #better than bob, but far from perfect on the pendulum edges, they are still kind of blocky.

```
**Note**: 'temporal-spatial' deinterlacer doesn't give better results.


