// t0582.cc
// currently causes Elsa to crash outright
// from James Dennet

struct vector {
   template < typename InputIterator > 
   vector(InputIterator,InputIterator);
};

struct string : vector {
   template < typename InputIterator > 
   string(InputIterator end)
     : vector(end, end ) 
   { }
};

template < typename ElementType,int ArraySize > 
ElementType *end(ElementType(&array)[ArraySize]) 
{
  int dummy[2];
  string(end(dummy)); // succeeds if this is moved pass the closing brace
}

// EOF
