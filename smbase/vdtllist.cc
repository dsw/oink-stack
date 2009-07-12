// vdtllist.cc            see license.txt for copyright and terms of use
// code for vdtllist.h

#include "vdtllist.h"      // this module

void VoidTailList::steal(VoidTailList *src)
{
  if (src) {
    top = src->top;
    tail = src->tail;
    src->top = NULL;    // paranoia
    delete src;
  }
  else {
    top = NULL;
    tail = NULL;
  }
}

void VoidTailList::prepend(void *newitem)
{
  VoidList::prepend(newitem);
  if (!tail) {
    tail = top;
  }
}

void VoidTailList::append(void *newitem)
{
  if (isEmpty()) {
    prepend(newitem);
  }
  else {
    // payoff: constant-time append
    tail->next = new VoidNode(newitem, NULL);
    tail = tail->next;
  }
}

void VoidTailList::appendAll(VoidTailList &tail)
{
  for (VoidTailListIter iter(tail); !iter.isDone(); iter.adv()) {
    this->append(iter.data());
  }
}

void VoidTailList::insertAt(void *newitem, int index)
{
  VoidList::insertAt(newitem, index);
  adjustTail();
}

void VoidTailList::concat(VoidTailList &srcList)
{
#if 1
  // quarl 2006-06-01: alter representation directly; VoidList::concat() would
  // take O(N) to find the tail.
  if (srcList.top) {
    if (top) {
      tail->next = srcList.top;
    } else {
      top = srcList.top;
    }

    tail = srcList.tail; xassert(tail);
    srcList.top = NULL;
    srcList.tail = NULL;
  } else {
    // nothing to do
  }
#else
  // grab what will be the tail of the concatenated list
  VoidNode *catTail = srcList.top? srcList.tail : tail;

  // build proper backbone structure
  VoidList::concat(srcList);

  // fix tails
  tail = catTail;
  srcList.tail = NULL;
#endif
}

void VoidTailList::adjustTail()
{
  if (!tail) {
    tail = top;
  }
  else if (tail->next) {
    tail = tail->next;
  }
  xassert(tail->next == NULL);
}

void *VoidTailList::removeFirst()
{
  xassert(top);
  if (top == tail) {
    tail = NULL;
  }
  void *retval = top->data;
  VoidNode *tmp = top;
  top = top->next;
  delete tmp;
  return retval;
}

void *VoidTailList::removeLast()
{
  xassert(top);
  if (top == tail) {
    return removeFirst();
  }

  VoidNode *before = top;
  while (before->next != tail) {
    before = before->next;
  }
  void *retval = tail->data;
  delete tail;
  tail = before;
  tail->next = NULL;
  return retval;
}

void *VoidTailList::removeAt(int index)
{
  xassert(top);
  if (index == 0) {
    // NOTE: removeFirst fixes the 'tail' pointer.
    return removeFirst();
  }

  VoidNode *before = top;    // will point to node before one to be removed
  index--;
  while (index > 0) {
    before = before->next;
    index--;
  }
  xassert(index == 0);

  // fix 'tail' if necessary
  if (tail == before->next) {
    tail = before;
  }

  // patch around before->next
  VoidNode *toDelete = before->next;
  void *retval = toDelete->data;
  before->next = toDelete->next;
  delete toDelete;

  return retval;
}

// for now, copy of VoidList::removeIfPresent, since the functions it calls
// are not virtual.
bool VoidTailList::removeIfPresent(void *item)
{
  // for now, not a real fast implementation
  int index = indexOf(item);
  if (index == -1) {
    return false;   // not there
  }
  else {
    removeAt(index);
    return true;
  }
}

// for now, copy of VoidList::removeItem, since the functions it calls are not
// virtual.
void VoidTailList::removeItem(void *item)
{
  bool wasThere = removeIfPresent(item);
  xassert(wasThere);
}

void VoidTailList::removeAll()
{
  VoidList::removeAll();
  tail = NULL;
}

bool VoidTailList::prependUnique(void *newitem)
{
  bool retval = VoidList::prependUnique(newitem);
  adjustTail();
  return retval;
}

bool VoidTailList::appendUnique(void *newitem)
{
  bool retval = VoidList::appendUnique(newitem);
  adjustTail();
  return retval;
}

void VoidTailList::selfCheck() const
{
  VoidList::selfCheck();

  if (isNotEmpty()) {
    // get last node
    VoidNode *n = top;
    while (n->next) {
      n = n->next;
    }

    // 'tail' should be the last one
    xassert(tail == n);
  }
  else {
    xassert(tail == NULL);
  }
}


// --------------- VoidTailListMutator ------------------
VoidTailListMutator&
  VoidTailListMutator::operator=(VoidTailListMutator const &obj)
{
  // we require that the underlying lists be the same
  // because we can't reset the 'list' reference if they
  // aren't
  xassert(&list == &obj.list);

  prev = obj.prev;
  current = obj.current;

  return *this;
}


void VoidTailListMutator::insertBefore(void *item)
{
  if (prev == NULL) {
    // insert at start of list
    list.prepend(item);
    reset();
  }
  else {
    current = new VoidNode(item, current);
    if (list.tail == current)
      list.tail = current;
    prev->next = current;
  }
}


void VoidTailListMutator::insertAfter(void *item)
{
  xassert(!isDone());
  current->next = new VoidNode(item, current->next);
  if (list.tail == current) {
    list.tail = current->next;
  }
}


void VoidTailListMutator::append(void *item)
{
  xassert(isDone());
  insertBefore(item);
  adv();
}


void *VoidTailListMutator::remove()
{
  xassert(!isDone());
  void *retval = data();
  if (prev == NULL) {
    // removing first node
    list.top = current->next;
    delete current;
    current = list.top;
    if (current == NULL) {
      list.tail = NULL;
    }
  }
  else {
    current = current->next;
    delete prev->next;   // old 'current'
    prev->next = current;
    if (current == NULL) {
      list.tail = prev;
    }
  }
  return retval;
}


// --------------------- test code ------------------
#ifdef TEST_VDTLLIST

#include <stdio.h>    // printf

int main()
{
  VoidTailList list;
  int zero, one, two, three;

  // This isn't a very exhaustive test; it's mainly to check that
  // selfCheck doesn't do anything really stupid (it used to).

  list.selfCheck();

  list.append(&two);     list.selfCheck();
  list.prepend(&one);    list.selfCheck();
  list.append(&three);   list.selfCheck();
  list.prepend(&zero);   list.selfCheck();

  xassert(list.nth(0) == &zero);
  xassert(list.nth(1) == &one);
  xassert(list.nth(2) == &two);
  xassert(list.nth(3) == &three);

  list.removeAll();
  list.selfCheck();

  printf("vdtllist works\n");

  return 0;
}

#endif // TEST_VDTLLIST
