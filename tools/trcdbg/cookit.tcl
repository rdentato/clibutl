# Create a standalone program f
#
#

if {$argc < 1} {
  puts "cookit cookit.tcl exename"
  exit
}

set exename [info nameofexecutable]
set outname [lindex $argv 0]
set vfsname "$outname.vfs"
set outname "$outname.exe"

vfs::unmount $exename
 
file copy -force $exename $outname
 
vfs::cookfs::Mount $outname $outname
file copy -force $vfsname/main.tcl $outname
file copy -force $vfsname/res $outname
file copy -force $vfsname/lib $outname
 
vfs::unmount $outname
exit
