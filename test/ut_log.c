/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
**
*/

#include "utl.h"

int main(int argc, char *argv[])
{
  logopen("l_log.log");

  logprintf("This line will always be printed");
  
  logwatch("%(info%))","%(warning%)","%(error%)") {
    loginfo("This line is expected to be in the log (info)");
    logwarning("This line is expected to be in the log (warning)");
    logerror("This line is expected to be in the log (error)");
  }

  loglevel("INFO");
  logexpect(utl_log_prdlvl == UTL_LOG_I,"Log Level: %d",utl_log_prdlvl);
  
  logwatch("%(info%)","%(warning%)","%(error%)") {
    loginfo("This line is expected to be in the log (info)");
    logwarning("This line is expected to be in the log (warning)");
    logerror("This line is expected to be in the log (error)");
  }
  
  loglevel("WARNING");
  logexpect(utl_log_prdlvl == UTL_LOG_W,"Log Level: %d",utl_log_prdlvl);
  
  logwatch("!%(info%)","%(warning%)","%(error%)") {
    loginfo("This line is NOT expected to be in the log (info)");
    logwarning("This line is expected to be in the log (warning)");
    logerror("This line is expected to be in the log (error)");
  }
  
  loglevel("ERROR");
  logexpect(utl_log_prdlvl == UTL_LOG_E,"Log Level: %d",utl_log_prdlvl);
  
  logwatch("!%(info%)","!%(warning%)","%(error%)") {
    loginfo("This line is NOT expected to be in the log (info)");
    logwarning("This line is NOT expected to be in the log (warning)");
    logerror("This line is expected to be in the log (error)");
  }
  
  loglevel("NONE");
  logexpect(utl_log_prdlvl == UTL_LOG_N,"Log Level: %d",utl_log_prdlvl);
  
  logwatch("!%(info%)","!%(warning%)","!%(error%)") {
    loginfo("This line is NOT expected to be in the log (info)");
    logwarning("This line is NOT expected to be in the log (warning)");
    logerror("This line is NOT expected to be in the log (error)");
  }
  
  logclose();
}
