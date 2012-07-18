GearmanClient <- setRefClass(
   'GearmanClient',
   fields = c('client'),
   methods = list(
      initialize = function(...){
         client <<- .Call(gearman:::createclient)
         callSuper(...)
      },
      finalize = function(){
         .Call(gearman:::finalizeclient,client)
      },
      addServer = function(host=NULL,port=0L){
         invisible(.Call(gearman:::addserver,client,host,as.integer(port)))
      },
      addServers = function(servers){
         invisible(.Call(gearman:::addservers,client,servers))
      },
      doNormal = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=FALSE,high=NULL)
      },
      doHigh = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=FALSE,high=TRUE)
      },
      doLow = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=FALSE,high=FALSE)
      },
      doBackground = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=TRUE,high=NULL)
      },
      doHighBackground = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=TRUE,high=TRUE)
      },
      doLowBackground = function(fun=NULL,work=NULL,uval=NULL){
         .Call(gearman:::dotask,client,fun,work,uval,background=TRUE,high=FALSE)
      },
      jobStatus = function(job=NULL){
         .Call(gearman:::jobstatus,client,job)
      },
      success = function() .Call(gearman:::success,client),
      failed = function() .Call(gearman:::failed,client),
      continue = function() .Call(gearman:::continue,client),
      strerror = function() .Call(gearman:::strerror,client)
   )
)
