#@desc Checking that we can create native space ROIs
${TRACTOR} mkroi data/session@FA 50 59 33 Width:3 ROIName:tmp/region
${TRACTOR} which tmp/region eq 1
rm -f tmp/region.*
