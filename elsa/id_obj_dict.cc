// quarl 2006-05-30

#include "id_obj_dict.h"

#include <cctype>

// TODO: It would simplify deserialization a lot of we got rid of the id
// prefixes.  One way to solve the embedded member problem might be to add
// {1,2,3} to embedded member pointers before checking the hash table.


// parse an unsigned decimal integer from a string; returns true iff string is
// an integer only.
inline
bool atoiFull(char const *p, unsigned &result)
{
  result = 0;

  while (*p) {
    if (!isdigit(*p)) return false;
    result = result * 10 + (*p - '0');
    ++p;
  }
  return true;
}

// static
bool IdSObjDict::parseId(char const *id,
                         Prefix &prefix,
                         unsigned &idnum)
{
  if (!isupper(id[0]) || !isupper(id[1]))
    return false;

  if (!atoiFull(id+2, idnum)) return false;

  prefix.set(id);

  return true;
}

void *IdSObjDict::queryif(char const *id)
{
  Prefix prefix;
  unsigned idnum;

  if (parseId(id, prefix, idnum)) {
    prefix.selfCheck();
    if (idnum+1 > (unsigned) objectsById.size()) {
      return NULL;
    }

    IdNode *node = &objectsById[idnum];
    do {
      if (node->prefix == prefix) {
        return node->object;
      }
      node = node->next;
    } while (node);
    return NULL;
  } else {
    return objectsOther.queryif(id);
  }
}

void IdSObjDict::add(char const *id, void *obj)
{
  Prefix prefix;
  unsigned idnum;

  if (parseId(id, prefix, idnum)) {
    prefix.selfCheck();
    objectsById.ensureIndexDoubler(idnum+1);

    IdNode &entry = objectsById[idnum];
    if (entry.object == NULL) {
      // set it here
      entry.object = obj;
      entry.prefix = prefix;
      xassert(entry.next == NULL);
    } else {
      // We don't care much for order.  Prepend; but we can't move the first
      // item so insert after that.
      //
      // Does not check for duplicates!
      IdNode *node = pool.alloc();
      node->object = obj;
      node->prefix = prefix;
      node->next = entry.next;
      entry.next = node;
    }
  } else {
    objectsOther.add(id, obj);
  }
}

