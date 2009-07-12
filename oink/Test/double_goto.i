// from 4Suite-0.11.1-13/Oif.tab-ddlI.i.c_out

// elsa can't handle two gotos to the same target
int main() {
  goto a;
  goto a;
a: ;
}
