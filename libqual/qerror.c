/* see License.txt for copyright and terms of use */

#include "libqual/qerror.h"
#include "libqual/qual_flags.h"
#include <assert.h>
#include <stdio.h>

static region qerror_region = NULL;

/* Set to TRUE if any qualifier errors happened (unsatisfiable
   constraints) */
bool qerrors = FALSE;

/* Detailed information about the last invalid constraint */
growbuf qual_error_detail = NULL;
growbuf qtype_error_detail = NULL;

void init_qerror(void)
{
  assert(!qerror_region);
  qerror_region = newregion();
  qual_error_detail = growbuf_new(qerror_region, 256);
  qtype_error_detail = growbuf_new(qerror_region, 256);
  assert(!qerrors);
}

void reset_qerror(void)
{
  if (qerror_region)
    deleteregion(qerror_region);
  qual_error_detail = NULL;
  qtype_error_detail = NULL;
  qerror_region = NULL;
  qerrors = FALSE;
}

/* printf into qual_error_detail */
int qual_ed_printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  return gvprintf(qual_error_detail, fmt, args);
}

/* printf into qtype_error_detail */
int qtype_ed_printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  return gvprintf(qtype_error_detail, fmt, args);
}

/**************************************************************************
 *                                                                        *
 * Error reporting customization                                          *
 *                                                                        *
 **************************************************************************/

/* Report an error at location l */
void default_vreport_qerror(const char *current_function_name, location loc,
		    severity sev, qual q, const char *format,
		    va_list args)
{
  fflush(NULL);
  if (loc != NO_LOCATION)
    fprintf(stderr, "%s:%ld ", location_filename(loc), location_line_number(loc));
  vfprintf(stderr, format, args);
  if (!growbuf_empty(qual_error_detail) ||
      !growbuf_empty(qtype_error_detail))
    {
      fprintf(stderr, "\n");
      if (loc)
	fprintf(stderr, "%s:%ld ", location_filename(loc), location_line_number(loc));
      if (!growbuf_empty(qual_error_detail))
	fprintf(stderr, "%s", growbuf_contents(qual_error_detail));
      else
	fprintf(stderr, "%s", growbuf_contents(qtype_error_detail));
    }
  fprintf(stderr, "\n");
}

vreport_qerror_fn vreport_qerror_func = &default_vreport_qerror;

void set_vreport_qerror_func(vreport_qerror_fn f)
{
  vreport_qerror_func = f;
}

/**************************************************************************
 *                                                                        *
 * Error reporting                                                        *
 *                                                                        *
 **************************************************************************/

void vreport_qerror(const char *current_function_name, location loc,
		    severity sev, qual q, const char *format,
		    va_list args)
{
  if (sev == sev_err || (sev == sev_warn && report_warnings_as_errors))
    qerrors = TRUE;

  vreport_qerror_func(current_function_name, loc, sev, q, format, args);

  if (!growbuf_empty(qual_error_detail))
    growbuf_reset(qual_error_detail);
  if (!growbuf_empty(qtype_error_detail))
    growbuf_reset(qtype_error_detail);
}

/* Report an error at loc */
void report_error(char const *func, location loc, severity sev, const char *format, ...)
{
  va_list args;

  va_start(args, format);
  vreport_qerror(func, loc, sev, NULL, format, args);
}

/* Report an error at loc */
void report_qual_error(location loc, severity sev, qual q, const char *format, ...)
{
  va_list args;

  va_start(args, format);
  vreport_qerror(NULL, loc, sev, q, format, args);
}
