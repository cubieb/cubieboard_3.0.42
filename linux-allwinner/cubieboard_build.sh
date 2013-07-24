# !/bin/sh


# # cp sun4i_defconfig to .config
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- sun4i_defconfig
# # build kernel and driver modules
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16 uImage modules
# # cp kernel and drivers to output dir
cp -f ./arch/arm/boot/uImage ./output/
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=output modules_install
