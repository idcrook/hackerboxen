
RTL88x2 Wifi/USB Adaptor USB 3.0 AC1200

0bda:b812

Chipset:
  idVendor           0x0bda Realtek Semiconductor Corp.
  idProduct          0xb812

`rtl8812bu`?

```
[  575.800051] usb 3-1: new high-speed USB device number 2 using xhci_hcd
[  575.948424] usb 3-1: New USB device found, idVendor=0bda, idProduct=b812
[  575.948429] usb 3-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  575.948432] usb 3-1: Product: USB3.0 802.11ac 1200M Adapter
[  575.948435] usb 3-1: Manufacturer: Realtek
[  575.948437] usb 3-1: SerialNumber: 123456
```

Packaging has 8811CU/8812BU on label

## Building from driver source

https://askubuntu.com/a/1067500

```
git clone https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.3.1_27678.20180430_COEX20180427-5959
sudo dkms add ./rtl88x2BU_WiFi_linux_v5.3.1_27678.20180430_COEX20180427-5959
sudo dkms install -m rtl88x2bu -v 5.3.1
sudo modprobe -v 88x2bu
```

Driver works

dmesg warns about fuse error and a lot of diagnostics



### driver source version archives

- https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.2.4.1_22719_COEX20170518-4444.20170613
- https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.2.4.4_25643.20171212_COEX20171012-5044
- https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.3.1_27678.20180430_COEX20180427-5959
- https://github.com/ulli-kroll/rtl8822bu
