#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show diff of source files in 'CMakeLists.txt' (only reads):
#   ./scripts/glob_src.rb
#
# Show source files to copy & paste (only crawls dirs):
#   ./scripts/glob_src.rb -p
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
  VERSION = '0.2.4'

  TP_DIR = 'third_party'
  SRC_DIR = 'src'

  CMAKE_TP_DIR = '${TP_DIR}'
  CMAKE_SRC_DIR = '${SRC_DIR}'
  CMAKE_FILE = 'CMakeLists.txt'
  CMAKE_FUNC = 'target_sources'

  SRC_EXTS = %w[.c .cc .cpp .cxx .c++].to_set(&:downcase).freeze

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = VERSION
      op.summary_width = 16

      op.separator ''
      op.separator "v#{op.version}"
      op.separator ''
      op.separator "By default, shows diff of source files in '#{CMAKE_FILE}' in func #{CMAKE_FUNC}()."

      op.separator ''
      op.separator 'Options'
      op.on('-p',nil,'[print] show source files')

      op.separator ''
      op.separator 'Basic Options'
      op.on('-h','--help','show help') do
        puts op.help
        exit
      end
    end

    opts = {}
    opt_parser.parse!(into: opts)

    if opts[:p]
      print_src
    else
      print_src_diff
    end
  end

  def print_src
    puts glob_all_src
  end

  def print_src_diff
    old_src = src_to_lines(read_cmake_src)
    new_src = src_to_lines(glob_all_src)

    matches = true
    i = 0
    j = 0

    while i < old_src.size || j < new_src.size
      if i >= old_src.size
        puts "    #{new_src[j]}"
        j += 1
        matches = false
        next
      end
      if j >= new_src.size
        puts "-   #{old_src[i]}"
        i += 1
        matches = false
        next
      end

      old_line = old_src[i]
      new_line = new_src[j]

      if old_line == new_line
        i += 1
        j += 1
        next
      end

      matches = false

      # Check if the src file has been either added or removed.
      is_new_src_file = true

      ((i + 1)...old_src.size).each do |k|
        if old_src[k] == new_line
          is_new_src_file = false
          break
        end
      end

      if is_new_src_file
        puts "    #{new_line}"
        j += 1
      else
        puts "-   #{old_line}"
        i += 1
      end
    end

    if matches
      puts "> No changes in '#{CMAKE_FILE}'."
    end
  end

  def glob_all_src
    str = ''.dup

    str << glob_src(TP_DIR,CMAKE_TP_DIR)
    str << "\n\n"
    str << glob_src(SRC_DIR,CMAKE_SRC_DIR) do |path1,path2|
      # Bubble Cybel files to top.
      is_cybel1 = path1.to_s.include?('cybel')
      is_cybel2 = path2.to_s.include?('cybel')

      if is_cybel1 && !is_cybel2
        -1
      elsif !is_cybel1 && is_cybel2
        1
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
    # - Sort by basename ignoring case.
    paths = paths.sort do |path1,path2|
      if block
        cmp = block.call(path1,path2) # rubocop:disable Performance/RedundantBlockCall
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
        sub_result = glob_src(
          path,macro,
          indent: indent,
          space_out_dirs: space_out_sub_dirs,
          space_out_sub_dirs: space_out_sub_dirs,
          root_dir_str: root_dir_str,
          &block
        )

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

  def read_cmake_src
    src = ''.dup
    data = File.read(CMAKE_FILE,mode: 'rt',encoding: 'BOM|UTF-8:UTF-8')

    data.scan(
      # `^func(...)$ ... ^)$`
      /(?<func_begin>^\s*#{Regexp.quote(CMAKE_FUNC)}\s*\([^\)]+?$)(?<src>.+?)(?<func_end>^\s*\)\s*$)/im
    ) do
      md = Regexp.last_match
      src << md[:src] << "\n"
    end

    return src
  end

  def src_to_lines(src)
    return src.split(/[\r\n]+/m)
              .map { |line| line.sub(/^\s*\#+/,'').strip }
              .reject(&:empty?)
  end
end

main if __FILE__ == $PROGRAM_NAME
