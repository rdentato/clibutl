/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_UNITTEST

#include "utl.h"

FILE *f = NULL;
char buf[512];
int k=0;
int c=0;

#define lg logStderr

void functhrow(tryenv env, int err)
{
    throw(env, err);
    logTest(lg,0); /* it's an error to be executed! */
}

int main (int argc, char *argv[])
{
  tryenv env = NULL;
  
  logTestPlan(lg,"utl unit test: try/catch") {
  
    k = 0;
    try(env) {
      throw(env,1); 
    }
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9; 
    }); 
    logEQint(lg,"Exception 1 caught", 1,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0; env = NULL;
    try(env) {
      throw(env,2);
    }
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    
    logEQint(lg,"Exception 2 caught", 2,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) { throw(env,3); }
    catch({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logEQint(lg,"Exception not caught", 9,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) { functhrow(env,1); }
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9; 
    }); 
    logEQint(lg,"Exception 1 caught", 1,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0; env = NULL;
    try(env)  functhrow(env,2); 
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logEQint(lg,"Exception 2 caught", 2,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) { functhrow(env,3); }
    catch({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logEQint(lg,"Exception not caught", 9,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env)      {
      try(env)    { 
        functhrow(env,20);
      }
      catch({
        case 10 : k += 10; break;
        case 20 : k += 20; break;
        default : rethrow;
      });
    }
    catch({
      case 1 : k += 1; break;
      case 2 : k += 2; break;
    });
    logEQint(lg,"Exception 20 caught", 20,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) {
      try(env) {
       functhrow(env,2);
      }
      catch({
        case 10 : k += 10; break;
        case 20 : k += 20; break;
        default : rethrow;
      });
    }
    catch({
      case 1 : k += 1; break;
      case 2 : k += 2; break;
      default : rethrow;
    });
    
    logEQint(lg,"Exception 2 caught", 2,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    env = NULL;
    try(env) {
      logTestNote(lg,"A %d %p %d",k,env,__LINE__);
      try(env) {
        logTestNote(lg,"B %d %p %d",k,env,__LINE__);
        try(env) { 
          logTestNote(lg,"C %d %p %d",k,env,__LINE__);
          functhrow(env,2);
        }
        catch({
          case 10 : k += 10; break;
          case 20 : k += 20; break;
          default : logTestNote(lg,"2 %d %p %d",k,env,__LINE__); rethrow;
        });
      }
      catch({
        case 1 : k += 1; logTestNote(lg,"3 %d %p %d",k,env,__LINE__);break;
        case 2 : k += 2; logTestNote(lg,"4 %d %p %d",k,env,__LINE__);functhrow(env,10); break;
        default: k += 100; logTestNote(lg,"5 %d %p %d",k,env,__LINE__);
      });
    }
    catch(
     default: k += 300; logTestNote(lg,"6 %d %p %d",k,env,__LINE__);
    );
    logEQint(lg,"Inner try are invisible", 102,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env)  { throw(env,2); }
    catch({
      case 1 :  k += 1; break;
      case 2 :  k += 2; functhrow(env,1);
      default:  k += 9;  
    });
    logEQint(lg,"Exception 2 then 1 caught", 3,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) {
      try(env)   { functhrow(env,20); }
      catch({
        case 10 : k += 10; functhrow(env,2);
        case 20 : k += 20; throw(env,10);
        default : rethrow;
      });
    }
    catch({
      case 1 : k += 1; break;
      case 2 : k += 2; break;
    });
    
    logEQint(lg,"Exception 10,20,2 caught", 32,k);
    logEQptr(lg,"Env is null",NULL,env);
    
    k = 0;
    try(env) {
      try(env)    { functhrow(env,20); }
      catch({
        case 10 : k += 10; functhrow(env,2);
        case 20 : k += 20; throw(env,10);
        default : rethrow;
      });
    }
    catch({
      case 1 : k += 1; break;
      case 2 : k += 2; functhrow(env,3);
      default: k += 100;  
    });
    logEQint(lg,"Exception 10,20,2,3 caught", 132,k);
    logEQptr(lg,"Env is null",NULL,env);
  }
  return 0;
}
