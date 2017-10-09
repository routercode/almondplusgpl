#!/bin/sh
# RETURN: 
#      ERROR 1 - WFO mode and PKT_BUF for QM enabled.
#      ERROR 2 - Normal mode and PKT_BUF for QM disabled.

WFO_MODE_ID=$1
CUR_QM_INT_BUFF=`fw_printenv QM_INT_BUFF |  awk '{FS="="} {print$2}'`

if  [[ "$WFO_MODE_ID" -lt 18 ]]; then
    if [ $CUR_QM_INT_BUFF -ne 0 ]; then
        echo "PKT_BUF for QM should be disabled."
        exit 1
    fi
else
    if [ "$CUR_QM_INT_BUFF" == "0" ]; then
        echo "PKT_BUF for QM should be enabled."
        exit 2
    fi
fi
