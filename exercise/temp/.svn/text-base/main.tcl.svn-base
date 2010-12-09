proc printMaxSeqno {max} {
	for {set i 0} {$i < $max} {incr i} {
		global sink$i
		
		set result [eval "\$sink$i set maxseen_"]
		puts "\n\[MAX_SEQNO\] $i $result"
	}
}

proc printMaxAck {max} {
	for {set i 0} {$i < $max} {incr i} {
        global tcp$i
                                                                                                   
        set result [eval "\$tcp$i set ack_"]
        puts "\[MAX_ACKNO\] $i $result"
    }

}

proc printThroughput {max} {
	global overall_bandwidth
    puts "\[TOTAL_THROUGPUT\] $overall_bandwidth"
}

proc BuildTopology {} {
	global total_nodes topology node_ sender_nodes receiver_nodes

	set x_space 200
	set y_space 100
	
	for {set i 0} {$i < $sender_nodes} {incr i} {
		$node_($i) set X_ $x_space
	    $node_($i) set Y_ $y_space
    	$node_($i) set Z_ 0.0
		set x_space [expr $x_space + 200]
	}

	set temp [expr $sender_nodes + $receiver_nodes]
	for {set i $temp} {$i < $total_nodes} {incr i} {
		$node_($i) set X_ $x_space
		$node_($i) set Y_ $y_space
		$node_($i) set Z_ 0.0
		set x_space [expr $x_space + 200]
	}

	for {set i $sender_nodes} {$i< $temp} {incr i} {
		$node_($i) set X_ $x_space
   		$node_($i) set Y_ $y_space
   		$node_($i) set Z_ 0.0
		set x_space [expr $x_space + 200]
	}
}

proc BuildConnection {} {
	global connection_type packet_size window_size send_rate my_function sender_nodes receiver_nodes

	set fileId [open ./connection_scenario w]		
	set openId [open $my_function r]
	
	if {$connection_type == "tcp"} {
		puts $fileId "Agent/TCP set packetSize_ $packet_size"
		puts $fileId "Application/FTP set packetSize_ $packet_size"

		for {set i 0} {$i < $sender_nodes} {incr i} {
			puts $fileId "set tcp$i \[new Agent/TCP\]"
		}	

		for {set i 0} {$i < $receiver_nodes} {incr i} {
            puts $fileId "set sink$i \[new Agent/TCPSink\]"
        }	

		for {set i 0} {$i < $sender_nodes} {incr i} {
			puts $fileId "set ftp$i \[new Application/FTP\]"
		}

		for {set i 0} {$i < $sender_nodes} {incr i} {
			 puts $fileId "\$ns_ attach-agent \$node_($i) \$tcp$i"
		}

		set j $sender_nodes
		
		for {set i 0} {$i < $receiver_nodes} {incr i} {
			puts $fileId "\$ns_ attach-agent \$node_($j) \$sink$i"
			set j [expr $j + 1]
		}

		set r_id 0
		set time 300
		set times 100
		for {set i 0} {$i < $sender_nodes} {incr i} {
			if { $r_id < $sender_nodes } {
				puts $fileId "\$ns_ connect \$tcp$r_id \$sink0"
				set r_id [expr $r_id + 1]
			}
			puts $fileId "\$ftp$i attach-agent \$tcp$i"
			puts $fileId "\$ns_ at $times \"\$ftp$i start\""
		}

		foreach line [split [read $openId] \n] {
			puts $fileId "$line"
		}
	} elseif {$connection_type == "udp"} {
		puts $fileId "Agent/UDP set packetSize_ $packet_size"
        puts $fileId "Application/Traffic/CBR set packetSize_ $packet_size"
	
		for {set i 0} {$i < $sender_nodes} {incr i} {
            puts $fileId "set udp$i \[new Agent/UDP\]"
        }				

		for {set i 0} {$i < $receiver_nodes} {incr i} {
            puts $fileId "set sink$i \[new Agent/LossMonitor\]"
        }

		for {set i 0} {$i < $sender_nodes} {incr i} {
            puts $fileId "set cbr$i \[new Application/Traffic/CBR\]"
        }	

		 for {set i 0} {$i < $sender_nodes} {incr i} {
             puts $fileId "\$ns_ attach-agent \$node_($i) \$udp$i"
        }		

		set j $sender_nodes
                                                                                                                            
        for {set i 0} {$i < $receiver_nodes} {incr i} {
            puts $fileId "\$ns_ attach-agent \$node_($j) \$sink$i"
            set j [expr $j + 1]
        }

		set r_id 0
        for {set i 0} {$i < $sender_nodes} {incr i} {
            if { $r_id < $sender_nodes } {
                puts $fileId "\$ns_ connect \$udp$r_id \$sink0"
                set r_id [expr $r_id + 1]
            }
            puts $fileId "\$cbr$i attach-agent \$udp$i"
            puts $fileId "\$ns_ at 100 \"\$cbr$i start\""
        }		

		foreach line [split [read $openId] \n] {
            puts $fileId "$line"
        }		
	}

	close $fileId
}

set connection_type ""
set total_nodes 0
set my_function ""
set sender_nodes 0
set receiver_nodes 0
set packet_size 0
set window_size 0
set speed_file ""
set send_rate 0
set topology ""
set duration 0

set number_argument $argc

if {$number_argument % 2 != 0} {
	puts "Invalid arguments given!!"
	exit 0
}

for {set i 0} {$i < $number_argument} {incr i} {
	set current [lindex $argv $i]
	
	if {$current == "-t"} {
		set connection_type [lindex $argv [expr $i+1]]	
	} elseif {$current == "-n"} {
		set total_nodes [lindex $argv [expr $i+1]]
	} elseif {$current == "-s"} {
		set my_function [lindex $argv [expr $i+1]]
	} elseif {$current == "-p" } {
		set packet_size [lindex $argv [expr $i+1]]
	} elseif {$current == "-w"} {
		set window_size [lindex $argv [expr $i+1]]
	} elseif {$current == "-f"} {
		set speed_file [lindex $argv [expr $i+1]]
	} elseif {$current == "-s"} {
		set send_rate [lindex $argv [expr $i+1]]
	} elseif {$current == "-z"} {
		set topology [lindex $argv [expr $i+1]]
	} elseif {$current == "-d"} {
		set duration [lindex $argv [expr $i+1]]
	} elseif {$current == "-ns"} {
		set sender_nodes [lindex $argv [expr $i+1]]
	} elseif {$current == "-nr"} {
		set receiver_nodes [lindex $argv [expr $i+1]]
	}
}

set val(chan)       Channel/WirelessChannel
set val(prop)       Propagation/TwoRayGround
set val(netif)      Phy/WirelessPhy
set val(mac)        Mac/802_11
#set val(ifq)        Queue/DropTail/PriQueue
set val(ifq)        CMUPriQueue
set val(ll)         LL
set val(ant)        Antenna/OmniAntenna
set val(x)              $topology
set val(y)              $topology
set val(ifqlen)         40            ;# max packet in ifq
set val(seed)           0.0
set val(adhocRouting)   DSR
set val(nn)             $total_nodes
set val(cp)             "./connection_scenario"
set val(stop)           $duration

set ns_         [new Simulator]                                                      
set topo        [new Topography]
set tracefd     [open wireless-ftp-1-out.tr w]
set namtrace    [open wireless-ftp-1-out.nam w]
set rate		[open rate.tr w]
#set rate_1		[open rate_1.tr w]
set event       [open event-1.tr w]
set awnd_event [open awnd_event.tr w]
#set own_event_1 [open own-event-5-1.tr w]

set overall_bandwidth 0

$ns_ use-newtrace
$ns_ trace-all $tracefd
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)
$ns_ eventtrace-all $event

$topo load_flatgrid $val(x) $val(y)

set god_ [create-god $val(nn)]

$ns_ node-config -adhocRouting $val(adhocRouting) \
                 -llType $val(ll) \
                 -macType $val(mac) \
                 -ifqType $val(ifq) \
                 -ifqLen $val(ifqlen) \
                 -antType $val(ant) \
                 -propType $val(prop) \
                 -phyType $val(netif) \
                 -channelType $val(chan) \
                 -topoInstance $topo \
                 -agentTrace ON \
                 -routerTrace ON \
                 -macTrace OFF \
				 -ifqTrace OFF

for {set i 0} {$i < $val(nn) } {incr i} {
        set node_($i) [$ns_ node]
#        $node_($i) random-motion 0
}

BuildConnection

source $val(cp)
#source "./build"

BuildTopology

for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop).01 "$node_($i) reset"
}
  
#$ns_ at $val(stop).02 "printMaxSeqno $receiver_nodes"
#$ns_ at $val(stop).02 "printMaxAck $sender_nodes"
#$ns_ at $val(stop).02 "printThroughput $receiver_nodes"

$ns_ at  $val(stop).02 "puts \"NS EXITING...\" ; $ns_ halt"


#$ns_ at 110 "$node_(0) setdest 100.31231 500.321321 10.2324"

puts $tracefd "M 0.0 nn $val(nn) x $val(x) y $val(y) rp $val(adhocRouting)"
#puts $tracefd "M 0.0 sc $val(sc) cp $val(cp) seed $val(seed)"
puts $tracefd "M 0.0 prop $val(prop) ant $val(ant)"
                                                                                                                             
puts "Starting Simulation..."
$ns_ run
