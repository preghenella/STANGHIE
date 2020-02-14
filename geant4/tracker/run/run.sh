#! /usr/bin/env bash

../bin/tracker tracker.mac | tee tracker.log
hadd -f e-.root     e-.*.root     && rm e-.*.root
hadd -f mu-.root    mu-.*.root    && rm mu-.*.root
hadd -f pi+.root    pi+.*.root    && rm pi+.*.root
hadd -f kaon+.root  kaon+.*.root  && rm kaon+.*.root
hadd -f proton.root proton.*.root && rm proton.*.root

hadd -f all.root e-.root mu-.root pi+.root kaon+.root proton.root
