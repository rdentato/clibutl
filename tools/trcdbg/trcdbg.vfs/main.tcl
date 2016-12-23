# 
# 
# 

set topdir [info nameofexecutable]
set exename [file tail $topdir]
if { $exename == "cookit.exe" || $exename == "cookit"} {
  set topdir [file dirname $argv0]
}

set tracefname ""
set curtraceline 0

proc build_gui {} {
  global topdir
  
  set win_ico   [image create photo -file "$topdir/res/trcdbg.gif"]
  set load_ico  [image create photo -file "$topdir/res/document-open.gif"]
  set next_ico  [image create photo -file "$topdir/res/go-next.gif"]
  set prev_ico  [image create photo -file "$topdir/res/go-previous.gif"]
  set first_ico [image create photo -file "$topdir/res/go-first.gif"]
  set quit_ico  [image create photo -file "$topdir/res/system-log-out.gif"]
  
  wm title . "Tracing Debugger"
  wm iconphoto . -default $win_ico
  
  ttk::panedwindow .panes -orient horizontal
  grid .panes -row 1 -column 0 -sticky nswe 
  
  font create txtfont -family Courier -size 10
  
  foreach  p {.panes.trace .panes.source} {
    ttk::labelframe $p -height 450 -width 150
    .panes add $p -weight 1 
    
    ttk::scrollbar $p.hsb -orient horizontal -command [list $p.txt xview]
    ttk::scrollbar $p.vsb -orient vertical -command [list $p.txt yview]
    text $p.txt -xscrollcommand [list $p.hsb set] -yscrollcommand [list $p.vsb set] -font txtfont -insertwidth 0 -cursor arrow -wrap none
    
    grid $p.txt -column 0 -row 0 -sticky nswe
    grid $p.hsb -column 0 -row 1 -sticky we
    grid $p.vsb -column 1 -row 0 -sticky ns
    
    grid columnconfigure $p 0 -weight 1
    grid rowconfigure $p 0 -weight 1
  }
  
  .panes.trace  configure -text "Trace: " -height 650
  .panes.source configure -text "Source: " -height 650
  
  .panes.trace.txt configure -width 50 -height 40 
  .panes.source.txt configure -width 50 -height 40 
  
  
  ttk::frame .cmds -relief groove -height 40
  ttk::sizegrip .cmds_grp
  
  ttk::button .cmds.load -image $load_ico -command open_trace
  ttk::label .cmds.spc1 -text " "
  ttk::button .cmds.first -image $first_ico -command {set_trace_line 1}
  ttk::button .cmds.prev  -image $prev_ico  -command {set_trace_line [expr $curtraceline - 1]}
  ttk::button .cmds.next  -image $next_ico  -command {set_trace_line [expr $curtraceline + 1]}
  ttk::label .cmds.spc2 -text " "
  
  ttk::button .cmds.quit -image $quit_ico -command exit
  
  pack .cmds.load  -pady 2 -padx 1  -side left
  pack .cmds.spc1  -pady 2 -padx 5  -side left
  pack .cmds.first -pady 2 -padx 1  -side left
  pack .cmds.prev  -pady 2 -padx 1  -side left
  pack .cmds.next  -pady 2 -padx 1  -side left
  pack .cmds.spc2  -pady 2 -padx 5  -side left
  
  pack .cmds.quit  -pady 2 -padx 1 -side right
  
  grid .cmds -row 0 -column 0  -stick ew
  grid .cmds_grp -row 2 -column 0 -stick e
  
  grid columnconfigure . 0 -weight 1
  grid rowconfigure . 1 -weight 1
  
  bind .panes.trace.txt <ButtonPress> trace_click
  bind .panes.trace.txt <KeyPress>  break
  bind .panes.source.txt <KeyPress>  break
}

proc set_trace_line {ln} {
  global curtraceline 
  set w .panes.trace.txt
  if {$ln > 0 && $ln < [$w count -lines 1.0 end]} {
    $w tag delete curln
    $w tag add curln $ln.0 $ln.end
    $w tag configure  curln  -foreground yellow  -background blue
    $w see $ln.0
    set curtraceline $ln
  }
}


proc trace_click {} {
  set ln [lindex [split [.panes.trace.txt index current] .] 0]
  set_trace_line $ln
}

proc open_trace {} {
  global tracefname
  
  set types {
    {{Log Files}     {.log}        }
    {{Trace Files}   {.trc .trace} }
    {{Text Files}    {.txt}        }
    {{All Files}     *             }
  }
  set tracefname [tk_getOpenFile -filetypes $types]

  if {$tracefname ne ""} {
    set fl [open $tracefname]
    set data [read $fl]
    close $fl
    
    .panes.trace  configure -text "Trace: [file tail $tracefname]" 
    .panes.trace.txt delete 0.0 end
    .panes.trace.txt insert 0.0 [encoding convertfrom utf-8 $data]
    
  }
   set_trace_line 1
}

build_gui


