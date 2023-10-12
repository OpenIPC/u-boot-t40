#!/bin/sh -e

export ARCH=mips
export CROSS_PATH="../../resource/toolchain/gcc_720/mips-linux-gnu-ingenic-gcc7.2.0-glibc2.29-fp64/bin"
export CROSS_COMPILE="$(readlink -f $CROSS_PATH)/mips-linux-gnu-"

declare -A cfg
cfg[t40n]="isvp_t40n_sfcnor"
cfg[t40n-nand]="isvp_t40n_sfcnand"
cfg[t40xp]="isvp_t40xp_sfcnor"
cfg[t40xp-nand]="isvp_t40xp_sfcnand"
cfg[t40a]="isvp_t40a_sfcnor"
cfg[t40a-nand]="isvp_t40a_sfcnand"

OUTPUTDIR="${HOME}/uboot"
mkdir -p ${OUTPUTDIR} 2>&1 > /dev/null

for soc in "${!cfg[@]}" ;do

make distclean
make ${cfg[$soc]}
make -j`nproc`

cp u-boot-with-spl.bin ${OUTPUTDIR}/u-boot-${soc}-universal.bin

done
