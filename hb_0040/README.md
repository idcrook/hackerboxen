
#


## project page
https://www.instructables.com/id/HackerBox-0040-PIC-of-Destiny/

### installation links


 - MPLAB X IDE
   - https://www.microchip.com/mplab/mplab-x-ide
   - 32-bit libraries info : http://microchipdeveloper.com/install:mplabx-lin64


```
cd ~/projects/hackerbox/hb_0040
tar xvf ./MPLABX-v5.15-linux-installer.tar
sudo ./MPLABX-v5.15-linux-installer.sh
64 Bit, check libraries
Check for 32 Bit libraries
These 32 bit libraries were not found and are needed for MPLAB X to run:
libexpat.so
libX11.so
libXext.so
```

install the required 32-bit packages:

```
sudo apt-get install libx11-6:i386 libxext6:i386 libexpat1:i386
```


### XC8 compiler


https://www.microchip.com/mplab/compilers

Download XC8 for Linux

```
chmod +x ./xc8-v2.05-full-install-linux-installer.run
sudo ./xc8-v2.05-full-install-linux-installer.run
```


## get ready

http://microchipdeveloper.com/mplabx:installation

```
sudo systemctl edit systemd-udevd
```

contents `override.conf`

```
[Service]

IPAddressAllow=localhost
```

reload and check (should show `override.conf` has been loaded)
```
sudo systemctl daemon-reload
sudo systemctl status systemd-udevd
```


## launch


The executables reside in: `/opt/microchip/mplabx/mplab_ide/bin`
not exactly

```
ls /opt/microchip/mplabx/v5.15/mplab_platform/bin/
/opt/microchip/mplabx/v5.15/mplab_platform/bin/mplab_ide

```
