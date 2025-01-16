#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   ./scripts/glob_src.rb
#
# Show source files to copy & paste (only crawls dirs):
#   ./scripts/glob_src.rb -p
#
# Update source files in 'CMakeLists.txt' (overwrites file):
#   ./scripts/glob_src.rb -U
#
# @author Bradley Whited
###

require 'optparse'
require 'pathname'
require 'set'

def main
  SrcGlobber.new.run
end

class SrcGlobber
  VERSION = '0.1.2'

  # Must be all lower-cased for case-insensitive comparison.
  SRC_EXTS = %w[ .c .cc .cpp .cxx .c++ ].to_set(&:downcase).freeze

  CMAKE_FILE = 'CMakeLists.txt'
  CMAKE_FUNC = 'target_sources'

  def initialize
    @dry_run = true
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = VERSION
      op.summary_width = 8

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-p',nil,'[print] print source files')
      op.on('-U',nil,"[update] update '#{CMAKE_FILE}'; overwrites lines in #{CMAKE_FUNC}()")

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')
    end

    opts = {}
    opt_parser.parse!(into: opts)

    if opts.empty?
      puts opt_parser.help
      exit
    end

    @dry_run = opts[:n]

    if opts[:p]
      print_src
    elsif opts[:U]
      update_cmake
    end
  end

  def print_src
    puts glob_all_src
  end

  def update_cmake
    src = glob_all_src
    data = File.read(CMAKE_FILE,mode: 'rt',encoding: 'BOM|UTF-8:UTF-8')

    new_data = data.sub(
      # `^func(...)$ ... ^)$`
      /(^\s*#{Regexp.quote(CMAKE_FUNC)}\s*\(.+?$).+?(^\s*\)\s*$)/im,
      "\\1\n#{src}\n\\2"
    )

    if new_data == data
      puts "No change: '#{CMAKE_FILE}'"
    elsif @dry_run
      puts new_data
    else
      File.write(CMAKE_FILE,new_data,mode: 'wt',encoding: 'UTF-8')
      puts "Updated: '#{CMAKE_FILE}'"
    end
  end

  def glob_all_src
    str = ''.dup

    str << glob_src('third_party','${TP_DIR}')
    str << "\n\n"
    str << glob_src('src','${SRC_DIR}') do |path1,path2|
      # Bubble Cybel files to top.
      if path1.to_s.include?('cybel') && !path2.to_s.include?('cybel')
        -1
      else
        0
      end
    end

    return str
  end

  def glob_src(dir,macro,indent: 4,space_out_dirs: true,space_out_sub_dirs: false,root_dir_str: nil,&block)
    dir = Pathname(dir)
    indent = ' ' * indent if indent.is_a?(Numeric)
    root_dir_str = dir.to_s if root_dir_str.nil?

    # Grab only dirs & src files.
    paths = dir.children.select do |path|
      path.directory? || SRC_EXTS.include?(path.extname.to_s.strip.downcase)
    end
    # - Sort by `block` if given.
    # - Bubble dirs to top.
    # - Sort by basename case-insensitive.
    paths = paths.sort do |path1,path2|
      if block
        cmp = block.call(path1,path2)
        next cmp if cmp != 0
      end

      if path1.directory? && !path2.directory?
        -1
      elsif !path1.directory? && path2.directory?
        1
      else
        path1.basename.to_s.downcase <=> path2.basename.to_s.downcase
      end
    end
    result = ''.dup

    paths.each do |path|
      if path.directory?
        sub_result = glob_src(path,macro,indent: indent,space_out_dirs: space_out_sub_dirs,
            space_out_sub_dirs: space_out_sub_dirs,root_dir_str: root_dir_str,&block)

        if !sub_result.empty?
          result << sub_result << "\n"
          result << "\n" if space_out_dirs
        end
      else
        file = path.to_s.sub(root_dir_str,macro).strip
        result << "#{indent}\"#{file}\"\n" unless file.empty?
      end
    end

    return result.rstrip # Remove last newline.
  end
end

main if __FILE__ == $PROGRAM_NAME
