#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   $ ./artifacts.rb
#
# Get (download) artifacts using `gh` CLI:
#   $ ./artifacts.rb -g
#
# @version 0.1.0
# @author Bradley Whited
###

require 'fileutils'
require 'optparse'
require 'shellwords'

def main
  ArtifactsMan.new.run
end

class ArtifactsMan
  BUILD_DIR = 'build'
  ARTIFACTS_DIR = File.join(BUILD_DIR,'artifacts')
  ARTIFACT_NAMES = [
    'linux-appimage-x64',
    'macos-uni',
    'windows-x64',
  ].freeze

  GH_CMD = %w[ gh ].freeze

  def initialize
    @dry_run = true
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = '0.1.0'
      op.summary_width = 8

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-C',nil,"[clean] delete '#{ARTIFACTS_DIR}/'")
      op.on('-g',nil,"[get] download artifacts to '#{ARTIFACTS_DIR}/'")

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

    # Order matters! Because user can specify all opts.
    clean if opts[:C]
    get_artifacts if opts[:g]
  end

  def clean
    if File.directory?(ARTIFACTS_DIR)
      FileUtils.rmdir(ARTIFACTS_DIR,noop: @dry_run,verbose: true)
    else
      puts "Already gone: #{ARTIFACTS_DIR}"
    end
  end

  def get_artifacts
    FileUtils.mkdir_p(ARTIFACTS_DIR,noop: @dry_run,verbose: true) unless File.directory?(ARTIFACTS_DIR)

    # Download each one separately so that they don't create subdirs.
    ARTIFACT_NAMES.each do |name|
      run_cmd(GH_CMD,'run','download','--dir',ARTIFACTS_DIR,'--name',name)
    end
  end

  def run_cmd(*cmd)
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map(&Shellwords.method(:escape)).join(' ')
    system(*cmd) unless @dry_run
  end
end

main if __FILE__ == $PROGRAM_NAME
