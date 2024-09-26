#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   $ ./scripts/pkg_assets.rb
#
# Clean, package, & upload 'assets/' to GitHub as 'assets.zip':
#   $ ./scripts/pkg_assets.rb -c -p -g
#
# @version 0.1.0
# @author Bradley Whited
###

require 'fileutils'
require 'optparse'
require 'shellwords'

def main
  AssetsPkger.new.run
end

class AssetsPkger
  BUILD_DIR = 'build'
  ASSETS_DIR = 'assets'
  ASSETS_ZIP = File.join(BUILD_DIR,"#{ASSETS_DIR}.zip")

  ZIP_CMD = %w[ zip -r -9 -v -y ].freeze
  # Excludes must be at end of full command.
  ZIP_CMD_SUFFIX = ['-x','maps/*'].freeze
  GH_CMD = %w[ gh release upload --clobber v1.99 ].freeze

  def initialize
    @dry_run = true
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = '0.1.0'
      op.summary_width = 8

      # Opt indent.
      oi = op.summary_indent + (' ' * op.summary_width)

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-n',nil,'[no-clob] no-clobber dry run')
      op.on('-c',nil,"[clean] delete '#{ASSETS_ZIP}'")
      op.on('-p',nil,"[pkg] package up '#{ASSETS_DIR}/' into '#{ASSETS_ZIP}'" \
            "\n#{oi}       -- must have `#{ZIP_CMD[0]}` installed")
      op.on('-g',nil,'[gh] upload package to GitHub release (overwrites package)' \
            "\n#{oi}      -- must have `#{GH_CMD[0]}` installed")
    end

    opts = {}
    opt_parser.parse!(into: opts)
    @dry_run = opts[:n]

    if opts.empty? || (opts.length == 1 && @dry_run)
      puts opt_parser.help
      exit
    end

    # Order matters! Because user can specify all opts.
    clean if opts[:c]
    pkg_assets if opts[:p]
    upload_to_gh if opts[:g]
  end

  def clean
    FileUtils.rm(ASSETS_ZIP,noop: @dry_run,verbose: true) if File.file?(ASSETS_ZIP)
  end

  def pkg_assets
    FileUtils.mkdir(BUILD_DIR,noop: @dry_run,verbose: true) unless File.directory?(BUILD_DIR)

    # Need absolute path before cd().
    out_file = File.absolute_path(ASSETS_ZIP)

    FileUtils.cd(ASSETS_DIR,verbose: true) do
      run_cmd(*ZIP_CMD,out_file,'.',*ZIP_CMD_SUFFIX)
    end
  end

  def upload_to_gh
    run_cmd(*GH_CMD,ASSETS_ZIP)
  end

  def run_cmd(*cmd)
    cmd = cmd.flatten

    puts cmd.map(&Shellwords.method(:escape)).join(' ')
    system(*cmd) unless @dry_run
  end
end

main if __FILE__ == $PROGRAM_NAME
