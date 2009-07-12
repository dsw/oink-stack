static int
calccost (/*impl-int*/ srcy, /*impl-int*/ srcx)
{
  srcy++;
  srcx++;
}

static int
calccostB (srcy, srcx)     
     int srcy;
     /*impl-int srcx;*/
{
  srcy++;
  srcx++;
}

static int calccost2 (/*impl-int*/ srcy)
{
  srcy++;
}

// FIX: this parses in gcc but not in oink
static int calccost3 (register /*impl-int*/ *srcy)
{
  (*srcy)++;
}

// does not parse in gcc
/*  static int calccost (*srcy) */
/*  { */
/*    (*srcy)++; */
/*  } */

// does not parse in gcc
/*  int main2() { */
/*    x, y; */
/*    x++; */
/*    y++; */
/*  } */

extern void host_read_event(/*impl-int*/ gpointer, /*impl-int*/ gint);

int f1(x, y)
  int x;
  /*impl-int y;*/ 
{
  x++;
  y++;
}
