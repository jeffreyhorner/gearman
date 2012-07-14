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
         invisible(.Call(gearman:::addserver,client,host,port))
      },
      addServers = function(servers){
         invisible(.Call(gearman:::addserver,client,servers))
      },
      doBackground = function(){
         .Call(gearman:::dotask,client,background=TRUE,high=NA)
      },
      doHigh = function(){
         .Call(gearman:::dotask,client,background=FALSE,high=TRUE)
      },
      doHighBackground = function(){
         .Call(gearman:::dotask,client,background=TRUE,high=TRUE)
      },
      doLow = function(){
         .Call(gearman:::dotask,client,background=FALSE,high=FALSE)
      },
      doLowBackground = function(){
         .Call(gearman:::dotask,client,background=TRUE,high=FALSE)
      },
      doNormal = function(){
         .Call(gearman:::dotask,client,background=FALSE,high=NA)
      },
      jobStatus = function(){
      }
      success = function() .Call(gearman:::success,client),
      failed = function() .Call(gearman:::failed,client),
      continue = function() .Call(gearman:::continue,client),
      strerror = function() .Call(gearman:::strerror,client)
   )
)
