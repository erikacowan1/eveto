#!/bin/bash -v

./cbc_eveto \
    --gps-start-time 1096275690 \
    --gps-end-time 1096290950 \
    --detector L1 \
    --safe-channel-file safe_channels.txt \
    --cbc-trigger-database l1-lock-clustered.sqlite \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /home/dbrown/public_html/detchar/omicron/L1/triggers/  \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --omicron-start-time 1096275690 \
    --omicron-end-time 1096290950 \
    --output-directory . \
    --sig-threshold 5.0 \
    --dumb-veto-window 3.0 \
    --max-rounds 3 \
    --verbose
