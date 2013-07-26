cubieboard_3.0.42
=================

cubieboard_3.0.42


crate for cubieboard image debian_wheezy_armhf_v1_mele.img 

if you installed the debian_wheezy_armhf_v1_mele.img ,and then you want to add some drivers for your special 
devices, you will need this kernel source code.

build script:
./cubieboard_build.sh


if you want to add your own drivers ,you need to make menuconfig then save your config to 
./arch/arm/config/sun4i_defconfig 
and then you can start the build script to make your new kernel image and modules.

after build success, the output file will be stored in dir ./output .

then you can copy the uImage and lib dir to your cubieboard system,
then folow below steps:


## mount /dev/mmcblk0p1 /mnt
## cp ./uImage /mnt/
## umount /mnt
## cp -a ./lib /
## chown root:root -R /lib/modules


Enjoy your new cubieboard systems!

Thanks,
Bob
