#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Simply run it (only reads files):
#   ./scripts/check_header_guards.rb
#
# @version 0.2.3
# @author Bradley Whited
###

require 'pathname'

def main
  is_good = check_multi_header_guards(
    ['Cybel'   ,'src/cybel'],
    ['EkoScape','src'],
  )

  exit(1) unless is_good
end

def check_multi_header_guards(*projs)
  projs = projs.map do |(name,src_dir)|
    [name,Pathname.new(src_dir).realdirpath.to_s]
  end

  is_good = true

  projs.each do |(name,src_dir)|
    exc_dirs = projs.map { |(_n,d)| d }
                    .reject { |d| src_dir.start_with?(d) }

    is_good &&= check_header_guards(name,src_dir,exc_dirs: exc_dirs)
  end

  return is_good
end

def check_header_guards(proj_name,src_dir,exc_dirs: [])
  src_path = Pathname.new(src_dir)
  exc_dirs = exc_dirs.map { |d| Pathname.new(d).realdirpath.to_s }

  # Guess project name based on parent dir.
  proj_name = src_path.realdirpath.parent.basename.to_s if proj_name.nil?
  proj_name = proj_name.strip.upcase

  is_good = true

  puts
  src_path.glob('**/*.{h,H,hh,hpp,hxx,h++}') do |file|
    next if exc_dirs.any? { |exc_dir| file.realdirpath.to_s.start_with?(exc_dir) }

    guard = file.relative_path_from(src_path).descend
                .map { |p| p.basename.to_s.strip.upcase }
                .reject(&:empty?)
                .join('_').tr('.','_')
    guard += '_'

    # In order of how it should appear in file.
    guards = [
      "#ifndef #{proj_name}_#{guard}",
      "#define #{proj_name}_#{guard}",
    ]
    guards_to_find = guards.dup

    file.each_line do |line|
      break if guards_to_find.empty?

      guards_to_find.delete_at(0) if line.start_with?(guards_to_find[0])
    end

    if !guards_to_find.empty?
      is_good = false

      puts "'#{file}'"
      guards.each { |g| puts g }
      puts
    end
  end

  return is_good
end

main if __FILE__ == $PROGRAM_NAME
