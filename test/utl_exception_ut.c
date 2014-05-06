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


void functhrow(tryenv env, int err)
{
    throw(env, err);
    TST("Returned to main",0); /* it's an error to be executed! */
}

int main (int argc, char *argv[])
{
  tryenv env = NULL;
  int err;
  
  TSTPLAN("utl unit test: try/catch") {
  
    TSTSECTION("Simple catch") {
      TSTGROUP("catch 1") {
        TSTCODE {
          k = 0;
          try(env) {
            throw(env,1); 
          }
          catch ({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9; 
          }); 
        }
        TSTEQINT("Exception 1 caught", 1,k);
        TSTEQPTR("Env is null",NULL,env);
      }
     
      TSTGROUP("catch 2") {
        TSTCODE {
          k = 0; env = NULL;
          try(env) {
            throw(env,2);
          }
          catch ({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9;  
          });
        }
        TSTEQINT("Exception 2 caught", 2,k);
        TSTEQPTR("Env is null",NULL,env);
      }
      
      TSTGROUP("catch default") {
        TSTCODE {
          k = 0;
          try(env) { throw(env,3); }
          catch({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9;  
          });
        } TSTEQINT("Exception not caught", 9,k);
        TSTEQPTR("Env is null",NULL,env);
      }
    }
  
    TSTSECTION("Catch exceptions from functions") {
      TSTGROUP("catch 1") {
        TSTCODE {
          k = 0;
          try(env) { functhrow(env,1); }
          catch ({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9; 
          }); 
        } TSTEQINT("Exception 1 caught", 1,k);
        TSTEQPTR("Env is null",NULL,env);
      }
     
      TSTGROUP("catch 2") {
        TSTCODE {
          k = 0; env = NULL;
          try(env)  functhrow(env,2); 
          catch ({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9;  
          });
        } TSTEQINT("Exception 2 caught", 2,k);
        TSTEQPTR("Env is null",NULL,env);
      }
      
      TSTGROUP("catch default") {
        TSTCODE {
          k = 0;
          try(env) { functhrow(env,3); }
          catch({
            case 1 :  k = 1; break;
            case 2 :  k = 2; break;
            default:  k = 9;  
          });
        } TSTEQINT("Exception not caught", 9,k);
        TSTEQPTR("Env is null",NULL,env);
      }
    }

    TSTSECTION("Nested") {
      TSTGROUP("2 levels") {
        TSTCODE {
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
        }
        TSTEQINT("Exception 20 caught", 20,k);
        TSTEQPTR("Env is null",NULL,env);
        
        TSTCODE {
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
        }
        TSTEQINT("Exception 2 caught", 2,k);
        TSTEQPTR("Env is null",NULL,env);
      }
      
      TSTGROUP("Visibility") {
        TSTCODE {
          k = 0;
          env = NULL;
          try(env) {
            TSTNOTE("A %d %p %d",k,env,__LINE__);
            try(env) {
              TSTNOTE("B %d %p %d",k,env,__LINE__);
              try(env) { 
                TSTNOTE("C %d %p %d",k,env,__LINE__);
                functhrow(env,2);
              }
              catch({
                case 10 : k += 10; break;
                case 20 : k += 20; break;
                default : TSTNOTE("2 %d %p %d",k,env,__LINE__); rethrow;
              });
            }
            catch({
              case 1 : k += 1; TSTNOTE("3 %d %p %d",k,env,__LINE__);break;
              case 2 : k += 2; TSTNOTE("4 %d %p %d",k,env,__LINE__);functhrow(env,10); break;
              default: k += 100; TSTNOTE("5 %d %p %d",k,env,__LINE__);
            });
          }
          catch(
           default: k += 300; TSTNOTE("6 %d %p %d",k,env,__LINE__);
          );
        }
        TSTEQINT("Inner try are invisible", 102,k);
        TSTEQPTR("Env is null",NULL,env);
      }
    }

    TSTSECTION("Multiple throw") {
      TSTGROUP("Same try") {
        TSTCODE {
          k = 0;
          try(env)  { throw(env,2); }
          catch({
            case 1 :  k += 1; break;
            case 2 :  k += 2; functhrow(env,1);
            default:  k += 9;  
          });
        } TSTEQINT("Exception 2 then 1 caught", 3,k);
        TSTEQPTR("Env is null",NULL,env);
      }
        
      TSTGROUP("Nested try") {
        TSTCODE {
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
          
        }  TSTEQINT("Exception 10,20,2 caught", 32,k);
        TSTEQPTR("Env is null",NULL,env);
        
        TSTCODE {
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
        }  TSTEQINT("Exception 10,20,2,3 caught", 132,k);
        TSTEQPTR("Env is null",NULL,env);
      }
    }
  }
}
