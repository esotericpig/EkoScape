#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# @version 0.1.0
# @author Bradley Whited
###

require 'pathname'

def check_header_guards(src_dir,proj_name=nil)
  src_path = Pathname.new(src_dir)

  # Guess project name based on parent dir.
  proj_name = src_path.realdirpath.parent.basename.to_s if proj_name.nil?
  proj_name = proj_name.strip.upcase

  puts
  src_path.glob('**/*{.h,.hpp}') do |file|
    guard = file.relative_path_from(src_path).descend.to_a
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
      error = (guards_to_find.length == guards.length) ? 'missing guards?' : 'bad guards'

      puts "'#{file}' -- #{error}"
      guards.each { |g| puts g }
      puts
    end
  end
end

check_header_guards('src')
