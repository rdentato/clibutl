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
    logTestNote(lg,"Exception 1 caught");
    logEQint(lg, 1,k);
    logNULL(lg,env);
    
    k = 0; env = NULL;
    try(env) {
      throw(env,2);
    }
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    
    logTestNote(lg,"Exception 1 caught");
    logEQint(lg, 2,k);
    logNULL(lg,env);
    
    k = 0;
    try(env) { throw(env,3); }
    catch({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logTestNote(lg,"Exception not caught");
    logEQint(lg,9,k);
    logNULL(lg,env);
    
    k = 0;
    try(env) { functhrow(env,1); }
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9; 
    }); 
    logTestNote(lg,"Exception 1 caught");
    logEQint(lg, 1,k);
    logNULL(lg, env);
    
    k = 0; env = NULL;
    try(env)  functhrow(env,2); 
    catch ({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logTestNote(lg,"Exception 2 caught");
    logEQint(lg, 2,k);
    logNULL(lg, env);
    
    k = 0;
    try(env) { functhrow(env,3); }
    catch({
      case 1 :  k = 1; break;
      case 2 :  k = 2; break;
      default:  k = 9;  
    });
    logTestNote(lg,"Exception not caught");
    logEQint(lg, 9,k);
    logNULL(lg, env);
    
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
    logTestNote(lg,"Exception 20 caught");
    logEQint(lg, 20,k);
    logNULL(lg, env);
    
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
    
    logTestNote(lg,"Exception 2 caught");
    logEQint(lg, 2,k);
    logNULL(lg, env);
    
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
    logTestNote(lg,"Inner try are invisible");
    logEQint(lg, 102,k);
    logNULL(lg, env);
    
    k = 0;
    try(env)  { throw(env,2); }
    catch({
      case 1 :  k += 1; break;
      case 2 :  k += 2; functhrow(env,1);
      default:  k += 9;  
    });
    logTestNote(lg,"Exception 2 then 1 caught");
    logEQint(lg, 3,k);
    logNULL(lg, env);
    
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
    
    logTestNote(lg,"Exception 10,20,2 caught");
    logEQint(lg, 32,k);
    logNULL(lg, env);
    
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
    logTestNote(lg,"Exception 10,20,2,3 caught");
    logEQint(lg, 132,k);
    logNULL(lg, env);
  }
  return 0;
}
