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

SEXP dotask(SEXP Sclient, SEXP Sfun, SEXP Swork, SEXP Suval, SEXP Sback, SEXP Shigh){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   SEXP ret;
   const char *fun, *work, *uval;
   int back, high;

   fun = (Sfun != R_NilValue)? (const char *)CHAR(STRING_ELT(Sfun,0)) : NULL;
   work = (Swork != R_NilValue)? (const char *)CHAR(STRING_ELT(Swork,0)) : NULL;
   uval = (Suval != R_NilValue)? (const char *)CHAR(STRING_ELT(Suval,0)) : NULL;

   back = LOGICAL(Sback)[0];
   high = (Shigh != R_NilValue) ? LOGICAL(Shigh)[0] : -1;

   if (back){
      gearman_return_t (*gm_fun)(gearman_client_st *client, const char *function_name, const char *unique, const void *workload, size_t workload_size, gearman_job_handle_t job_handle); 
      char *job = (char *)R_Calloc(1,gearman_job_handle_t);
      gm_fun = (high == -1)? gearman_client_do_background : (high)? gearman_client_do_high_background : gearman_client_do_low_background ;
      client->lastretcode = gm_fun(client->client,fun,uval,work,strlen(work),job);
      ret = PROTECT(allocVector(STRSXP,1));
      SET_STRING_ELT(ret,0,mkChar(job));
      R_Free(job);
      UNPROTECT(1);
   } else {
      void *(*gm_fun)(gearman_client_st *client, const char *function_name, const char *unique, const void *workload, size_t workload_size, size_t *result_size, gearman_return_t *ret_ptr); 
      size_t resultsize;
      void *result;
      gm_fun = (high == -1)? gearman_client_do : (high)? gearman_client_do_high : gearman_client_do_low ;
      result = gm_fun(client->client,fun,uval,work,strlen(work),&resultsize,&client->lastretcode);
      ret = PROTECT(allocVector(STRSXP,1));
      SET_STRING_ELT(ret,0,mkCharLen((char *)result,resultsize));
      UNPROTECT(1);
   }

   return ret;
}

SEXP jobstatus(SEXP Sclient, SEXP Sjob){
   rgearman_client_t *client = (rgearman_client_t *)R_ExternalPtrAddr(Sclient);
   const char *job;
   bool isknown, isrunning;
   uint32_t numerator, denominator;
   SEXP ans, nms;

   job = (Sjob != R_NilValue)? (const char *)CHAR(STRING_ELT(Sjob,0)) : NULL;

   client->lastretcode = gearman_client_job_status(client->client,job, &isknown, &isrunning, &numerator, &denominator);

   PROTECT(ans = allocVector(VECSXP, 4));
   PROTECT(nms = allocVector(STRSXP, 4));

   SET_STRING_ELT(nms,0,mkChar("known"));
   SET_VECTOR_ELT(ans,0,ScalarLogical(isknown==TRUE));

   SET_STRING_ELT(nms,1,mkChar("running"));
   SET_VECTOR_ELT(ans,1,ScalarLogical(isrunning==TRUE));

   SET_STRING_ELT(nms,2,mkChar("numerator"));
   SET_VECTOR_ELT(ans,2,ScalarReal((double)numerator));

   SET_STRING_ELT(nms,3,mkChar("denominator"));
   SET_VECTOR_ELT(ans,3,ScalarReal((double)denominator));

   setAttrib(ans, R_NamesSymbol, nms);

   UNPROTECT(2);
   return ans;
}


R_CallMethodDef callMethods[] = {
   {"createclient", (DL_FUNC) &createclient, 0},
   {"finalizeclient", (DL_FUNC) &finalizeclient, 1},
   {"addserver", (DL_FUNC) &addserver, 3},
   {"addservers", (DL_FUNC) &addservers, 2},
   {"dotask", (DL_FUNC) &dotask, 6}, 
   {"jobstatus", (DL_FUNC) &jobstatus, 2},
   {"success", (DL_FUNC) &success, 1},
   {"failed", (DL_FUNC) &failed, 1},
   {"continue", (DL_FUNC) &gm_continue, 1},
   {"strerror", (DL_FUNC) &gm_strerror, 1},
   {NULL, NULL, 0}
};

void R_init_gearman(DllInfo *info) {
   R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
