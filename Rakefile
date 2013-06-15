require 'rake/clean'

CLEAN.include '*.o'
CLEAN.include 'stegasaurus'
CLEAN.include 'unstegasaurus'

CC     = 'clang'
CXX    = 'clang++'
CFLAGS = '-Wall -Wextra -pedantic -O0'
LIBS   = '-lgif'

SRC = FileList['*.c']
OBJ = SRC.ext('o')
STEG_OBJ = ['stegasaurus.o', 'encode.o', 'scan_gif.o'].join(' ')
UNSTEG_OBJ = ['unstegasaurus.o', 'decode.o', 'scan_gif.o'].join(' ')

task :default => [:stegasaurus] #, :unstegasaurus]

rule '.o' => '.c' do |t|
  sh "#{CC} #{CFLAGS} -c -o #{t.name} #{t.source}"
end

file 'stegasaurus' => OBJ do
  sh "#{CC} #{CFLAGS} -o stegasaurus #{LIBS} #{STEG_OBJ}"
end

file 'unstegasaurus' => OBJ do
  sh "#{CC} #{CFLAGS} -o unstegasaurus #{LIBS} #{UNSTEG_OBJ}"
end
