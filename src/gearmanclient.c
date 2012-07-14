#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "libgearman/gearman.h"

SEXP createclient(void){
   gearman_client_st *client;

   client = gearman_client_create(NULL);

   return (client) ?
      R_MakeExternalPtr(client,R_NilValue,R_NilValue) :
      R_NilValue;
}

SEXP finalizeclient(SEXP Sclient){
   gearman_client_st *client = (gearman_client_st *)R_ExternalPtrAddr(Sclient);

   if (client)
      gearman_client_free(client);
   else
      warning("GearmanClient::finalize called with NULL client");

   return R_NilValue;
}

/*
SEXP addserver(SEXP Sclient, SEXP Shost, SEXP Sport){
}

SEXP addservers(SEXP Sclient, SEXP Sservers){
}

SEXP dotask(SEXP Sclient, ...){
}

SEXP jobstatus(SEXP Sclient){
} */

R_CallMethodDef callMethods[] = {
   {"createclient", (DL_FUNC) &createclient, 0},
   {"finalizeclient", (DL_FUNC) &finalizeclient, 1},
   {"addserver", (DL_FUNC) &addserver, 3},
   {"addservers", (DL_FUNC) &addservers, 2},
/*   {"dotask", (DL_FUNC) &dotask, 0}, 
   {"jobstatus, (DL_FUNC) &jobstatus, 0}, */
   {"success", (DL_FUNC) &success, 1},
   {"failed", (DL_FUNC) &failed, 1},
   {"continue", (DL_FUNC) &gm_continue, 1},
   {"strerror", (DL_FUNC) &gm_strerror, 1},
   {NULL, NULL, 0}
};

void R_init_gearman(DllInfo *info) {
   R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
