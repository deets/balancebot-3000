#!/bin/bash

openspin -Llib $1.spin && sudo ~/software/vc/propeller-hat/software/p1load/p1load $1.binary
