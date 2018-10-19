#!/bin/bash
# Arguments
# Random seed number
SEED=20

# Throughput
TP1="5"

# Latency
L1="30"

# LOSS RATE
LOSS=0.0001

# Buffers
RCV_BUFFER="1MiB"
RCV_BUFFER_ARGV=1073741824
SND_BUFFER="1MiB"
SND_BUFFER_ARGV=1073741824

# Segment size(fixed value)
SEGMENT_SIZE_ARGV=512
SEGMENT_SIZE="512Byte"

DIRNAME=$TP1'Mbps-'$L1'ms-'$j'-'$RCV_BUFFER'-'$SEED

echo "$DIRNAME2 start."

./waf --run "scratch/single-path_mptcp --DATARATE1=$TP1'Mbps' --DELAY1=$L1'ms' --PACKET_LOSS_1=$LOSS --SEED_NUMBER=$SEED" > $DIRNAME.log 2>&1

echo "Done."
