# LTM-Telemetry-OLED
LTM Telemetry OLED display (for Turnigy 9x, other transmitters or standalone device).

LTM is Light TeleMetry protocol for slow baudrates and therefore long range. Info on LTM here: https://github.com/iNavFlight/inav/wiki/Lightweight-Telemetry-(LTM)

This project is a  LTM OLED reader for INAV. I have added an small OLED screen to my Turnigy 9x, showing all LTM telemetry data in six pages. Some of the data is calculated from LTM reads, as distance to home. I can see RSSI, number of packets arrived ok or bad, altitude, coordinates...

For Turnigy 9x this OLEDs are great, because they fit by removing the small rectangle in the front (see photos).

Now I can fly in third view no-fpv with full data from the plane. Also is useful when FPVing as GPS beacon for search lost models. Or for other way for debugging in-flight problems.

Some photos of the hack (it is an arduino nano decoding the LTM from a openlrsng TX 433 módule) here:

https://www.rcgroups.com/forums/showpost.php?p=37011233&postcount=11930



![Alt text](https://s1.postimg.org/cu9aul53z/IMG_20170303_161406.jpg)


![Alt text](https://s1.postimg.org/i4e9lvpcv/IMG_20170303_160109.jpg)


![Alt text](https://s1.postimg.org/wwedzh5wf/IMG_20170303_154720.jpg)








