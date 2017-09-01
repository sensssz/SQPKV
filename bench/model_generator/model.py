''' A Model class that represents a table in the database.
'''

class Model(object):
  ''' Represents a model
  '''
  _header_includes = ['"utils/nullable.h"', '<string>', '<cstdint>', '<ctime>']
  _impl_includes = ['"utils/utils.h"', '"json.h"', '<cassert>']
  _cpp_type_map = {'FLOAT': 'double',
                   'TIMESTAMP': 'std::time_t',
                   'VARCHAR':   'std::string',
                   'BIGINT':    'uint64_t',
                   'INT':       'int',
                   'INTEGER':   'int',
                   'TINYINT':   'int'}
  _json_type_map = {'FLOAT': 'number',
                    'TIMESTAMP':  'string',
                    'VARCHAR':    'string',
                    'BIGINT':     'number',
                    'INT':        'number',
                    'INTEGER':    'number',
                    'TINYINT':    'number'}

  def __init__(self, name, namespace, member_names, member_types, type_nullable):
    self.__name = name
    self.__namespace = namespace
    self.__member_names = member_names
    self.__member_json_types = [Model._json_type_map[x] for x in member_types]
    self.__type_nullable = type_nullable

    self.__member_cpp_types = []
    for i in range(0, len(member_types)):
      type_name = Model._cpp_type_map[member_types[i]]
      if type_nullable[i]:
        type_name = 'Nullable<%s>' % type_name
      self.__member_cpp_types.append(type_name)

    self.__class_name = ''.join(x.capitalize() for x in self.__name.split('_'))

  def __autogen_warning(self, content):
    return '/* This file is automatically generated. Do NOT modify it directly! */\n\n' + content + '\n'

  def __include_guard(self, content):
    ''' Add include guard to the content of the file.
    '''
    guard = 'MODELS_' + self.__name + '_H_'
    return ('#ifndef %s\n'
            '#define %s\n\n'
            '%s\n' +\
            '#endif // %s') %\
            (guard, guard, content, guard)

  def __to_include_stmts(self, includes):
    include_str = ''
    for include in includes:
      include_str += '#include ' + include + '\n'
    return include_str

  def __header_and_namespace(self, content, is_header):
    ''' Add include headers and namespace to the content.
    '''
    if is_header:
      includes = self.__to_include_stmts(Model._header_includes)
    else:
      includes = self.__to_include_stmts(Model._impl_includes)
      includes = ('#include "%s.h"\n'
                  '%s') % (self.__name.lower(), includes)
    return ('%s\n'
            'namespace %s {\n\n'
            '%s\n\n'
            '} // namespace %s\n') %\
            (includes, self.__namespace, content, self.__namespace)

  def __gen_definition(self):
    ''' Generate definition of this class.
    '''
    definition = ('class %s {\n'
                  'public:\n'
                  '  static %s FromJson(const std::string &json);\n'
                  '  std::string ToJson();\n'
                  '  std::string name() {\n'
                  '    return "%s";\n'
                  '  }\n\n') %\
                  (self.__class_name, self.__class_name, self.__class_name)
    max_type_len = max([len(x) for x in self.__member_cpp_types])
    for mname, mtype in zip(self.__member_names, self.__member_cpp_types):
      definition += ('  %s' % mtype).ljust(max_type_len + 6) + mname + ';\n'
    return definition + '};'

  def __gen_fromjson_impl(self):
    '''Generate implementation of function FromJson.
    '''
    impl = ('%s %s::FromJson(const std::string &json) {\n'
            '  njson j = njson::parse(json);\n'
            '  %s model_instance;\n') %\
            (self.__class_name, self.__class_name, self.__class_name)
    for i in range(0, len(self.__member_names)):
      mname = self.__member_names[i]
      json_type = self.__member_json_types[i]
      cpp_type = self.__member_cpp_types[i]
      if self.__type_nullable[i]:
        cpp_type = cpp_type[len('Nullable<'):-1]
        impl += ('  assert(j["%s"].is_%s() || j["%s"].is_null());\n'
                 '  if (!j["%s"].is_null()) {\n'
                 '    model_instance.%s = j["%s"].get<%s>();\n'
                 '  }\n') %\
                 (mname, json_type, mname, mname, mname, mname, cpp_type)
      else:
        impl += ('  assert(j["%s"].is_%s());\n'
                 '  model_instance.%s = j["%s"].get<%s>();\n') %\
                 (mname, json_type, mname, mname, cpp_type)
    impl += '  return model_instance;\n}'
    return impl

  def __gen_tostring__impl(self):
    ''' Generate implementation for funciton ToString.
    '''
    impl = ('std::string %s::ToJson() {\n'
            '  njson j;\n\n') % (self.__class_name)
    for i in range(0, len(self.__member_names)):
      mname = self.__member_names[i]
      cpp_type = self.__member_cpp_types[i]
      if self.__type_nullable[i]:
        if 'time' in cpp_type:
          conversion = 'TimeToString'
        else:
          conversion = ''
        impl += ('  if (%s.IsNull()) {\n'
                 '    j["%s"] = nullptr;\n'
                 '  } else {\n'
                 '    j["%s"] = %s(%s.value());\n'
                 '  }\n') %\
                 (mname, mname, mname, conversion, mname)
      else:
        impl += '  j["%s"] = %s(%s);\n' % (mname, conversion, mname)
    impl += ('  return j.dump();\n'
             '}')
    return impl

  def __gen_impl(self):
    ''' Generate implementation of the class.
    '''
    impls = 'using njson = nlohmann::json;\n\n'
    impls += self.__gen_fromjson_impl()
    impls += '\n\n'
    impls += self.__gen_tostring__impl()
    return impls

  def file_name(self):
    ''' Return the base name of the header and implementation file.
    '''
    return self.__name.lower()

  def gen_header_content(self):
    ''' Generate content of the header file.
    '''
    content = self.__gen_definition()
    content = self.__header_and_namespace(content, True)
    content = self.__include_guard(content)
    content = self.__autogen_warning(content)
    return content

  def gen_impl_content(self):
    ''' Generate content of the implementation file.
    '''
    content = self.__gen_impl()
    content = self.__header_and_namespace(content, False)
    content = self.__autogen_warning(content)
    return content