/* see License.txt for copyright and terms of use */

#include "libqual/dd_list-serialize.h"

/* Most clients don't need these: just use the standard
   s18n_container_ops dd_list_ops.  These are just available in case you
   need to overload some of them. */
void s18n_dd_list_iterator(s18n_type *ct, void *container, void *iterator, void *arg)
{
  dd_list_pos *ddls = (dd_list_pos *)iterator;
  *ddls = dd_first(container);
}

void *s18n_dd_list_next(s18n_type *ct, void *container, void *iterator, void *arg)
{
  dd_list_kind *ddlk = (dd_list_kind *)
    ((s18n_container_info *)ct->private_info)->private_info;
  dd_list_pos *ddls = (dd_list_pos *)iterator;

  while (!dd_is_end(*ddls)) {
    void *elt = DD_GET(void*, *ddls);
    *ddls = dd_next(*ddls);

    if (ddlk->ddlp == NULL || ddlk->ddlp->f == NULL ||
	ddlk->ddlp->f(elt, ddlk->ddlp->arg) == 0)
      return elt;
  }

  return NULL;
}

void *s18n_dd_list_constructor(s18n_type *ct, size_t size_dummy, void *arg)
{
  dd_list_kind *ddlk = (dd_list_kind *)
    ((s18n_container_info *)ct->private_info)->private_info;

  return dd_new_list(*ddlk->r);
}

void s18n_dd_list_insert(s18n_type *ct, void *container, void *element, void *arg)
{
  dd_list_kind *ddlk = (dd_list_kind *)
    ((s18n_container_info *)ct->private_info)->private_info;

  dd_add_last(*ddlk->r, container, element);
}

/* The standard serialization ops for dd_lists */
s18n_container_ops dd_list_ops =
{
  .iterator = s18n_dd_list_iterator,
  .next = s18n_dd_list_next,
  .constructor = s18n_dd_list_constructor,
  .insert = s18n_dd_list_insert
};
