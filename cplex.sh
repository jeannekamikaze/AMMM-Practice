#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/marcct/Applications/cplex/opl/bin/x86_sles10_4.1/
/home/marcct/Applications/cplex/opl/bin/x86_sles10_4.1/oplrun $1 $2 > log.txt