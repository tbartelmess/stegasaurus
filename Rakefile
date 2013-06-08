require 'rake/clean'

CLEAN.include '*.o'
CLEAN.include 'stegasaurus'

CC     = 'clang'
CXX    = 'clang++'
CFLAGS = '-Wall -Werror -pedantic -O2'

SRC = FileList['*.c']
OBJ = SRC.ext('o')

task :default => :stegasaurus

rule '.o' => '.c' do |t|
  sh "#{CC} #{CFLAGS} -c -o #{t.name} #{t.source}" 
end

file 'stegasaurus' => OBJ do
  sh "#{CC} #{CFLAGS} -o stegasaurus #{OBJ}" 
end

