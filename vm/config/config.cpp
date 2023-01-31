#include "vmconfig/config.hpp"

MajesticConsoleConfig consoleConfig = {

    .address =              10000,
    .consoleInFD =          0,
    .consoleOutFD =         1,
    .graphicsPixelOutFD =   3,
    .graphicsScreenOutFD =  4,
    .configuratonInFD =     5,
    .userInfoOutFD =        6,

};


RAMConfig ramConfig = {

    .address =  4096,
    .size =     4096,

};


ROMConfig romConfig = {

    .address =  0,
    .size =     4096,

};

VMConfig vmConfig = {

    .attachConsole =    true,
    .attachRAM =        true,
    .attachROM =        true,

};
