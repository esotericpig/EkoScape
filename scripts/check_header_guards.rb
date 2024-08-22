#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# @version 0.2.0
# @author Bradley Whited
###

require 'pathname'

def check_header_guards(proj_name,src_dir,exc_dirs: [])
  src_path = Pathname.new(src_dir)
  exc_dirs = exc_dirs.map { |d| Pathname.new(d).realdirpath.to_s }

  # Guess project name based on parent dir.
  proj_name = src_path.realdirpath.parent.basename.to_s if proj_name.nil?
  proj_name = proj_name.strip.upcase

  puts
  src_path.glob('**/*.{h,H,hh,hpp,hxx,h++}') do |file|
    next if exc_dirs.any? { |exc_dir| file.realdirpath.to_s.start_with?(exc_dir) }

    guard = file.relative_path_from(src_path).descend
                .map { |p| p.basename.to_s.strip.upcase }
                .reject(&:empty?)
                .join('_').gsub('.','_')
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
      puts "'#{file}'"
      guards.each { |g| puts g }
      puts
    end
  end
end

def check_multi_header_guards(*projs)
  projs = projs.map do |proj|
    proj[1] = Pathname.new(proj[1]).realdirpath.to_s
    proj
  end

  projs.each do |(name,src_dir)|
    exc_dirs = projs.map { |(_n,d)| d }
                    .reject { |d| src_dir.start_with?(d) }

    check_header_guards(name,src_dir,exc_dirs: exc_dirs)
  end
end

check_multi_header_guards(
  ['Cybel'   ,'src/cybel'],
  ['EkoScape','src'],
)
