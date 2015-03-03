#!/bin/bash -v

./cbc_eveto \
    --gps-start-time 1096275690 \
    --gps-end-time 1096290950 \
    --detector L1 \
    --safe-channel-file safe_channel_list.txt \
    --cbc-trigger-database /Users/dbrown/Documents/Research/detchar/eveto/l1-lock-clustered.sqlite \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /Users/dbrown/Documents/Research/detchar/triggers \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --output-directory . \
    --verbose
