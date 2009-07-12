// ERR-MATCH: 4ef4c3e9-7047-42ce-a305-4b4e19b2867a

// qual: Assertion failed: paramIter.data() == fv->getEllipsis() &&
// "4ef4c3e9-7047-42ce-a305-4b4e19b2867a", file dataflow_ex.cc line 241

void foo(int x, ...) {
}

void (*bar)() = (void (*)()) foo;

int main() {
    (*bar)(42, 43);
}
