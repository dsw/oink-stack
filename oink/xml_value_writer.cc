// see License.txt for copyright and terms of use

#include "xml_value_writer.h"   // this module
#include "xml_enum.h"           // XTOK_*
#include "oink_util.h"          // USER_ERROR
#include "stringset.h"          // StringSet

#define serializeOracle (dynamic_cast<XVW_SerializeOracle*> (serializeOracle_m))

// NOTE: This does not go here because it is overridden depending on
// whether we are doing an oink analysis (for which we cannot mention
// qvars) or a qual analysis.
// static inline bool shouldSerialize(Value const *value) {return value;}

XmlValueWriter::XmlValueWriter
  (IdentityManager &idmgr0,
   // VarPredicateFunc *varPred0, ValuePredicateFunc *valuePred0,
   ASTVisitor *astVisitor0, ValueVisitor *valueVisitor0,
   std::ostream *out0, int &depth0, bool indent0,
   XVW_SerializeOracle *serializeOracle0)
  : XmlTypeWriter(idmgr0, astVisitor0, out0, depth0, indent0, serializeOracle0)
  // , varPred(varPred0)
  // , valuePred(valuePred0)
  , valueVisitor(valueVisitor0)
{}

char const * const XmlValueWriter::tagName_Value(Value *v) {
  switch(v->getTag()) {
  default: xfailure("illegal tag");
  case Value::V_ATOMIC: return "CVAtomicValue";
  case Value::V_POINTER: return "PointerValue";
  case Value::V_REFERENCE: return "ReferenceValue";
  case Value::V_FUNCTION: return "FunctionValue";
  case Value::V_ARRAY: return "ArrayValue";
  case Value::V_POINTERTOMEMBER: return "PointerToMemberValue";
  }
}

void XmlValueWriter::toXml(Value *v) {
  // bool printedRepr;
  // if (v->isCVAtomicValue()) {
  //   printedRepr = idmgr.printed0(v->asCVAtomicValue());
  // }

  // idempotency
  if (idmgr.printed(v)) return;

  // visiting
  if (valueVisitor) {
    if (!valueVisitor->preVisitValue(v)) return;
  }

  // manual virtual dispatch
  switch(v->getTag()) {
  default: xfailure("illegal tag");

  case Value::V_ATOMIC: {
    CVAtomicValue *atom = v->asCVAtomicValue();

    // CVAtomicValue *repr_value = atom->find();
    BucketOValues *reprBucket = atom->getBucket();

    bool printedReprBucket = idmgr.printed0(reprBucket);
    // xassert(printedRepr == printedReprBucket);

    // CVAtomicValue *orig_ufLink = atom->ufLink;
    if (! printedReprBucket) {
      // quarl 2006-05-20
      //   This is the first time we are printing a CVAtomicValue within this
      //   equivalence class.  Thus we make this value the representative of
      //   this class.  This is necessary, because otherwise the
      //   representative and its bucket might not get printed due to
      //   shouldSerialize() being false for that object.  (The lack of this
      //   call is why instance sensitivity serialization wasn't working until
      //   now!)
      atom->makeRepresentative();
    }

    // We are about to print atom->ufLink; make sure the path is already
    // compressed so that ufLink is either NULL, or points at the
    // representative.
    xassert(atom->ufLinkForEquivRelOfDomOfMapToBucket == NULL ||
            atom->ufLinkForEquivRelOfDomOfMapToBucket
            ->ufLinkForEquivRelOfDomOfMapToBucket == NULL);

    if (atom->bucket != NULL) xassert(atom->bucket->isEnabled());

    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(atom), atom);
      // **** attributes
      // * superclasses
      toXml_Value_properties(atom);

      // * members
      printPtr(atom, type);
      printPtr(atom, atVoid);

      // fprintf(stderr, "## toXml(v=%p): "
      //         "printedReprBucket=%d, "
      //         "orig_ufLink=%p, "
      //         "ufLink=%p, bucket=%p\n",
      //         v,
      //         printedReprBucket,
      //         orig_ufLink,
      //         atom->ufLink, atom->bucket);

      // quarl 2006-05-20
      //   Since we've permuted the ufLinks above, ufLink should definitely
      //   point to something within the serialized tree, and that node's
      //   bucket gets printed.  Thus we can print raw 'ufLink' and 'bucket'
      //   pointers, making deserialization a piece of cake - just read in the
      //   pointers and satisfy links.

      printPtr(atom, ufLinkForEquivRelOfDomOfMapToBucket);
      printPtr(atom, bucket);

      // printThing(_repr_value, repr_value, xmlPrintPointer(
      //              xmlPrintPointer(idmgr.idPrefix(bucket), idmgr.uniqueId(bucket))));

      // printThing(_repr_bucket, repr_bucket, xmlPrintPointer(
      //              xmlPrintPointer(idmgr.idPrefix(bucket), idmgr.uniqueId(bucket))));

      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(atom);
    // * members
    trav(atom->type);
    trav(atom->atVoid);

    // * buckets

    // quarl 2006-05-19
    //   We now inline the BucketOValues, so we serialize it here.
    //
    //   We serialize the bucket of the representative CVAtomicValue now,
    //   otherwise it may never happen: the representative CVAtomicValue may
    //   be not reached due to shouldSerialize.

    // There need not be a bucket here; this qvar may have never participated
    // in data flow, e.g. a struct that was never used.

    if (atom->bucket != NULL) {
      // This should have been fixed earlier in this function
      xassert(atom->bucket->isEnabled());
      bool shouldSrzRange = serializeOracle==NULL ||
        serializeOracle->shouldSerialize(atom->bucket);
      {
        Restorer<ValueVisitor*>
          restorerKey(this->valueVisitor, shouldSrzRange?this->valueVisitor:NULL);
        toXml(atom->bucket);
      }
    }

    if (reprBucket != NULL) {
      // quarl 2006-05-20
      //   We should have printed the bucket either just now, or earlier.
      //   It is important that printed0(reprBucket) be true here because when
      //   we visit another CVAtomicValue in this equivalence relation, we'll
      //   decide whether we've printed that CVAtomicValue based on whether
      //   the representative bucket has ever been printed.
      xassert(idmgr.printed0(reprBucket));
    }

    break;
  }

  case Value::V_POINTER: {
    PointerValue *ptr = v->asPointerValue();

    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(ptr), ptr);
      // **** attributes
      // * superclasses
      toXml_Value_properties(ptr);
      // * members
      printPtr(ptr, type);
      printPtr(ptr, atValue);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(ptr);
    // * members
    trav(ptr->type);
    trav(ptr->atValue);
    break;
  }

  case Value::V_REFERENCE: {
    ReferenceValue *ref = v->asReferenceValue();

    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(ref), ref);
      // **** attributes
      // * superclasses
      toXml_Value_properties(ref);
      // * members
      printPtr(ref, type);
      printPtr(ref, atValue);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(ref);
    // * members
    trav(ref->type);
    trav(ref->atValue);
    break;
  }

  case Value::V_FUNCTION: {
    FunctionValue *func = v->asFunctionValue();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(func), func);
      // **** attributes
      // * superclasses
      toXml_Value_properties(func);
      // * members
      printPtr(func, type);
      printPtr(func, retValue);
      printPtr(func, retVar);
      printPtr(func, params);

      // was this before ellipsis became a pointer to a pointer
      printPtr(func, ellipsisHolder);

      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(func);
    // * members
    trav(func->type);
    trav(func->retValue);
    trav(func->retVar);
    travObjListPtr_S(func, FunctionValue, params, Variable_O);
    trav(func->ellipsisHolder);
    break;
  }

  case Value::V_ARRAY: {
    ArrayValue *arr = v->asArrayValue();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(arr), arr);
      // **** attributes
      // * superclasses
      toXml_Value_properties(arr);
      // * members
      printPtr(arr, type);
      printPtr(arr, atValue);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(arr);
    // * members
    trav(arr->type);
    trav(arr->atValue);
    break;
  }

  case Value::V_POINTERTOMEMBER: {
    PointerToMemberValue *ptm = v->asPointerToMemberValue();
    XmlTagPrinter tagPrinter(*this);
    if (writingP()) {
      tagPrinter.printOpenTag(tagName_Value(ptm), ptm);
      // **** attributes
      // * superclasses
      toXml_Value_properties(ptm);
      // * members
      printPtr(ptm, type);
      printPtr(ptm, atValue);
      tagPrinter.tagEnd();
    }

    // **** subtags
    // * superclasses
    toXml_Value_subtags(ptm);
    // * members
    trav(ptm->type);
    trav(ptm->atValue);
    break;
  }

  }

  // visiting
  if (valueVisitor) {
    valueVisitor->postVisitValue(v);
  }
}

void XmlValueWriter::toXml(FVEllipsisHolder *fveh) {
  // idempotency
  if (idmgr.printed(fveh)) return;

  // visiting
  if (valueVisitor) {
    if (!valueVisitor->preVisitFVEllipsisHolder(fveh)) {
      return;
    }
  }

  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("FVEllipsisHolder", fveh);
    // **** attributes
    // * members
    printPtr(fveh, ellipsis);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * members
  trav(fveh->ellipsis);

  // visiting
  if (valueVisitor) {
    valueVisitor->postVisitFVEllipsisHolder(fveh);
  }
}

bool XmlValueWriter::XVW_SerializeOracle::shouldSerialize(Variable const *x) {
  return serializeVar_O(const_cast<Variable*>(x));
}


char const * const XmlValueWriter::tagName_Variable() {
  return "Variable_O";
}

// NOTE: do NOT delegate to the superclass; we handle all of it here
void XmlValueWriter::toXml(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  // printf ("## toXml(Variable): var->name '%s'\n", var->name); fflush(stdout);

  // idempotency
  if (idmgr.printed(var)) return;

  // FIX: hack: we use this visitor for serialization but also for
  // general visitation so that we can mark qvars as extern; during
  // that pass, a serialization oracle is not provided.  In general it
  // is only the serialization oralce that is preventing variables
  // that are not getFilteredKeep() from being visited, so if one is
  // not present, this assertion is guaranteed to fail.
  if (serializeOracle_m) {
    xassert(!var->filteredOut());
  }

  // avoid varibles that shouldn't be serialized
  // FIX: this is off for now
  // if (varPred && !varPred(var)) return;

  // instead of passing in a varPred, just use preVisitValue or shouldSerialize.

  // visiting
  if (valueVisitor) {
    if (!valueVisitor->preVisitVariable(var)) {
      return;
    }
  }

  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag(tagName_Variable(), var);
    // **** attributes
    // * superclasses
    toXml_Variable_properties(var);
    // * members
    printPtr(var, abstrValue0);
    // NOTE: might bring this back, but for now is serialized as user1
//     printXml_bool(hasFuncDefn, var->hasFuncDefn);
    tagPrinter.tagEnd();
  }

  // **** subtags
  // * superclasses
  toXml_Variable_subtags(var);//SUBTRAVERSAL
  // * members
  trav(var->abstrValue0);//SUBTRAVERSAL

  // visiting
  if (valueVisitor) {
    valueVisitor->postVisitVariable(var);
  }

}

void XmlValueWriter::toXml_externVars(TailList<Variable_O> *list) {
  travObjList0(*list, externVars, Variable_O, FOREACH_TAILLIST_NC, TailList);
}

void XmlValueWriter::toXml_Value_properties(Value *value) {
  printXml_SourceLoc(loc, value->loc);
  printPtr(value, refToMe);
  printPtr(value, ptrToMe);
  if (!Value::allow_annotation) {
    bool usedInDataflow = value->getUsedInDataflow();
    printXml_bool(usedInDataflow, usedInDataflow);
  }
}

void XmlValueWriter::toXml_Value_subtags(Value *value) {
  trav(value->refToMe);
  trav(value->ptrToMe);
}

void XmlValueWriter::toXml(BucketOValues *bv) {
  xassert(bv->enabled);
  // idempotency
  if (idmgr.printed(bv)) return;
  XmlTagPrinter tagPrinter(*this);
  if (writingP()) {
    tagPrinter.printOpenTag("BucketOValues", bv);

    // **** attributes
    printEmbed(bv, nameToValue);
    tagPrinter.tagEnd();
  }

  // **** subtags
  if (!idmgr.printed(&bv->nameToValue)) {
    XmlTagPrinter tagPrinter2(*this);
    if (writingP()) {
      tagPrinter2.printOpenTag("NameMap_BucketOValues_nameToValue", &bv->nameToValue);
      tagPrinter2.tagEnd();
    }

    // iterates over them in key-sorted order
    for(StringSObjDict<Value>::Iter iter(bv->nameToValue);
        !iter.isDone(); iter.next()) {
      rostring name = iter.key();
      // dsw: do you know how bad it gets if I don't put a const-cast
      // here?
      Value *value = const_cast<Value*>(iter.value());

      XmlTagPrinter tagPrinter3(*this);
      if (writingP()) {
        tagPrinter3.printNameMapItemOpenTag(name.c_str(), value);
      }
      toXml(value);
    }
  }
}
