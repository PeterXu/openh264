/*
 *  openh264.cpp
 *  openh264
 *
 *  Created by Peter on 11/26/16.
 *  Copyright © 2016 com.cisco. All rights reserved.
 *
 */

#include <iostream>
#include "openh264.hpp"
#include "openh264Priv.hpp"

void openh264::HelloWorld(const char * s)
{
    openh264Priv *theObj = new openh264Priv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void openh264Priv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

