static char code[] = "\n\
if {[info commands package] == \"\"} {\n\
error \"version mismatch: library\\nscripts expect Tcl version 7.5b1 or later but the loaded version is\\nonly [info patchlevel]\"\n\
}\n\
package require -exact Tcl 8.4\n\
\n\
\n\
if {![info exists auto_path]} {\n\
if {[info exists env(TCLLIBPATH)]} {\n\
set auto_path $env(TCLLIBPATH)\n\
} else {\n\
set auto_path \"\"\n\
}\n\
}\n\
namespace eval tcl {\n\
variable Dir\n\
if {[info library] != \"\"} {\n\
foreach Dir [list [info library] [file dirname [info library]]] {\n\
if {[lsearch -exact $::auto_path $Dir] < 0} {\n\
lappend ::auto_path $Dir\n\
}\n\
}\n\
}\n\
set Dir [file join [file dirname [file dirname \\\n\
[info nameofexecutable]]] lib]\n\
if {[lsearch -exact $::auto_path $Dir] < 0} {\n\
lappend ::auto_path $Dir\n\
}\n\
if {[info exists ::tcl_pkgPath]} {\n\
foreach Dir $::tcl_pkgPath {\n\
if {[lsearch -exact $::auto_path $Dir] < 0} {\n\
lappend ::auto_path $Dir\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
if {(![interp issafe]) && [string equal $tcl_platform(platform) \"windows\"]} {\n\
namespace eval tcl {\n\
proc EnvTraceProc {lo n1 n2 op} {\n\
set x $::env($n2)\n\
set ::env($lo) $x\n\
set ::env([string toupper $lo]) $x\n\
}\n\
proc InitWinEnv {} {\n\
global env tcl_platform\n\
foreach p [array names env] {\n\
set u [string toupper $p]\n\
if {![string equal $u $p]} {\n\
switch -- $u {\n\
COMSPEC -\n\
PATH {\n\
if {![info exists env($u)]} {\n\
set env($u) $env($p)\n\
}\n\
trace variable env($p) w \\\n\
[namespace code [list EnvTraceProc $p]]\n\
trace variable env($u) w \\\n\
[namespace code [list EnvTraceProc $p]]\n\
}\n\
}\n\
}\n\
}\n\
if {![info exists env(COMSPEC)]} {\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
set env(COMSPEC) cmd.exe\n\
} else {\n\
set env(COMSPEC) command.com\n\
}\n\
}\n\
}\n\
InitWinEnv\n\
}\n\
}\n\
\n\
\n\
package unknown tclPkgUnknown\n\
\n\
if {![interp issafe]} {\n\
if {[string equal $::tcl_platform(platform) \"unix\"] && \\\n\
[string equal $::tcl_platform(os) \"Darwin\"]} {\n\
package unknown [list tcl::MacOSXPkgUnknown [package unknown]]\n\
}\n\
if {[string equal $::tcl_platform(platform) \"macintosh\"]} {\n\
package unknown [list tcl::MacPkgUnknown [package unknown]]\n\
}\n\
}\n\
\n\
\n\
if {[llength [info commands exec]] == 0} {\n\
\n\
\n\
set auto_noexec 1\n\
}\n\
set errorCode \"\"\n\
set errorInfo \"\"\n\
\n\
\n\
if {[llength [info commands tclLog]] == 0} {\n\
proc tclLog {string} {\n\
catch {puts stderr $string}\n\
}\n\
}\n\
\n\
\n\
proc unknown args {\n\
global auto_noexec auto_noload env unknown_pending tcl_interactive\n\
global errorCode errorInfo\n\
\n\
\n\
set cmd [lindex $args 0]\n\
if {[regexp \"^:*namespace\\[ \\t\\n\\]+inscope\" $cmd] && [llength $cmd] == 4} {\n\
set arglist [lrange $args 1 end]\n\
set ret [catch {uplevel 1 ::$cmd $arglist} result]\n\
if {$ret == 0} {\n\
return $result\n\
} else {\n\
return -code $ret -errorcode $errorCode $result\n\
}\n\
}\n\
\n\
\n\
set savedErrorCode $errorCode\n\
set savedErrorInfo $errorInfo\n\
set name [lindex $args 0]\n\
if {![info exists auto_noload]} {\n\
if {[info exists unknown_pending($name)]} {\n\
return -code error \"self-referential recursion in \\\"unknown\\\" for command \\\"$name\\\"\";\n\
}\n\
set unknown_pending($name) pending;\n\
set ret [catch {auto_load $name [uplevel 1 {::namespace current}]} msg]\n\
unset unknown_pending($name);\n\
if {$ret != 0} {\n\
append errorInfo \"\\n    (autoloading \\\"$name\\\")\"\n\
return -code $ret -errorcode $errorCode -errorinfo $errorInfo $msg\n\
}\n\
if {![array size unknown_pending]} {\n\
unset unknown_pending\n\
}\n\
if {$msg} {\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
set code [catch {uplevel 1 $args} msg]\n\
if {$code ==  1} {\n\
set cinfo $args\n\
set ellipsis \"\"\n\
while {[string bytelength $cinfo] > 150} {\n\
set cinfo [string range $cinfo 0 end-1]\n\
set ellipsis \"...\"\n\
}\n\
append cinfo $ellipsis \"\\\"\\n    (\\\"uplevel\\\" body line 1)\"\n\
append cinfo \"\\n    invoked from within\"\n\
append cinfo \"\\n\\\"uplevel 1 \\$args\\\"\"\n\
set expect \"$msg\\n    while executing\\n\\\"$cinfo\"\n\
if {$errorInfo eq $expect} {\n\
return -code error -errorcode $errorCode $msg\n\
}\n\
set expect \"\\n    invoked from within\\n\\\"$cinfo\"\n\
set exlen [string length $expect]\n\
set eilen [string length $errorInfo]\n\
set i [expr {$eilen - $exlen - 1}]\n\
set einfo [string range $errorInfo 0 $i]\n\
if {$errorInfo ne \"$einfo$expect\"} {\n\
error \"Tcl bug: unexpected stack trace in \\\"unknown\\\"\" {} \\\n\
[list CORE UNKNOWN BADTRACE $expect $errorInfo]\n\
}\n\
return -code error -errorcode $errorCode \\\n\
-errorinfo $einfo $msg\n\
} else {\n\
return -code $code $msg\n\
}\n\
}\n\
}\n\
\n\
if {([info level] == 1) && [string equal [info script] \"\"] \\\n\
&& [info exists tcl_interactive] && $tcl_interactive} {\n\
if {![info exists auto_noexec]} {\n\
set new [auto_execok $name]\n\
if {$new != \"\"} {\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
set redir \"\"\n\
if {[string equal [info commands console] \"\"]} {\n\
set redir \">&@stdout <@stdin\"\n\
}\n\
return [uplevel 1 exec $redir $new [lrange $args 1 end]]\n\
}\n\
}\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
if {[string equal $name \"!!\"]} {\n\
set newcmd [history event]\n\
} elseif {[regexp {^!(.+)$} $name dummy event]} {\n\
set newcmd [history event $event]\n\
} elseif {[regexp {^\\^([^^]*)\\^([^^]*)\\^?$} $name dummy old new]} {\n\
set newcmd [history event -1]\n\
catch {regsub -all -- $old $newcmd $new newcmd}\n\
}\n\
if {[info exists newcmd]} {\n\
tclLog $newcmd\n\
history change $newcmd 0\n\
return [uplevel 1 $newcmd]\n\
}\n\
\n\
set ret [catch {set cmds [info commands $name*]} msg]\n\
if {[string equal $name \"::\"]} {\n\
set name \"\"\n\
}\n\
if {$ret != 0} {\n\
return -code $ret -errorcode $errorCode \\\n\
\"error in unknown while checking if \\\"$name\\\" is a unique command abbreviation: $msg\"\n\
}\n\
if {[llength $cmds] == 1} {\n\
return [uplevel 1 [lreplace $args 0 0 $cmds]]\n\
}\n\
if {[llength $cmds]} {\n\
if {[string equal $name \"\"]} {\n\
return -code error \"empty command name \\\"\\\"\"\n\
} else {\n\
return -code error \\\n\
\"ambiguous command name \\\"$name\\\": [lsort $cmds]\"\n\
}\n\
}\n\
}\n\
return -code error \"invalid command name \\\"$name\\\"\"\n\
}\n\
\n\
\n\
proc auto_load {cmd {namespace {}}} {\n\
global auto_index auto_oldpath auto_path\n\
\n\
if {[string length $namespace] == 0} {\n\
set namespace [uplevel 1 [list ::namespace current]]\n\
}\n\
set nameList [auto_qualify $cmd $namespace]\n\
lappend nameList $cmd\n\
foreach name $nameList {\n\
if {[info exists auto_index($name)]} {\n\
namespace eval :: $auto_index($name)\n\
if {[namespace which -command $name] ne \"\"} {\n\
return 1\n\
}\n\
}\n\
}\n\
if {![info exists auto_path]} {\n\
return 0\n\
}\n\
\n\
if {![auto_load_index]} {\n\
return 0\n\
}\n\
foreach name $nameList {\n\
if {[info exists auto_index($name)]} {\n\
namespace eval :: $auto_index($name)\n\
if {[namespace which -command $name] ne \"\"} {\n\
return 1\n\
}\n\
}\n\
}\n\
return 0\n\
}\n\
\n\
\n\
proc auto_load_index {} {\n\
global auto_index auto_oldpath auto_path errorInfo errorCode\n\
\n\
if {[info exists auto_oldpath] && \\\n\
[string equal $auto_oldpath $auto_path]} {\n\
return 0\n\
}\n\
set auto_oldpath $auto_path\n\
\n\
\n\
set issafe [interp issafe]\n\
for {set i [expr {[llength $auto_path] - 1}]} {$i >= 0} {incr i -1} {\n\
set dir [lindex $auto_path $i]\n\
set f \"\"\n\
if {$issafe} {\n\
catch {source [file join $dir tclIndex]}\n\
} elseif {[catch {set f [open [file join $dir tclIndex]]}]} {\n\
continue\n\
} else {\n\
set error [catch {\n\
set id [gets $f]\n\
if {[string equal $id \\\n\
\"# Tcl autoload index file, version 2.0\"]} {\n\
eval [read $f]\n\
} elseif {[string equal $id \"# Tcl autoload index file: each line identifies a Tcl\"]} {\n\
while {[gets $f line] >= 0} {\n\
if {[string equal [string index $line 0] \"#\"] \\\n\
|| ([llength $line] != 2)} {\n\
continue\n\
}\n\
set name [lindex $line 0]\n\
set auto_index($name) \\\n\
\"source [file join $dir [lindex $line 1]]\"\n\
}\n\
} else {\n\
error \"[file join $dir tclIndex] isn't a proper Tcl index file\"\n\
}\n\
} msg]\n\
if {$f != \"\"} {\n\
close $f\n\
}\n\
if {$error} {\n\
error $msg $errorInfo $errorCode\n\
}\n\
}\n\
}\n\
return 1\n\
}\n\
\n\
\n\
proc auto_qualify {cmd namespace} {\n\
\n\
set n [regsub -all {::+} $cmd :: cmd]\n\
\n\
\n\
\n\
if {[regexp {^::(.*)$} $cmd x tail]} {\n\
if {$n > 1} {\n\
return [list $cmd]\n\
} else {\n\
return [list $tail]\n\
}\n\
}\n\
\n\
\n\
if {$n == 0} {\n\
if {[string equal $namespace ::]} {\n\
return [list $cmd]\n\
} else {\n\
return [list ${namespace}::$cmd $cmd]\n\
}\n\
} elseif {[string equal $namespace ::]} {\n\
return [list ::$cmd]\n\
} else {\n\
return [list ${namespace}::$cmd ::$cmd]\n\
}\n\
}\n\
\n\
\n\
proc auto_import {pattern} {\n\
global auto_index\n\
\n\
\n\
if {![string match *::* $pattern]} {\n\
return\n\
}\n\
\n\
set ns [uplevel 1 [list ::namespace current]]\n\
set patternList [auto_qualify $pattern $ns]\n\
\n\
auto_load_index\n\
\n\
foreach pattern $patternList {\n\
foreach name [array names auto_index $pattern] {\n\
if {([namespace which -command $name] eq \"\")\n\
&& ([namespace qualifiers $pattern] eq [namespace qualifiers $name])} {\n\
namespace eval :: $auto_index($name)\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
if {[string equal windows $tcl_platform(platform)]} {\n\
proc auto_execok name {\n\
global auto_execs env tcl_platform\n\
\n\
if {[info exists auto_execs($name)]} {\n\
return $auto_execs($name)\n\
}\n\
set auto_execs($name) \"\"\n\
\n\
set shellBuiltins [list cls copy date del erase dir echo mkdir \\\n\
md rename ren rmdir rd time type ver vol]\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
lappend shellBuiltins \"start\"\n\
}\n\
if {[info exists env(PATHEXT)]} {\n\
set execExtensions [split \";$env(PATHEXT)\" \";\"]\n\
} else {\n\
set execExtensions [list {} .com .exe .bat]\n\
}\n\
\n\
if {[lsearch -exact $shellBuiltins $name] != -1} {\n\
set cmd $env(COMSPEC)\n\
if {[file exists $cmd]} {\n\
set cmd [file attributes $cmd -shortname]\n\
}\n\
return [set auto_execs($name) [list $cmd /c $name]]\n\
}\n\
\n\
if {[llength [file split $name]] != 1} {\n\
foreach ext $execExtensions {\n\
set file ${name}${ext}\n\
if {[file exists $file] && ![file isdirectory $file]} {\n\
return [set auto_execs($name) [list $file]]\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
set path \"[file dirname [info nameof]];.;\"\n\
if {[info exists env(WINDIR)]} {\n\
set windir $env(WINDIR) \n\
}\n\
if {[info exists windir]} {\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
append path \"$windir/system32;\"\n\
}\n\
append path \"$windir/system;$windir;\"\n\
}\n\
\n\
foreach var {PATH Path path} {\n\
if {[info exists env($var)]} {\n\
append path \";$env($var)\"\n\
}\n\
}\n\
\n\
foreach dir [split $path {;}] {\n\
if {[info exists checked($dir)] || [string equal {} $dir]} { continue }\n\
set checked($dir) {}\n\
foreach ext $execExtensions {\n\
set file [file join $dir ${name}${ext}]\n\
if {[file exists $file] && ![file isdirectory $file]} {\n\
return [set auto_execs($name) [list $file]]\n\
}\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
} else {\n\
proc auto_execok name {\n\
global auto_execs env\n\
\n\
if {[info exists auto_execs($name)]} {\n\
return $auto_execs($name)\n\
}\n\
set auto_execs($name) \"\"\n\
if {[llength [file split $name]] != 1} {\n\
if {[file executable $name] && ![file isdirectory $name]} {\n\
set auto_execs($name) [list $name]\n\
}\n\
return $auto_execs($name)\n\
}\n\
foreach dir [split $env(PATH) :] {\n\
if {[string equal $dir \"\"]} {\n\
set dir .\n\
}\n\
set file [file join $dir $name]\n\
if {[file executable $file] && ![file isdirectory $file]} {\n\
set auto_execs($name) [list $file]\n\
return $auto_execs($name)\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
}\n\
\n\
proc tcl::CopyDirectory {action src dest} {\n\
set nsrc [file normalize $src]\n\
set ndest [file normalize $dest]\n\
if {[string equal $action \"renaming\"]} {\n\
if {[lsearch -exact [file volumes] $nsrc] != -1} {\n\
return -code error \"error $action \\\"$src\\\" to\\\n\
\\\"$dest\\\": trying to rename a volume or move a directory\\\n\
into itself\"\n\
}\n\
}\n\
if {[file exists $dest]} {\n\
if {$nsrc == $ndest} {\n\
return -code error \"error $action \\\"$src\\\" to\\\n\
\\\"$dest\\\": trying to rename a volume or move a directory\\\n\
into itself\"\n\
}\n\
if {[string equal $action \"copying\"]} {\n\
return -code error \"error $action \\\"$src\\\" to\\\n\
\\\"$dest\\\": file already exists\"\n\
} else {\n\
set existing [glob -nocomplain -directory $dest * .*]\n\
eval [list lappend existing] \\\n\
[glob -nocomplain -directory $dest -type hidden * .*]\n\
foreach s $existing {\n\
if {([file tail $s] != \".\") && ([file tail $s] != \"..\")} {\n\
return -code error \"error $action \\\"$src\\\" to\\\n\
\\\"$dest\\\": file already exists\"\n\
}\n\
}\n\
}\n\
} else {\n\
if {[string first $nsrc $ndest] != -1} {\n\
set srclen [expr {[llength [file split $nsrc]] -1}]\n\
set ndest [lindex [file split $ndest] $srclen]\n\
if {$ndest == [file tail $nsrc]} {\n\
return -code error \"error $action \\\"$src\\\" to\\\n\
\\\"$dest\\\": trying to rename a volume or move a directory\\\n\
into itself\"\n\
}\n\
}\n\
file mkdir $dest\n\
}\n\
set filelist [concat [glob -nocomplain -directory $src *] \\\n\
[glob -nocomplain -directory $src -types hidden *]]\n\
\n\
foreach s [lsort -unique $filelist] {\n\
if {([file tail $s] != \".\") && ([file tail $s] != \"..\")} {\n\
file copy $s [file join $dest [file tail $s]]\n\
}\n\
}\n\
return\n\
}\n\
\n\
\n\
namespace eval tcl {\n\
variable history\n\
if {![info exists history]} {\n\
array set history {\n\
nextid	0\n\
keep	20\n\
oldest	-20\n\
}\n\
}\n\
}\n\
\n\
\n\
proc history {args} {\n\
set len [llength $args]\n\
if {$len == 0} {\n\
return [tcl::HistInfo]\n\
}\n\
set key [lindex $args 0]\n\
set options \"add, change, clear, event, info, keep, nextid, or redo\"\n\
switch -glob -- $key {\n\
a* { # history add\n\
\n\
if {$len > 3} {\n\
return -code error \"wrong # args: should be \\\"history add event ?exec?\\\"\"\n\
}\n\
if {![string match $key* add]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 3} {\n\
set arg [lindex $args 2]\n\
if {! ([string match e* $arg] && [string match $arg* exec])} {\n\
return -code error \"bad argument \\\"$arg\\\": should be \\\"exec\\\"\"\n\
}\n\
}\n\
return [tcl::HistAdd [lindex $args 1] [lindex $args 2]]\n\
}\n\
ch* { # history change\n\
\n\
if {($len > 3) || ($len < 2)} {\n\
return -code error \"wrong # args: should be \\\"history change newValue ?event?\\\"\"\n\
}\n\
if {![string match $key* change]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 2} {\n\
set event 0\n\
} else {\n\
set event [lindex $args 2]\n\
}\n\
\n\
return [tcl::HistChange [lindex $args 1] $event]\n\
}\n\
cl* { # history clear\n\
\n\
if {($len > 1)} {\n\
return -code error \"wrong # args: should be \\\"history clear\\\"\"\n\
}\n\
if {![string match $key* clear]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistClear]\n\
}\n\
e* { # history event\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history event ?event?\\\"\"\n\
}\n\
if {![string match $key* event]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 1} {\n\
set event -1\n\
} else {\n\
set event [lindex $args 1]\n\
}\n\
return [tcl::HistEvent $event]\n\
}\n\
i* { # history info\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history info ?count?\\\"\"\n\
}\n\
if {![string match $key* info]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistInfo [lindex $args 1]]\n\
}\n\
k* { # history keep\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history keep ?count?\\\"\"\n\
}\n\
if {$len == 1} {\n\
return [tcl::HistKeep]\n\
} else {\n\
set limit [lindex $args 1]\n\
if {[catch {expr {~$limit}}] || ($limit < 0)} {\n\
return -code error \"illegal keep count \\\"$limit\\\"\"\n\
}\n\
return [tcl::HistKeep $limit]\n\
}\n\
}\n\
n* { # history nextid\n\
\n\
if {$len > 1} {\n\
return -code error \"wrong # args: should be \\\"history nextid\\\"\"\n\
}\n\
if {![string match $key* nextid]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [expr {$tcl::history(nextid) + 1}]\n\
}\n\
r* { # history redo\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history redo ?event?\\\"\"\n\
}\n\
if {![string match $key* redo]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistRedo [lindex $args 1]]\n\
}\n\
default {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistAdd {command {exec {}}} {\n\
variable history\n\
\n\
if {[string trim $command] == \"\"} {\n\
return \"\"\n\
}\n\
\n\
set i [incr history(nextid)]\n\
set history($i) $command\n\
set j [incr history(oldest)]\n\
if {[info exists history($j)]} {unset history($j)}\n\
if {[string match e* $exec]} {\n\
return [uplevel #0 $command]\n\
} else {\n\
return {}\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistKeep {{limit {}}} {\n\
variable history\n\
if {[string length $limit] == 0} {\n\
return $history(keep)\n\
} else {\n\
set oldold $history(oldest)\n\
set history(oldest) [expr {$history(nextid) - $limit}]\n\
for {} {$oldold <= $history(oldest)} {incr oldold} {\n\
if {[info exists history($oldold)]} {unset history($oldold)}\n\
}\n\
set history(keep) $limit\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistClear {} {\n\
variable history\n\
set keep $history(keep)\n\
unset history\n\
array set history [list \\\n\
nextid	0	\\\n\
keep	$keep	\\\n\
oldest	-$keep	\\\n\
]\n\
}\n\
\n\
\n\
proc tcl::HistInfo {{num {}}} {\n\
variable history\n\
if {$num == {}} {\n\
set num [expr {$history(keep) + 1}]\n\
}\n\
set result {}\n\
set newline \"\"\n\
for {set i [expr {$history(nextid) - $num + 1}]} \\\n\
{$i <= $history(nextid)} {incr i} {\n\
if {![info exists history($i)]} {\n\
continue\n\
}\n\
set cmd [string trimright $history($i) \\ \\n]\n\
regsub -all \\n $cmd \"\\n\\t\" cmd\n\
append result $newline[format \"%6d  %s\" $i $cmd]\n\
set newline \\n\n\
}\n\
return $result\n\
}\n\
\n\
\n\
proc tcl::HistRedo {{event -1}} {\n\
variable history\n\
if {[string length $event] == 0} {\n\
set event -1\n\
}\n\
set i [HistIndex $event]\n\
if {$i == $history(nextid)} {\n\
return -code error \"cannot redo the current event\"\n\
}\n\
set cmd $history($i)\n\
HistChange $cmd 0\n\
uplevel #0 $cmd\n\
}\n\
\n\
\n\
proc tcl::HistIndex {event} {\n\
variable history\n\
if {[catch {expr {~$event}}]} {\n\
for {set i [expr {$history(nextid)-1}]} {[info exists history($i)]} \\\n\
{incr i -1} {\n\
if {[string match $event* $history($i)]} {\n\
return $i;\n\
}\n\
if {[string match $event $history($i)]} {\n\
return $i;\n\
}\n\
}\n\
return -code error \"no event matches \\\"$event\\\"\"\n\
} elseif {$event <= 0} {\n\
set i [expr {$history(nextid) + $event}]\n\
} else {\n\
set i $event\n\
}\n\
if {$i <= $history(oldest)} {\n\
return -code error \"event \\\"$event\\\" is too far in the past\"\n\
}\n\
if {$i > $history(nextid)} {\n\
return -code error \"event \\\"$event\\\" hasn't occured yet\"\n\
}\n\
return $i\n\
}\n\
\n\
\n\
proc tcl::HistEvent {event} {\n\
variable history\n\
set i [HistIndex $event]\n\
if {[info exists history($i)]} {\n\
return [string trimright $history($i) \\ \\n]\n\
} else {\n\
return \"\";\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistChange {cmd {event 0}} {\n\
variable history\n\
set i [HistIndex $event]\n\
set history($i) $cmd\n\
}\n\
\n\
\n\
if {[string equal $::tcl_platform(platform) \"windows\"]} {\n\
set tcl_wordchars \"\\\\S\"\n\
set tcl_nonwordchars \"\\\\s\"\n\
} else {\n\
set tcl_wordchars \"\\\\w\"\n\
set tcl_nonwordchars \"\\\\W\"\n\
}\n\
\n\
\n\
proc tcl_wordBreakAfter {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
set str [string range $str $start end]\n\
if {[regexp -indices \"$tcl_wordchars$tcl_nonwordchars|$tcl_nonwordchars$tcl_wordchars\" $str result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_wordBreakBefore {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[string equal $start end]} {\n\
set start [string length $str]\n\
}\n\
if {[regexp -indices \"^.*($tcl_wordchars$tcl_nonwordchars|$tcl_nonwordchars$tcl_wordchars)\" [string range $str 0 $start] result]} {\n\
return [lindex $result 1]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_endOfWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[regexp -indices \"$tcl_nonwordchars*$tcl_wordchars+$tcl_nonwordchars\" \\\n\
[string range $str $start end] result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_startOfNextWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[regexp -indices \"$tcl_wordchars*$tcl_nonwordchars+$tcl_wordchars\" \\\n\
[string range $str $start end] result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_startOfPreviousWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[string equal $start end]} {\n\
set start [string length $str]\n\
}\n\
if {[regexp -indices \\\n\
\"$tcl_nonwordchars*($tcl_wordchars+)$tcl_nonwordchars*\\$\" \\\n\
[string range $str 0 [expr {$start - 1}]] result word]} {\n\
return [lindex $word 0]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
package require Tcl 8.2\n\
package provide http 2.4.5\n\
\n\
namespace eval http {\n\
variable http\n\
array set http {\n\
-accept */*\n\
-proxyhost {}\n\
-proxyport {}\n\
-proxyfilter http::ProxyRequired\n\
}\n\
set http(-useragent) \"Tcl http client package [package provide http]\"\n\
\n\
proc init {} {\n\
variable formMap\n\
variable alphanumeric a-zA-Z0-9\n\
for {set i 0} {$i <= 256} {incr i} {\n\
set c [format %c $i]\n\
if {![string match \\[$alphanumeric\\] $c]} {\n\
set formMap($c) %[format %.2x $i]\n\
}\n\
}\n\
array set formMap { \" \" + \\n %0d%0a }\n\
}\n\
init\n\
\n\
variable urlTypes\n\
array set urlTypes {\n\
http	{80 ::socket}\n\
}\n\
\n\
variable encodings [string tolower [encoding names]]\n\
variable defaultCharset \"iso8859-1\"\n\
\n\
namespace export geturl config reset wait formatQuery register unregister\n\
}\n\
\n\
\n\
proc http::register {proto port command} {\n\
variable urlTypes\n\
set urlTypes($proto) [list $port $command]\n\
}\n\
\n\
\n\
proc http::unregister {proto} {\n\
variable urlTypes\n\
if {![info exists urlTypes($proto)]} {\n\
return -code error \"unsupported url type \\\"$proto\\\"\"\n\
}\n\
set old $urlTypes($proto)\n\
unset urlTypes($proto)\n\
return $old\n\
}\n\
\n\
\n\
proc http::config {args} {\n\
variable http\n\
set options [lsort [array names http -*]]\n\
set usage [join $options \", \"]\n\
if {[llength $args] == 0} {\n\
set result {}\n\
foreach name $options {\n\
lappend result $name $http($name)\n\
}\n\
return $result\n\
}\n\
set options [string map {- \"\"} $options]\n\
set pat ^-([join $options |])$\n\
if {[llength $args] == 1} {\n\
set flag [lindex $args 0]\n\
if {[regexp -- $pat $flag]} {\n\
return $http($flag)\n\
} else {\n\
return -code error \"Unknown option $flag, must be: $usage\"\n\
}\n\
} else {\n\
foreach {flag value} $args {\n\
if {[regexp -- $pat $flag]} {\n\
set http($flag) $value\n\
} else {\n\
return -code error \"Unknown option $flag, must be: $usage\"\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::Finish { token {errormsg \"\"} {skipCB 0}} {\n\
variable $token\n\
upvar 0 $token state\n\
global errorInfo errorCode\n\
if {[string length $errormsg] != 0} {\n\
set state(error) [list $errormsg $errorInfo $errorCode]\n\
set state(status) error\n\
}\n\
catch {close $state(sock)}\n\
catch {after cancel $state(after)}\n\
if {[info exists state(-command)] && !$skipCB} {\n\
if {[catch {eval $state(-command) {$token}} err]} {\n\
if {[string length $errormsg] == 0} {\n\
set state(error) [list $err $errorInfo $errorCode]\n\
set state(status) error\n\
}\n\
}\n\
if {[info exists state(-command)]} {\n\
unset state(-command)\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::reset { token {why reset} } {\n\
variable $token\n\
upvar 0 $token state\n\
set state(status) $why\n\
catch {fileevent $state(sock) readable {}}\n\
catch {fileevent $state(sock) writable {}}\n\
Finish $token\n\
if {[info exists state(error)]} {\n\
set errorlist $state(error)\n\
unset state\n\
eval ::error $errorlist\n\
}\n\
}\n\
\n\
\n\
proc http::geturl { url args } {\n\
variable http\n\
variable urlTypes\n\
variable defaultCharset\n\
\n\
\n\
if {![info exists http(uid)]} {\n\
set http(uid) 0\n\
}\n\
set token [namespace current]::[incr http(uid)]\n\
variable $token\n\
upvar 0 $token state\n\
reset $token\n\
\n\
\n\
array set state {\n\
-binary		false\n\
-blocksize 	8192\n\
-queryblocksize 8192\n\
-validate 	0\n\
-headers 	{}\n\
-timeout 	0\n\
-type           application/x-www-form-urlencoded\n\
-queryprogress	{}\n\
state		header\n\
meta		{}\n\
coding		{}\n\
currentsize	0\n\
totalsize	0\n\
querylength	0\n\
queryoffset	0\n\
type            text/html\n\
body            {}\n\
status		\"\"\n\
http            \"\"\n\
}\n\
array set type {\n\
-binary		boolean\n\
-blocksize	integer\n\
-queryblocksize integer\n\
-validate	boolean\n\
-timeout	integer\n\
}	\n\
set state(charset)	$defaultCharset\n\
set options {-binary -blocksize -channel -command -handler -headers \\\n\
-progress -query -queryblocksize -querychannel -queryprogress\\\n\
-validate -timeout -type}\n\
set usage [join $options \", \"]\n\
set options [string map {- \"\"} $options]\n\
set pat ^-([join $options |])$\n\
foreach {flag value} $args {\n\
if {[regexp $pat $flag]} {\n\
if {[info exists type($flag)] && \\\n\
![string is $type($flag) -strict $value]} {\n\
unset $token\n\
return -code error \"Bad value for $flag ($value), must be $type($flag)\"\n\
}\n\
set state($flag) $value\n\
} else {\n\
unset $token\n\
return -code error \"Unknown option $flag, can be: $usage\"\n\
}\n\
}\n\
\n\
\n\
set isQueryChannel [info exists state(-querychannel)]\n\
set isQuery [info exists state(-query)]\n\
if {$isQuery && $isQueryChannel} {\n\
unset $token\n\
return -code error \"Can't combine -query and -querychannel options!\"\n\
}\n\
\n\
\n\
set exp {^(([^:]*)://)?([^@]+@)?([^/:]+)(:([0-9]+))?(/.*)?$}\n\
if {![regexp -nocase $exp $url x prefix proto user host y port srvurl]} {\n\
unset $token\n\
return -code error \"Unsupported URL: $url\"\n\
}\n\
if {[string length $proto] == 0} {\n\
set proto http\n\
set url ${proto}://$url\n\
}\n\
if {![info exists urlTypes($proto)]} {\n\
unset $token\n\
return -code error \"Unsupported URL type \\\"$proto\\\"\"\n\
}\n\
set defport [lindex $urlTypes($proto) 0]\n\
set defcmd [lindex $urlTypes($proto) 1]\n\
\n\
if {[string length $port] == 0} {\n\
set port $defport\n\
}\n\
if {[string length $srvurl] == 0} {\n\
set srvurl /\n\
}\n\
if {[string length $proto] == 0} {\n\
set url http://$url\n\
}\n\
set state(url) $url\n\
if {![catch {$http(-proxyfilter) $host} proxy]} {\n\
set phost [lindex $proxy 0]\n\
set pport [lindex $proxy 1]\n\
}\n\
\n\
\n\
if {$state(-timeout) > 0} {\n\
set state(after) [after $state(-timeout) \\\n\
[list http::reset $token timeout]]\n\
set async -async\n\
} else {\n\
set async \"\"\n\
}\n\
\n\
\n\
if {[info exists phost] && [string length $phost]} {\n\
set srvurl $url\n\
set conStat [catch {eval $defcmd $async {$phost $pport}} s]\n\
} else {\n\
set conStat [catch {eval $defcmd $async {$host $port}} s]\n\
}\n\
if {$conStat} {\n\
\n\
Finish $token \"\" 1\n\
cleanup $token\n\
return -code error $s\n\
}\n\
set state(sock) $s\n\
\n\
\n\
if {$state(-timeout) > 0} {\n\
fileevent $s writable [list http::Connect $token]\n\
http::wait $token\n\
\n\
if {[string equal $state(status) \"error\"]} {\n\
set err [lindex $state(error) 0]\n\
cleanup $token\n\
return -code error $err\n\
} elseif {![string equal $state(status) \"connect\"]} {\n\
return $token\n\
}\n\
set state(status) \"\"\n\
}\n\
\n\
\n\
fconfigure $s -translation {auto crlf} -buffersize $state(-blocksize)\n\
\n\
\n\
catch {fconfigure $s -blocking off}\n\
set how GET\n\
if {$isQuery} {\n\
set state(querylength) [string length $state(-query)]\n\
if {$state(querylength) > 0} {\n\
set how POST\n\
set contDone 0\n\
} else {\n\
unset state(-query)\n\
set isQuery 0\n\
}\n\
} elseif {$state(-validate)} {\n\
set how HEAD\n\
} elseif {$isQueryChannel} {\n\
set how POST\n\
fconfigure $state(-querychannel) -blocking 1 -translation binary\n\
set contDone 0\n\
}\n\
\n\
if {[catch {\n\
puts $s \"$how $srvurl HTTP/1.0\"\n\
puts $s \"Accept: $http(-accept)\"\n\
if {$port == $defport} {\n\
puts $s \"Host: $host\"\n\
} else {\n\
puts $s \"Host: $host:$port\"\n\
}\n\
puts $s \"User-Agent: $http(-useragent)\"\n\
foreach {key value} $state(-headers) {\n\
set value [string map [list \\n \"\" \\r \"\"] $value]\n\
set key [string trim $key]\n\
if {[string equal $key \"Content-Length\"]} {\n\
set contDone 1\n\
set state(querylength) $value\n\
}\n\
if {[string length $key]} {\n\
puts $s \"$key: $value\"\n\
}\n\
}\n\
if {$isQueryChannel && $state(querylength) == 0} {\n\
\n\
set start [tell $state(-querychannel)]\n\
seek $state(-querychannel) 0 end\n\
set state(querylength) \\\n\
[expr {[tell $state(-querychannel)] - $start}]\n\
seek $state(-querychannel) $start\n\
}\n\
\n\
\n\
if {$isQuery || $isQueryChannel} {\n\
puts $s \"Content-Type: $state(-type)\"\n\
if {!$contDone} {\n\
puts $s \"Content-Length: $state(querylength)\"\n\
}\n\
puts $s \"\"\n\
fconfigure $s -translation {auto binary}\n\
fileevent $s writable [list http::Write $token]\n\
} else {\n\
puts $s \"\"\n\
flush $s\n\
fileevent $s readable [list http::Event $token]\n\
}\n\
\n\
if {! [info exists state(-command)]} {\n\
\n\
\n\
wait $token\n\
if {[string equal $state(status) \"error\"]} {\n\
return -code error [lindex $state(error) 0]\n\
}		\n\
}\n\
} err]} {\n\
\n\
\n\
if {[string equal $state(status) \"error\"]} {\n\
Finish $token $err 1\n\
}\n\
cleanup $token\n\
return -code error $err\n\
}\n\
\n\
return $token\n\
}\n\
\n\
\n\
proc http::data {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(body)\n\
}\n\
proc http::status {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(status)\n\
}\n\
proc http::code {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(http)\n\
}\n\
proc http::ncode {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[regexp {[0-9]{3}} $state(http) numeric_code]} {\n\
return $numeric_code\n\
} else {\n\
return $state(http)\n\
}\n\
}\n\
proc http::size {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(currentsize)\n\
}\n\
\n\
proc http::error {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[info exists state(error)]} {\n\
return $state(error)\n\
}\n\
return \"\"\n\
}\n\
\n\
\n\
proc http::cleanup {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[info exists state]} {\n\
unset state\n\
}\n\
}\n\
\n\
\n\
proc http::Connect {token} {\n\
variable $token\n\
upvar 0 $token state\n\
global errorInfo errorCode\n\
if {[eof $state(sock)] ||\n\
[string length [fconfigure $state(sock) -error]]} {\n\
Finish $token \"connect failed [fconfigure $state(sock) -error]\" 1\n\
} else {\n\
set state(status) connect\n\
fileevent $state(sock) writable {}\n\
}\n\
return\n\
}\n\
\n\
\n\
proc http::Write {token} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
\n\
\n\
set done 0\n\
if {[catch {\n\
\n\
\n\
if {[info exists state(-query)]} {\n\
\n\
\n\
puts -nonewline $s \\\n\
[string range $state(-query) $state(queryoffset) \\\n\
[expr {$state(queryoffset) + $state(-queryblocksize) - 1}]]\n\
incr state(queryoffset) $state(-queryblocksize)\n\
if {$state(queryoffset) >= $state(querylength)} {\n\
set state(queryoffset) $state(querylength)\n\
set done 1\n\
}\n\
} else {\n\
\n\
\n\
set outStr [read $state(-querychannel) $state(-queryblocksize)]\n\
puts -nonewline $s $outStr\n\
incr state(queryoffset) [string length $outStr]\n\
if {[eof $state(-querychannel)]} {\n\
set done 1\n\
}\n\
}\n\
} err]} {\n\
\n\
set state(posterror) $err\n\
set done 1\n\
}\n\
if {$done} {\n\
catch {flush $s}\n\
fileevent $s writable {}\n\
fileevent $s readable [list http::Event $token]\n\
}\n\
\n\
\n\
if {[string length $state(-queryprogress)]} {\n\
eval $state(-queryprogress) [list $token $state(querylength)\\\n\
$state(queryoffset)]\n\
}\n\
}\n\
\n\
\n\
proc http::Event {token} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
\n\
if {[eof $s]} {\n\
Eof $token\n\
return\n\
}\n\
if {[string equal $state(state) \"header\"]} {\n\
if {[catch {gets $s line} n]} {\n\
Finish $token $n\n\
} elseif {$n == 0} {\n\
variable encodings\n\
set state(state) body\n\
if {$state(-binary) || ![string match -nocase text* $state(type)]\n\
|| [string match *gzip* $state(coding)]\n\
|| [string match *compress* $state(coding)]} {\n\
fconfigure $s -translation binary\n\
if {[info exists state(-channel)]} {\n\
fconfigure $state(-channel) -translation binary\n\
}\n\
} else {\n\
set idx [lsearch -exact $encodings \\\n\
[string tolower $state(charset)]]\n\
if {$idx >= 0} {\n\
fconfigure $s -encoding [lindex $encodings $idx]\n\
}\n\
}\n\
if {[info exists state(-channel)] && \\\n\
![info exists state(-handler)]} {\n\
fileevent $s readable {}\n\
CopyStart $s $token\n\
}\n\
} elseif {$n > 0} {\n\
if {[regexp -nocase {^content-type:(.+)$} $line x type]} {\n\
set state(type) [string trim $type]\n\
regexp -nocase {charset\\s*=\\s*(\\S+)} $type x state(charset)\n\
}\n\
if {[regexp -nocase {^content-length:(.+)$} $line x length]} {\n\
set state(totalsize) [string trim $length]\n\
}\n\
if {[regexp -nocase {^content-encoding:(.+)$} $line x coding]} {\n\
set state(coding) [string trim $coding]\n\
}\n\
if {[regexp -nocase {^([^:]+):(.+)$} $line x key value]} {\n\
lappend state(meta) $key [string trim $value]\n\
} elseif {[string match HTTP* $line]} {\n\
set state(http) $line\n\
}\n\
}\n\
} else {\n\
if {[catch {\n\
if {[info exists state(-handler)]} {\n\
set n [eval $state(-handler) {$s $token}]\n\
} else {\n\
set block [read $s $state(-blocksize)]\n\
set n [string length $block]\n\
if {$n >= 0} {\n\
append state(body) $block\n\
}\n\
}\n\
if {$n >= 0} {\n\
incr state(currentsize) $n\n\
}\n\
} err]} {\n\
Finish $token $err\n\
} else {\n\
if {[info exists state(-progress)]} {\n\
eval $state(-progress) \\\n\
{$token $state(totalsize) $state(currentsize)}\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::CopyStart {s token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[catch {\n\
fcopy $s $state(-channel) -size $state(-blocksize) -command \\\n\
[list http::CopyDone $token]\n\
} err]} {\n\
Finish $token $err\n\
}\n\
}\n\
\n\
\n\
proc http::CopyDone {token count {error {}}} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
incr state(currentsize) $count\n\
if {[info exists state(-progress)]} {\n\
eval $state(-progress) {$token $state(totalsize) $state(currentsize)}\n\
}\n\
if {[string length $error]} {\n\
Finish $token $error\n\
} elseif {[catch {eof $s} iseof] || $iseof} {\n\
Eof $token\n\
} else {\n\
CopyStart $s $token\n\
}\n\
}\n\
\n\
\n\
proc http::Eof {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[string equal $state(state) \"header\"]} {\n\
set state(status) eof\n\
} else {\n\
set state(status) ok\n\
}\n\
set state(state) eof\n\
Finish $token\n\
}\n\
\n\
\n\
proc http::wait {token} {\n\
variable $token\n\
upvar 0 $token state\n\
\n\
if {![info exists state(status)] || [string length $state(status)] == 0} {\n\
vwait $token\\(status)\n\
}\n\
\n\
return $state(status)\n\
}\n\
\n\
\n\
proc http::formatQuery {args} {\n\
set result \"\"\n\
set sep \"\"\n\
foreach i $args {\n\
append result $sep [mapReply $i]\n\
if {[string equal $sep \"=\"]} {\n\
set sep &\n\
} else {\n\
set sep =\n\
}\n\
}\n\
return $result\n\
}\n\
\n\
\n\
proc http::mapReply {string} {\n\
variable formMap\n\
variable alphanumeric\n\
\n\
\n\
regsub -all \\[^$alphanumeric\\] $string {$formMap(&)} string\n\
regsub -all {[][{})\\\\]\\)} $string {\\\\&} string\n\
return [subst -nocommand $string]\n\
}\n\
\n\
\n\
proc http::ProxyRequired {host} {\n\
variable http\n\
if {[info exists http(-proxyhost)] && [string length $http(-proxyhost)]} {\n\
if {![info exists http(-proxyport)] || \\\n\
![string length $http(-proxyport)]} {\n\
set http(-proxyport) 8080\n\
}\n\
return [list $http(-proxyhost) $http(-proxyport)]\n\
}\n\
}\n\
";
#include "tclcl.h"
EmbeddedTcl et_tcl(code);
