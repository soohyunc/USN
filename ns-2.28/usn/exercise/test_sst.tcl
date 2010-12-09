#####					#####
#####   test on SST sending/receiving	#####
#####					#####

# $Id: test_sst.tcl 1 2009-10-11 22:06:03Z soohyunc $ 

# creat a simulator object
set ns [new Simulator]

# file openings
set output [open trace/out.tr w]
$ns trace-all $output

set nam_out [open trace/out.nam w]
$ns namtrace-all $nam_out

set queue_out [open trace/out.queue w]

# topology definition
set n0 [$ns node]
set n1 [$ns node]

$ns duplex-link $n0 $n1 1Mb 5ms DropTail 

# set SST Tag agent
set tag [new Agent/SstTag]
$ns attach-agent $n0 $tag

# set SST Reader agent
set reader [new Agent/SstRdr]
$ns attach-agent $n1 $reader

# creat FTP from n1 to n3
set ftp [new Application/FTP]
$ftp attach-agent $tag

$ns at 0.3 "$ftp start"
$ns connect $tag $reader

$ns at 5.0 "$ns detach-agent $n0 $tag; $ns detach-agent $n1 $reader"
$ns trace-queue $n1 $n0 $queue_out

# simulation process
$ns at 10.0 "finish"
proc finish {} {
		global ns output nam_out queue_out
		$ns flush-trace
		close $output
		close $nam_out
		close $queue_out

	exec nam trace/out.nam &

	exec perl ../../bin/set_flow_id -s trace/out.tr
	#exec perl ../../bin/getrc -s 2 -d 3 < trace/out.tr > trace/foo
	exec perl ../../bin/getrc -s 1 -d 0 < trace/out.tr > trace/foo
	exec perl ../../bin/raw2xg -s 0.01 -m 90 < trace/foo > trace/temp.rands

	# TEMPORARY TRACE FILES
	# --- NO TEMPORARY FILES AT THE MOMENT 

	exec xgraph -bb -tk -nl -m -x time -y packets -t "SST Protocol Test" trace/temp.rands &
	#exec awk -f test_thru.awk trace/out.queue
	#exec xgraph -P -x time -y rate -t "SST Thruput" trace/test_thru.rands &

	#exec grep cwnd_ trace/temp.tfwc > trace/tfwc_cwnd.tr 
	#exec awk -f tfwc_cwnd.awk trace/tfwc_cwnd.tr
	#exec grep cwnd_ temp.tfwc | awk '{print $4, $3}' > trace/tfwc_cwnd.rands
	#exec xgraph -m -x time -y cwnd -t "TFWC CWND Dynamics" trace/tfwc_cwnd.rands &

	exit 0
}

$ns run
