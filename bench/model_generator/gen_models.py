#!/usr/bin/env python
''' This module takes in a .sql file containing the definition of SQL tables
    and generate C++ code for each of the tables.
'''

import os
import sys

import gflags

from gflags import FLAGS

from model import Model

gflags.DEFINE_string('model_file', None,
                     'Path of the .sql file containing definition of the tables.')
gflags.DEFINE_string('namespace', None,
                     'Namespace to which the code belongs.')
gflags.DEFINE_string('models_dir', 'models',
                     'Direcotry to write the output files to.')

def usage():
  ''' Print usage of this script.
  '''
  print 'Usage: %s ARGS\n%s' % (sys.argv[0], FLAGS)

def remove_comma(string):
  ''' Remove the comma from the string
  '''
  i = string.find(',')
  if i > 0:
    return string[:i]
  else:
    return string

def parse_models():
  ''' Parse the file and generate models objects from it.
  '''
  model_file = open(FLAGS.model_file)
  models = []
  in_model = False
  model_name = ''
  member_names = []
  member_types = []
  type_nullable = []
  for line in model_file:
    if 'CREATE TABLE' in line:
      model_name = line.split()[2]
      in_model = True
    elif ');' in line and len(model_name) > 0:
      models.append(Model(model_name, FLAGS.namespace, member_names, member_types, type_nullable))
      in_model = False
      model_name = ''
      member_names = []
      member_types = []
      type_nullable = []
    elif in_model and ' KEY ' not in line and 'CHECK' not in line:
      member_info = line.split()
      member_names.append(member_info[0])
      if 'VARCHAR' in member_info[1]:
        member_types.append('VARCHAR')
      else:
        member_types.append(remove_comma(member_info[1]))
      type_nullable.append('NOT NULL' in line)
  return models

def create_if_not_exists(dirname):
  ''' Create the dir if it does not exist.
  '''
  if not os.path.exists(dirname):
    os.makedirs(dirname)

def gen_model_files(models):
  ''' Generate the source code files for the models
  '''
  models_dir = FLAGS.models_dir
  if not models_dir.endswith('/'):
    models_dir += '/'
  create_if_not_exists(models_dir)
  for model in models:
    header_name = models_dir + model.file_name() + '.h'
    print 'Generate file %s...' % header_name
    header_file = open(header_name, 'w')
    header_file.write(model.gen_header_content())
    header_file.close()
    impl_name = models_dir + model.file_name() + '.cc'
    print 'Generate file %s...' % impl_name
    impl_file = open(impl_name, 'w')
    impl_file.write(model.gen_impl_content())
    impl_file.close()

def main(argv):
  ''' Main function.
  '''
  try:
    argv = FLAGS(argv)  # parse flags
  except gflags.FlagsError, err:
    print err
    usage()
    sys.exit(1)

  if FLAGS.model_file is None or FLAGS.namespace is None:
    usage()
    sys.exit(1)

  models = parse_models()
  gen_model_files(models)

if __name__ == '__main__':
  main(sys.argv)
