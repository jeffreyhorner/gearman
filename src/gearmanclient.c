#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/RS.h>

#include "libgearman/gearman.h"

typedef struct rgearman_client_t {
   gearman_client_st *client;
   gearman_return_t lastretcode;
} rgearman_client_t;

SEXP createclient(void){
   rgearman_client_t *client;

   client = (rgearman_client_t *)R_Calloc(1,rgearman_client_t);

   if (client)
      client->client = gearman_client_create(NULL);

   return (client) ?
      R_MakeExternalPtr(client,R_NilValue,R_NilValue) :
      R_NilValue;
}

SEXP finalizeclient(SEXP Sclient){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);

   if (client){
      gearman_client_free(client->client);
      R_Free(client);
   } else
      warning("GearmanClient::finalize called with NULL client");

   return R_NilValue;
}

SEXP success(SEXP Sclient){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   SEXP ret = PROTECT(allocVector(LGLSXP,1));

   LOGICAL(ret)[0] = (gearman_success(client->lastretcode))? TRUE : FALSE;

   UNPROTECT(1);
   return ret;
}

SEXP failed(SEXP Sclient){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   SEXP ret = PROTECT(allocVector(LGLSXP,1));

   LOGICAL(ret)[0] = (gearman_failed(client->lastretcode))? TRUE : FALSE;

   UNPROTECT(1);
   return ret;
}

SEXP gm_continue(SEXP Sclient){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   SEXP ret = PROTECT(allocVector(LGLSXP,1));

   LOGICAL(ret)[0] = (gearman_continue(client->lastretcode))? TRUE : FALSE;

   UNPROTECT(1);
   return ret;
}

SEXP gm_strerror(SEXP Sclient){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   SEXP ret = PROTECT(allocVector(STRSXP,1));

   SET_STRING_ELT(ret,0,mkChar(gearman_strerror(client->lastretcode)));

   UNPROTECT(1);
   return ret;
}

SEXP addserver(SEXP Sclient, SEXP Shost, SEXP Sport){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   in_port_t port = (in_port_t)INTEGER(Sport)[0];
   const char *host;

   if (Shost != R_NilValue)
      host = (const char *)CHAR(STRING_ELT(Shost,0));
   else
      host = NULL;

   client->lastretcode = gearman_client_add_server(client->client,host,port);

   return success(Sclient);
}

SEXP addservers(SEXP Sclient, SEXP Sservers){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   const char *servers=NULL;

   if (Sservers != R_NilValue)
      servers = (const char *)CHAR(STRING_ELT(Sservers,0));

   client->lastretcode = gearman_client_add_servers(client->client,servers);

   return success(Sclient);
}

/*
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
