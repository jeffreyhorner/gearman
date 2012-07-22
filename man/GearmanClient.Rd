\name{GearmanClient}
\alias{GearmanClient-class}
\alias{GearmanClient}
\docType{class}
\keyword{classes}

\title{
Class \code{GearmanClient}
}

\description{
\code{GearmanClient} allows users to submit jobs to one or more Gearman Servers.

}

\section{Methods}{
   \describe{
      \item{\code{addServer(host,port)}:}{
         Add the server specified by the host and port to the list of possible servers
         to send jobs. Called without arguments, it will add the localhost server listening
         on the default gearman port. Returns TRUE on success, FALSE otherwise.
      }
      \item{\code{addServers(servers)}:}{
         Add a list of servers in 'host:port' notation whereby each sever is separated by a 
         comma. Hence \code{servers} is a character vector of length one. Returns TRUE on
         success, FALSE otherwise.
      }
      \item{\code{doNormal(fun,work,uval)},\code{doHigh},\code{doLow}:}{
         Each of these methods takes the same arguments of fun, work, and uval. They are
         distinguished only by the created job's priority of 'Normal', 'High', or 'Low'. 'fun' is a 
         character vector describing which Gearman job to execute. 'work' is a character vector
         passed to the job, and 'uval' is an optional character vector uniquely identify the job.
         Returns the result of the job as a character vector.
      }
      \item{\code{doBackground(fun,work,uval)},\code{doHighBackground},\code{doLowBackground}:}{
         The only difference between these methods and the above is that jobs are submitted in
         the background, meaning the client will not wait on the result, and the return value is
         a character vector denoting the job.
      }
      \item{\code{jobStatus(job)}:}{
         Inquire about the job's status. 'job' is a character vector, the return value from one of
         \code{doBackground},\code{doHighBackground}, or \code{doLowBackground}. Returns a four element
         named list: 'known' is a logical denoting whether or not the job is known to the server(s),
         'running' is a logical denoting whether or not the job is running on one of the servers, 
         'numerator' and 'denominator' are numeric vectors of length one. 
      }
      \item{\code{success()}:}{
         Returns TRUE if the last gearman API call succeeded.
      }
      \item{\code{failed()}:}{
         Returns TRUE if the last gearman API call failed.
      }
      \item{\code{continue()}:}{
         Returns TRUE if the last gearman API call is waiting on I/O, is paused, is in progress, or even
         if the job exists.
      }
      \item{\code{strerror()}:}{
         Return a string representation of the last gearman API call.
      }
}}

\examples{
\dontrun{

# Create a client, add the localhost server, submit a job that counts the
# number of lines from the payload, and get the results.
cl <- GearmanClient$new()
cl$addServer()
ret <- cl$doNormal("wc","hello\nworld\n")

ret # Produces "2\n". Client is in charge of parsing results

# Submit a job in the background and check on the status periodically.
# presume the job runs for at least ten seconds
job <- cl$doBackground("slowwc","hello\nworld\n")
Sys.sleep(5)

# Here, the job is known to the server and it is know to be running

str(cl$jobStatus(job))
# List of 4
#  $ known      : logi TRUE
#  $ running    : logi TRUE
#  $ numerator  : num 0
#  $ denominator: num 0

Sys.sleep(6)

# Here, the job is NOT known to the server and it is NOT know to be running.
# Thus it must be done.

str(cl$jobStatus(job))
# List of 4
#  $ known      : logi FALSE
#  $ running    : logi FALSE
#  $ numerator  : num 0
#  $ denominator: num 0

}}
