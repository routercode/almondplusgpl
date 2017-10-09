#ifndef ERROR_LOG_H
#define ERROR_LOG_H


/*
  Logging functions are used as follows (printf format string):

  LOG(DBG("format", args));    logs debug message
  LOG(ERR("format", args));    logs error message
*/

#define LOGGING_ON 1
#ifdef LOGGING_ON
#define LOG(a) { a; }
#else
#define LOG(a) {}
#endif

void DBG(const char *fmt, ...);
void ERR(const char *fmt, ...);


/*
  open and close logfiles at startup / quit
  (if logging is turned off these do nothing)
*/

void open_logs(char *whoisit, int clean, int redirect_no);
void close_logs(void);

#endif
