
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_lang_instrument_ClassDefinition__
#define __java_lang_instrument_ClassDefinition__

#pragma interface

#include <java/lang/Object.h>
#include <gcj/array.h>


class java::lang::instrument::ClassDefinition : public ::java::lang::Object
{

public:
  ClassDefinition(::java::lang::Class *, JArray< jbyte > *);
  ::java::lang::Class * getDefinitionClass();
  JArray< jbyte > * getDefinitionClassFile();
private:
  ::java::lang::Class * __attribute__((aligned(__alignof__( ::java::lang::Object)))) theClass;
  JArray< jbyte > * theClassFile;
public:
  static ::java::lang::Class class$;
};

#endif // __java_lang_instrument_ClassDefinition__