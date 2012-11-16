#!/bin/sh
# Download linux docbook from
# URL: http://tali.admingilde.org/linux-docbook/

mkdir linux-docbook/
cd linux-docbook/
axel -a http://tali.admingilde.org/linux-docbook/deviceiobook.pdf
axel -a http://tali.admingilde.org/linux-docbook/gadget.pdf
axel -a http://tali.admingilde.org/linux-docbook/journal-api.pdf
axel -a http://tali.admingilde.org/linux-docbook/kernel-api.pdf
axel -a http://tali.admingilde.org/linux-docbook/kernel-hacking.pdf
axel -a http://tali.admingilde.org/linux-docbook/kernel-locking.pdf
axel -a http://tali.admingilde.org/linux-docbook/libata.pdf
axel -a http://tali.admingilde.org/linux-docbook/librs.pdf
axel -a http://tali.admingilde.org/linux-docbook/lsm.pdf
axel -a http://tali.admingilde.org/linux-docbook/mcabook.pdf
axel -a http://tali.admingilde.org/linux-docbook/mtdnand.pdf
axel -a http://tali.admingilde.org/linux-docbook/procfs-guide.pdf
axel -a http://tali.admingilde.org/linux-docbook/rapidio.pdf
axel -a http://tali.admingilde.org/linux-docbook/sis900.pdf
axel -a http://tali.admingilde.org/linux-docbook/usb.pdf
axel -a http://tali.admingilde.org/linux-docbook/videobook.pdf
axel -a http://tali.admingilde.org/linux-docbook/wanbook.pdf
axel -a http://tali.admingilde.org/linux-docbook/writing_usb_driver.pdf
axel -a http://tali.admingilde.org/linux-docbook/z8530book.pdf

exit
