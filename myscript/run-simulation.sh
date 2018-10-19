#! /bin/bash
# Arguments
# Random seed number
SEED_NUMBER=10

# Throughputs
TP1=5
TP2=5

# Latencies
L1=20
L2=20

# Packet loss rates
LOSS_RATE1=0.000
LOSS_RATE2=0.000

# Buffer sizes
RCV_BUFFER="1MiB"
RCV_BUFFER_ARGV=1073741824
SND_BUFFER="1MiB"
SND_BUFFER_ARGV=1073741824

# Segment size(fixed value)
SEGMENT_SIZE_ARGV=512

TP1=$TP1'Mbps'
TP2=$TP2'Mbps'
L1=$L1'ms'
L2=$L2'ms'
SEGMENT_SIZE=$SEGMENT_SIZE_ARGV'512Byte'

#Simulation main process
echo 'START MPTCP Simulation in ns-3!'

FILENAME=$TP1'-'$TP2'-'$L1'-'$L2'-'$LOSS_RATE1'-'$LOSS_RATE2'-'$RCV_BUFFER'-'$SND_BUFFER'-'$SEGMENT_SIZE'-'$SEED_NUMBER
echo 'Simulating pattern => ' $FILENAME

./waf --run="scratch/mptcp-delay-loss-simulation --DATARATE1=$TP1 --DATARATE2=$TP2 --DELAY1=$L1 --DELAY2=$L2 --PACKET_LOSS_1=$LOSS_RATE1 --PACKET_LOSS_2=$LOSS_RATE2 --SEED_NUMBER=$SEED_NUMBER --PACKET_SIZE=$SEGMENT_SIZE_ARGV --send_buffersize=$SND_BUFFER_ARGV --receive_buffersize=$RCV_BUFFER_ARGV" > $FILENAME.log 2>&1

echo 'Simulation has done!'
