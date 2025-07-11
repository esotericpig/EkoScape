#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# For rapid development.
#
# Show usage:
#   ./scripts/dev.rb
#
# Example usage:
#   # Use `-n` to only perform a dry-run.
#   # Use `-j [jobs]` to set the number of jobs in parallel.
#
#   # Configure build.
#   ./scripts/dev.rb -c
#
#   # Clean build files.
#   ./scripts/dev.rb -C
#
#   # Build Release or Debug.
#   ./scripts/dev.rb -b
#   ./scripts/dev.rb -b -d
#
#   # Run Release or Debug.
#   ./scripts/dev.rb -r
#   ./scripts/dev.rb -r -d
#
#   # Configure/Build/Run for Web.
#   ./scripts/dev.rb -c -w
#   ./scripts/dev.rb -b -w
#   ./scripts/dev.rb -r -w
#
#   # Check code quality.
#   ./scripts/dev.rb -k
#
#   # Build Linux AppImage.
#   ./scripts/dev.rb -A
#
#   # Package up.
#   ./scripts/dev.rb -P
#
# @author Bradley Whited
###

require 'optparse'
require 'shellwords'

def main
  DevApp.new.run
end

class DevApp
  VERSION = '0.1.9'

  CMAKE_CMD = %w[cmake].freeze

  def initialize
    @dry_run = true
    @preset = 'default'
    @config = 'Release'
    @extra_args = []
    @extra_build_args = {'-j' => 5}
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = VERSION
      op.summary_width = 16

      si = op.summary_indent

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-c',nil,"[config] configure build w/ preset '#{@preset}' for '#{@config}'")
      op.on('-C',nil,"[clean] clean '#{@config}'")
      op.on('-b',nil,"[build] build '#{@config}'")
      op.on('-r',nil,"[run] run '#{@config}'")
      op.separator ''
      op.on('-k',nil,'[check] check code quality (`cppcheck`)')
      op.on('-A',nil,'[AppImage] build AppImage (always uses Release)')
      op.on('-P',nil,'[pkg] package up AppImage & files using CPack')
      op.separator ''
      op.on('-j [jobs]','set number of jobs in parallel; if no number, uses 1') do |j|
        j = j.to_s.strip
        j = j.empty? ? 1 : j.to_i
        @extra_build_args['-j'] = j
        j
      end
      op.on('-d',nil,"use 'Debug' config")
      op.on('-w',nil,"use 'web' preset")
      op.on('-p <preset>','use <preset> for the preset') { |p| p.to_s.strip }

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')

      op.separator ''
      op.separator 'Notes'
      op.separator "#{si}# Any trailing options/args after '--' will be passed to the command directly:"
      op.separator "#{si}#{op.program_name} -c -- --warn-uninitialized"
    end

    args = ARGV
    dash_i = args.find_index('--')

    if !dash_i.nil?
      @extra_args = args[(dash_i + 1)..]
      args = args[0...dash_i]
    end

    opts = {}
    opt_parser.parse(args,into: opts)

    if opts.empty?
      puts opt_parser.help
      exit
    end

    @dry_run = opts[:n]
    @config = 'Debug' if opts[:d]

    if opts[:w]
      @preset = 'web'
    elsif !(opts[:p].nil? || opts[:p].empty?)
      @preset = opts[:p]
    end

    # Order matters! Because user can specify all opts.
    config_build if opts[:c]
    check_code if opts[:k]
    clean_build if opts[:C]
    build if opts[:b]
    build_appimage if opts[:A]
    pkg_up if opts[:P]
    run_app if opts[:r]
  end

  def config_build
    run_cmd(CMAKE_CMD,'--preset',@preset)
  end

  def check_code
    build(target: 'check')
  end

  def clean_build
    build(target: 'clean')
  end

  def build(target: nil,**kargs)
    extra = @extra_build_args.delete_if { |_k,v| v == false }
                             .transform_values { |v| (v == true) ? nil : v }
                             .to_a.flatten.compact
    cmd = [CMAKE_CMD,'--build','--preset',@preset,'--config',@config,*extra]
    cmd.push('--target',target) unless target.nil?

    run_cmd(cmd,**kargs)
  end

  def build_appimage
    build(target: 'appimage')
  end

  def pkg_up
    build(target: 'package')
  end

  def run_app
    # For the web run, we need to not use a subshell because of Ctrl+C to stop the server.
    build(target: 'run',subshell: false)
  end

  def run_cmd(*cmd,subshell: true)
    cmd += @extra_args
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map { |a| Shellwords.escape(a) }.join(' ')

    return if @dry_run

    if subshell
      system(*cmd,exception: true)
      puts # Separation between multiple command runs.
    else
      exec(*cmd)
    end
  end
end

main if __FILE__ == $PROGRAM_NAME
