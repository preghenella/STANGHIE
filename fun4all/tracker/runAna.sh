#! /usr/bin/env bash

nevents=10000

for field in 0.2; do
    for eta in 0.; do
	for pipe in 1.6 2.9; do
	    for pid in 11 211 321 2212; do
		tagname="pid=${pid}_pipe=${pipe}_field=${field}_eta=${eta}"
		root.exe -b -q "runAna.C(${pid} , ${pipe} , ${field} , ${eta} , \"${tagname}\", $nevents)"
	    done
	done
    done
done
