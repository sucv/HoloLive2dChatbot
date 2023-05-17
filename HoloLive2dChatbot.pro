  TEMPLATE = subdirs
 
  SUBDIRS = \
            Framework \   # sub-project names
            Sample
 
  # where to find the sub projects - give the folders
  Framework.subdir = src/Framework
  Sample.subdir  = src/Sample
 
  # what subproject depends on others
  Sample.depends = Framework
