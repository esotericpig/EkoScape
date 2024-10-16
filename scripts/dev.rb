#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# For rapid development.
#
# Show usage:
#   $ ./scripts/dev.rb
#
# @version 0.1.3
# @author Bradley Whited
###

require 'optparse'
require 'shellwords'

def main
  DevApp.new.run
end

class DevApp
  CMAKE_CMD = %w[ cmake ].freeze

  def initialize
    @dry_run = true
    @preset = 'default'
    @config = 'Release'
    @extra_args = []
    @extra_build_args = []
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = '0.1.3'
      op.summary_width = 14

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
      op.on('-A',nil,"[AppImage] build AppImage w/ '#{@config}'")
      op.on('-P',nil,'[pkg] package up AppImage & files using CPack')
      op.separator ''
      op.on('-p <preset>','use <preset> for the preset') { |p| p.to_s.strip }
      op.on('-d',nil,"use 'Debug' config")
      op.on('-j [jobs]','set number of jobs; if no number, uses 1') do |j|
        j = j.to_s.strip
        j = j.empty? ? 1 : j.to_i
        @extra_build_args.push('-j',j)
        j
      end

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')

      op.separator ''
      op.separator 'Notes'
      op.separator "#{si}Any trailing options/args after '--' will be passed to the command directly:"
      op.separator "#{si}$ #{op.program_name} -c -- --warn-uninitialized"
    end

    args = ARGV
    dash_i = args.find_index('--')

    if !dash_i.nil?
      @extra_args = args[dash_i + 1..]
      args = args[0...dash_i]
    end

    opts = {}
    opt_parser.parse(args,into: opts)

    if opts.empty?
      puts opt_parser.help
      exit
    end

    @dry_run = opts[:n]
    @preset = opts[:p] unless opts[:p].nil? || opts[:p].empty?
    @config = 'Debug' if opts[:d]

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

  def build(target: nil)
    cmd = [CMAKE_CMD,'--build','--preset',@preset,'--config',@config,@extra_build_args]
    cmd.push('--target',target) unless target.nil?

    run_cmd(cmd)
  end

  def build_appimage
    build(target: 'appimage')
  end

  def pkg_up
    build(target: 'package')
  end

  def run_app
    build(target: 'run')
  end

  def run_cmd(*cmd)
    cmd += @extra_args
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map(&Shellwords.method(:escape)).join(' ')
    system(*cmd) unless @dry_run
  end
end

main if __FILE__ == $PROGRAM_NAME
