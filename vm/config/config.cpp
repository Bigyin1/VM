#include "vmconfig/config.hpp"

const MajesticConsoleConfig consoleConfig = {

    .address =              10000,
    .consoleInFD =          0,
    .consoleOutFD =         1,
    .graphicsPixelOutFD =   3,
    .graphicsScreenOutFD =  4,
    .configuratonInFD =     5,
    .userInfoOutFD =        6,

};


const RAMConfig ramConfig = {

    .address =  4096,
    .size =     4096,

};


const ROMConfig romConfig = {

    .address =  0,
    .size =     4096,

};
