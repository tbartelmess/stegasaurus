require 'rake/clean'

CLEAN.include '*.o'
CLEAN.include 'stegasaurus'

CC     = 'clang'
CXX    = 'clang++'
CFLAGS = '-Wall -Wextra -pedantic -O2'
LIBS   = '-lgif'

SRC = FileList['*.c']
OBJ = SRC.ext('o')

task :default => :stegasaurus

rule '.o' => '.c' do |t|
  sh "#{CC} #{CFLAGS} -c -o #{t.name} #{t.source}"
end

file 'stegasaurus' => OBJ do
  sh "#{CC} #{CFLAGS} -o stegasaurus #{LIBS} #{OBJ}"
end
