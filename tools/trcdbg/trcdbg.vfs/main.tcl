# 
# 
# 

package require Tk
set topdir [info nameofexecutable]
set exename [file tail $topdir]
if { $exename == "cookit.exe" || $exename == "cookit64.run" || $exename == "cookit.run"} {
  set topdir [file dirname $argv0]
} elseif {$exename == "wish"} {
  set topdir [file dirname [info script]]
} 

set tracefname ""
set traceline 0
set sourcefname ""
set sourceline 0
set basename ""

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
  ttk::button .cmds.prev  -image $prev_ico  -command {set_trace_line [expr $traceline - 1]}
  ttk::button .cmds.next  -image $next_ico  -command {set_trace_line [expr $traceline + 1]}
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
  bind .                 <KeyPress> {keypress %K; break}
  bind .panes.trace.txt  <KeyPress> {keypress %K; break}
  bind .panes.source.txt <KeyPress> {keypress %K; break}
}

proc keypress {k}  {
  global traceline
  if {$k eq "Up" || $k eq "Left" } {
    set_trace_line [expr $traceline - 1]
  } elseif {$k eq "Down" || $k eq "Right" } {
    set_trace_line [expr $traceline + 1]
  } elseif {$k eq "Home"} {
    set_trace_line 1
  } else {
    #tk_messageBox -message $k
  }
  return false
}

proc set_trace_line {ln} {
  global traceline 
  set w .panes.trace.txt
  if {$ln > 0 && $ln < [$w count -lines 1.0 end]} {
    $w tag delete curln
    $w tag add curln $ln.0 $ln.end
    $w tag configure  curln  -foreground yellow  -background blue
    $w see $ln.0
    set traceline $ln
    set txtln [$w get -displaychars $ln.0 $ln.end]
    if {[regexp {^\d{4}.*\t:([^:]+):(\d+)\t$} $txtln x srcfn srcln]} {
      set_source_line $srcfn $srcln
    }
  }
}

proc set_source_line {fname ln} {
  global sourcefname
  global sourceline
  global basename
  
  set w .panes.source.txt
  if {$fname ne $sourcefname } {
    set sourcefname $fname
    .panes.source.txt delete 0.0 end
    
    if {![file exists $fname]} {
      set fname "$basename/../$fname"
      if {![file exists $fname]} {
        set fname "$basename/[file tail $fname]"
        if {![file exists $fname]} {
          set fname ""
        }
      }
    }
    #tk_messageBox -message $fname\n$ln
    if {$fname ne ""} {
      set fl [open $fname]
      set data [read $fl]
      close $fl
      .panes.source.txt insert 0.0 [encoding convertfrom utf-8 $data]
    } else {
      set sourcefname ""
      set sourceline 0
      set ln 0
    }
    .panes.source  configure -text "Source: $sourcefname" 
  }
  if {$ln != $sourceline} {
    $w tag delete curln
    $w tag add curln $ln.0 $ln.end
    $w tag configure  curln  -foreground yellow  -background blue
    $w see $ln.0
    set sourceline $ln
  }
}


proc trace_click {} {
  set ln [lindex [split [.panes.trace.txt index current] .] 0]
  set_trace_line $ln
}

proc open_trace {} {
  global tracefname
  global basename

  
  set types {
    {{Log Files}     {.log}        }
    {{Trace Files}   {.trc .trace} }
    {{Text Files}    {.txt}        }
    {{All Files}     *             }
  }
  set fname [tk_getOpenFile -filetypes $types]

  if {$fname ne ""} {
    set tracefname $fname
    set basename [file dirname $fname]
    set fl [open $tracefname]
    set data [read $fl]
    close $fl
    
    .panes.trace  configure -text "Trace: [file tail $tracefname]" 
    .panes.trace.txt delete 0.0 end
    .panes.trace.txt insert 0.0 [encoding convertfrom utf-8 $data]

    .panes.source  configure -text "Source: " 
    .panes.source.txt delete 0.0 end
    set sourcefname ""
    set sourceline 0
  }
  set_trace_line 1
}

build_gui


