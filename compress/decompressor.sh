#!/bin/bash
tail -c +104 $0 > /tmp/asd.xz;cd /tmp;xz -d asd.xz;chmod u+x asd;./asd && rm ./asd;exit 0;
