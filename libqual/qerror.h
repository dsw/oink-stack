/* see License.txt for copyright and terms of use */

#ifndef QERROR_H
#define QERROR_H

#include "libqual/location.h"
#include "libqual/buffer.h"
#include "libqual/quals.h"

typedef enum {sev_err, sev_warn, sev_info} severity;

void init_qerror(void);
void reset_qerror(void);

/* Set to TRUE if any qualifier errors happened (unsatisfiable
   constraints) */
extern bool qerrors;

/* Detailed information about the last invalid constraint */
extern growbuf qual_error_detail;
extern growbuf qtype_error_detail;

/* printf into X_error_detail */
int qual_ed_printf(const char *fmt, ...);
int qtype_ed_printf(const char *fmt, ...);


/*
 * Error reporting customization
 */
typedef void (*vreport_qerror_fn)(const char *current_function_name,
				  location loc, severity sev, qual q,
				  const char *format, va_list args);

void default_vreport_qerror(const char *current_function_name, location loc,
			    severity sev, qual q, const char *format,
			    va_list args);

void set_vreport_qerror_func(vreport_qerror_fn f);

/*
 * Functions for reporting errors
 */
void vreport_qerror(const char *current_function_name, location loc,
		    severity sev, qual q, const char *format,
		    va_list args);

void report_error(char const *func, location loc, severity sev, const char *format, ...);
void report_qual_error(location loc, severity sev, qual q, const char *format, ...);

#endif
