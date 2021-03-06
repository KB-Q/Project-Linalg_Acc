
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_xml_pipeline_PipelineFactory__
#define __gnu_xml_pipeline_PipelineFactory__

#pragma interface

#include <java/lang/Object.h>
#include <gcj/array.h>

extern "Java"
{
  namespace gnu
  {
    namespace xml
    {
      namespace pipeline
      {
          class EventConsumer;
          class PipelineFactory;
          class PipelineFactory$Pipeline;
          class PipelineFactory$Stage;
      }
    }
  }
}

class gnu::xml::pipeline::PipelineFactory : public ::java::lang::Object
{

public:
  static ::gnu::xml::pipeline::EventConsumer * createPipeline(::java::lang::String *);
  static ::gnu::xml::pipeline::EventConsumer * createPipeline(::java::lang::String *, ::gnu::xml::pipeline::EventConsumer *);
private:
  PipelineFactory();
public:
  static ::gnu::xml::pipeline::EventConsumer * createPipeline(JArray< ::java::lang::String * > *, ::gnu::xml::pipeline::EventConsumer *);
private:
  ::gnu::xml::pipeline::PipelineFactory$Pipeline * parsePipeline(JArray< ::java::lang::String * > *, ::gnu::xml::pipeline::EventConsumer *);
  ::gnu::xml::pipeline::PipelineFactory$Pipeline * parsePipeline(::gnu::xml::pipeline::EventConsumer *);
  ::gnu::xml::pipeline::PipelineFactory$Stage * parseStage();
public: // actually package-private
  static JArray< JArray< ::java::lang::String * > * > * access$0();
private:
  JArray< ::java::lang::String * > * __attribute__((aligned(__alignof__( ::java::lang::Object)))) tokens;
  jint index;
  static JArray< JArray< ::java::lang::String * > * > * builtinStages;
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_xml_pipeline_PipelineFactory__
