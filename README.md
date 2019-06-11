# ConMon (Undone)

[![LICENSE](https://img.shields.io/badge/license-LGPL--3.0-blue.svg)](https://github.com/MiroKaku/ConMon/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)

This is a demonstration version of how to monitoring Windows console (starting from Windows 8). The concept was based on the 2-part articles of fireeye blog but source code wasn't revealed, so i decide to write a POC for my own. Let me know if you want some more features.

The 2-part articles of fireeye blog can be viewed here:

https://www.fireeye.com/blog/threat-research/2017/08/monitoring-windows-console-activity-part-one.html https://www.fireeye.com/blog/threat-research/2017/08/monitoring-windows-console-activity-part-two.html

## Requirement

* VS2019
* WDK10.0.18362

## TODO

* Process and Message Association
* Handling `IOCTL_CONDRV_READ_IO` Function

## Bug

* `\Device\ConMon` Can't be deleted.

## Reference

https://github.com/EyeOfRa/WinConMon


## Screenshot

![Screenshot](https://github.com/MiroKaku/ConMon/raw/master/Sample.jpg)
